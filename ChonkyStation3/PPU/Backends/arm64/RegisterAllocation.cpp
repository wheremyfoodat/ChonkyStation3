#include <PPU/Backends/arm64/PPUArm64.hpp>
#include <cassert>

#define GPR_OFFSET(i) ((uintptr_t) &state.gprs[i] - (uintptr_t)this)

// Map the guest register corresponding to the index to a host register
// Used internally by the allocateReg functions. Don't use it directly
template <PPUArm64::LoadingMode mode>
void PPUArm64::reserveReg(int index) {
    const auto regToAllocate = allocateableRegisters[allocatedRegisterCount];  // Fetch the next host reg to be allocated
    gprs[index].allocatedReg = regToAllocate;
    gprs[index].markUnknown();     // Mark the register's value as unknown if it were previously const propagated
    gprs[index].allocated = true;  // Mark register as allocated
    gprs[index].allocatedRegIndex = allocatedRegisterCount;

    // For certain instructions like loads, we don't want to load the reg because it'll get instantly overwritten
    if constexpr (mode == LoadingMode::Load) {
        LDR(regToAllocate, state_pointer, GPR_OFFSET(index));  // Load reg
    }

    hostGPRs[allocatedRegisterCount].mappedReg = index;
    allocatedRegisterCount++;  // Advance our register allcoator
}

// Flush constants and allocated registers to host regs at the end of a block
void PPUArm64::flushRegs() {
    for (auto i = 0; i < 32; i++) {
        if (gprs[i].isConst()) {  // If const: Write the value directly, mark as unknown. Possibly change when
                                    // constants will be stored in host regs
            if (gprs[i].val != 0) {
                MOV(X4, gprs[i].val);
                STR(X4, state_pointer, GPR_OFFSET(i));
            } else {
                STR(XZR, state_pointer, GPR_OFFSET(i));
            }

            gprs[i].markUnknown();
        }

        else if (gprs[i].isAllocated()) {  // If it's been allocated to a register, unallocate
            gprs[i].allocated = false;
            if (gprs[i].writeback) {  // And if writeback was specified, write the value back
                STR(gprs[i].allocatedReg, state_pointer, GPR_OFFSET(i));
                gprs[i].writeback = false;  // And turn writeback off
            }
        }
    }

    for (auto i = 0; i < ALLOCATEABLE_GPR_COUNT; i++) {  // Unallocate all regs
        hostGPRs[i].mappedReg = std::nullopt;
    }

    allocatedRegisterCount = 0;
}

// Spill the volatile allocated registers into guest registers in preparation for a call to a C++ function
void PPUArm64::prepareForCall() {
    const size_t non_volatile_count = allocateableNonVolatiles.size();

    if (allocatedRegisterCount > non_volatile_count) {  // Check if there's any allocated volatiles to flush
        for (auto i = non_volatile_count; i < allocatedRegisterCount; i++) {  // iterate volatile regs
            if (hostGPRs[i].mappedReg) {  // Unallocate and spill to guest regs as appropriate
                const auto previous = hostGPRs[i].mappedReg.value();  // Get previously allocated register
                if (gprs[previous].writeback) {                       // Spill to guest reg if writeback is enabled
                    STR(allocateableRegisters[i], state_pointer, GPR_OFFSET(previous));
                    gprs[previous].writeback = false;
                }

                gprs[previous].allocated = false;  // Unallocate it
                hostGPRs[i].mappedReg = std::nullopt;
            }
        }

        // Since we just flushed all our volatiles, we can perform an optimization by making the allocator start
        // allocating from the first volatile again. This makes it so we have to flush less often, as we free up
        // regs every time we call a C++ function instead of letting them linger and go to waste.
        allocatedRegisterCount = non_volatile_count;
    }
}

// Used when our register cache overflows. Spill the entirety of it to host registers.
void PPUArm64::spillRegisterCache() {
    for (auto i = 0; i < allocatedRegisterCount; i++) {
        if (hostGPRs[i].mappedReg) {  // Check if the register is still allocated to a guest register
            const auto previous = hostGPRs[i].mappedReg.value();  // Get the reg it's allocated to

            if (gprs[previous].writeback) {  // Spill to guest register if writeback is enabled and disable writeback
                STR(allocateableRegisters[i], state_pointer, GPR_OFFSET(previous));
                gprs[previous].writeback = false;
            }

            hostGPRs[i].mappedReg = std::nullopt;  // Unallocate it
            gprs[previous].allocated = false;
        }
    }

    allocatedRegisterCount = 0;  // Nothing is allocated anymore
}

void PPUArm64::allocateReg(int reg) {
    if (!gprs[reg].isAllocated()) {
        if (allocatedRegisterCount >= ALLOCATEABLE_GPR_COUNT) {
            spillRegisterCache();
        }
        reserveReg<LoadingMode::Load>(reg);
    }
}

void PPUArm64::allocateRegWithoutLoad(int reg) {
    if (!gprs[reg].isAllocated()) {
        if (allocatedRegisterCount >= ALLOCATEABLE_GPR_COUNT) {
            spillRegisterCache();
        }
        reserveReg<LoadingMode::DoNotLoad>(reg);
    }
}

// T: Number of regs without writeback we must allocate
// U: Number of regs with writeback we must allocate
// We want both of them to be compile-time constants for efficiency
template <size_t T, size_t U>
void PPUArm64::allocateRegisters(std::array<uint, T> regsWithoutWb, std::array<uint, U> regsWithWb) {
    static_assert(T + U < ALLOCATEABLE_GPR_COUNT, "Trying to allocate too many registers");

start:
    // Which specific regs we need to load
    uint32_t regsToLoad = 0;
    // Which specific regs we need to allocate without loading, with writeback
    uint32_t regsToWriteback = 0;
    // How many registers we need to load
    int regsToAllocateCount = 0;

    for (int i = 0; i < T; i++) {
        const auto reg = regsWithoutWb[i];
        if (!gprs[reg].allocated && (regsToLoad & (1 << reg)) == 0) {
            regsToLoad |= 1 << reg;
            regsToAllocateCount++;
        }
    }

    for (int i = 0; i < U; i++) {
        const auto reg = regsWithWb[i];
        if (!gprs[reg].allocated && (regsToWriteback & (1 << reg)) == 0 && (regsToLoad & (1 << reg)) == 0) {
            regsToWriteback |= 1 << reg;
            regsToAllocateCount++;
        }
    }

    if (regsToAllocateCount != 0) {
        // Flush register cache if we're going to overflow it and restart alloc process
        if (allocatedRegisterCount + regsToAllocateCount >= ALLOCATEABLE_GPR_COUNT) {
            flushRegs();
            goto start;
        }

        // Check which registers we need to load
        for (int i = 0; i < T; i++) {
            const auto reg = regsWithoutWb[i];
            if ((regsToLoad & (1 << reg)) != 0 && !gprs[reg].allocated) {
                reserveReg<LoadingMode::Load>(reg);
            }
        }
    }

    // Specify writeback for whatever regs we need to
    for (int i = 0; i < U; i++) {
        const auto reg = regsWithWb[i];
        if (!gprs[reg].allocated) {
            reserveReg<LoadingMode::DoNotLoad>(reg);
        }
        gprs[reg].writeback = true;
    }
}

void PPUArm64::alloc_rs_wb_rt(Instruction instr) {
    allocateRegisters<1, 1>({instr.rs.Value()}, {instr.rt.Value()});
}

void PPUArm64::alloc_ra_wb_rt(Instruction instr) {
    allocateRegisters<1, 1>({instr.ra.Value()}, {instr.rt.Value()});
}

void PPUArm64::alloc_rs_rb_wb_ra(Instruction instr) {
    allocateRegisters<2, 1>({instr.rs.Value(), instr.rb.Value()}, {instr.ra.Value()});
}

void PPUArm64::alloc_ra_rb_wb_rt(Instruction instr) {
    allocateRegisters<2, 1>({instr.ra.Value(), instr.rb.Value()}, {instr.rt.Value()});
}

void PPUArm64::pushNonVolatiles() {
    const int non_volatile_count = int(allocateableNonVolatiles.size());
    
    assert((non_volatile_count & 1) == 0);  // Make sure we've got an even number of regs to push
    for (int i = 0; i < non_volatile_count; i += 2) {
        const auto reg1 = allocateableNonVolatiles[i];
        const auto reg2 = allocateableNonVolatiles[i + 1];
        STP(reg1, reg2, SP, PRE_INDEXED, -16);
    }
}

void PPUArm64::popNonVolatiles() {
    const int non_volatile_count = int(allocateableNonVolatiles.size());
    
    assert((non_volatile_count & 1) == 0);  // Make sure we've got an even number of regs to push
    for (int i = non_volatile_count - 1; i >= 0; i -= 2) {
        const auto reg1 = allocateableNonVolatiles[i];
        const auto reg2 = allocateableNonVolatiles[i - 1];
        LDP(reg2, reg1, SP, POST_INDEXED, 16);
    }
}