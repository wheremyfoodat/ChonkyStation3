#include "PPUArm64.hpp"
#include <PlayStation3.hpp>

#define PC_OFFSET ((uintptr_t) &state.pc - (uintptr_t)this)

PPUArm64::PPUArm64(Memory& mem, PlayStation3* ps3) : PPU(mem, ps3), interpreter(mem, ps3, stateInternal),
    oaknut::CodeBlock(alloc_size), oaknut::CodeGenerator(oaknut::CodeBlock::ptr()) {
    
    code_blocks = new JITCallback*[0x1'0000'0000ull / page_size]();
    emitDispatcher();
}

void PPUArm64::emitDispatcher() {
    align(16);
    dispatcher = xptr<JITCallback>();
    // TODO

    emitBlockLookup();
}

void PPUArm64::emitBlockLookup() {
    // TODO
}

PPUArm64::JITCallback PPUArm64::compileBlock() {
    oaknut::CodeBlock::unprotect();
    
    align(16);
    JITCallback cb = xptr<JITCallback>();

    jump(&PPUArm64::runInterpreterThunk, X4);

    // Mark memory as RX and invalidate affect icache ranges before returning and executing the block
    oaknut::CodeBlock::protect();
	oaknut::CodeBlock::invalidate((u32*)cb, uintptr_t(xptr<u32*>()) - uintptr_t(cb));
    return cb;
}

int PPUArm64::step() {
    // TODO: Make lookup faster & port to assembly
    auto& page = code_blocks[state.pc >> page_shift];
    if (page == nullptr) {
        page = new JITCallback[page_size >> 2]();
    }

    auto& func = page[(state.pc & page_mask) >> 2];
    if (!func) {
        func = compileBlock();
    }

    return func(this);
}

