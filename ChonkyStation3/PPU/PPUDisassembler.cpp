#include "PPUDisassembler.hpp"

#include <array>
#include <span>
#include <string>

Helpers::CapstoneDisassembler PPUDisassembler::capstone;

void PPUDisassembler::disasm(PPUTypes::State& state, PPUTypes::Instruction instr, Memory* mem) {
    auto capstoneDisasm = [](u32 instr, u32 pc) {
        if (!capstone.isInitialized()) {
            capstone.init(CS_ARCH_PPC, cs_mode(CS_MODE_64 | CS_MODE_BIG_ENDIAN | CS_MODE_PWR7));
        }

        std::string disassembly;
        // Convert instruction to byte array to pass to Capstone
        std::array<u8, 4> bytes = {
            u8((instr >> 24) & 0xff),
            u8((instr >> 16) & 0xff),
            u8((instr >> 8) & 0xff),
            u8(instr & 0xff),
        };
        capstone.disassemble(disassembly, pc, std::span(bytes));
        printf("0x%016llx | %s                         ; (0x%08x via Capstone)\n", pc, disassembly.c_str(), instr);
    };

    switch (instr.opc) {
    
    case PPUTypes::Instructions::ADDI:      printf("0x%016llx | addi       r%d, r%d, 0x%04x\n", state.pc, (u8)instr.rt, (u8)instr.ra, (u8)instr.si); break;
    case PPUTypes::Instructions::ADDIS:     printf("0x%016llx | addis      r%d, r%d, 0x%04x\n", state.pc, (u8)instr.rt, (u8)instr.ra, (u8)instr.si); break;
    case PPUTypes::Instructions::BC:        printf("0x%016llx | bc%c%c%c       %d, %d, 0x%08x		; %s\n", state.pc, instr.lk ? 'l' : (instr.aa ? '\0' : ' '), instr.aa ? 'a' : ' ', (instr.aa && !instr.lk) ? ' ' : '\0', (u8)instr.bo, (u8)instr.bi, (s32)(s16)(instr.bd << 2), branchCondition(instr.bo, instr.bi, state) ? "taken" : "not taken");  break;
    case PPUTypes::Instructions::SC:        printf("0x%016llx | sc\n", state.pc); break;
    case PPUTypes::Instructions::B:         printf("0x%016llx | b%c%c%c        0x%08x\n", state.pc, instr.lk ? 'l' : (instr.aa ? '\0' : ' '), instr.aa ? 'a' : ' ', (instr.aa && !instr.lk) ? ' ' : '\0', (s32)(instr.li << 8) >> 6);  break;   // What the fuck did I just do
    case PPUTypes::Instructions::G_13: {
        switch (instr.g_13_field) {

        case PPUTypes::G_13Opcodes::BCLR:   printf("0x%016llx | bclr%c      %d, %d  				; %s\n", state.pc, instr.lk ? 'l' : ' ', (u8)instr.bo, (u8)instr.bi, branchCondition(instr.bo, instr.bi, state) ? "taken" : "not taken");    break;
        case PPUTypes::G_13Opcodes::BCCTR:  printf("0x%016llx | bcctr%c     %d, %d  				; %s\n", state.pc, instr.lk ? 'l' : ' ', (u8)instr.bo, (u8)instr.bi, branchCondition(instr.bo, instr.bi, state) ? "taken" : "not taken");    break;

        default: capstoneDisasm(instr.raw, state.pc); break;
        }
        break;
    }
    case PPUTypes::Instructions::ORI:       printf("0x%016llx | ori        r%d, r%d, 0x%04x\n", state.pc, (u8)instr.ra, (u8)instr.rs, (u16)instr.ui); break;
    case PPUTypes::Instructions::ORIS:      printf("0x%016llx | oris       r%d, r%d, 0x%04x 		; r%d = 0x%016llx\n", state.pc, (u8)instr.ra, (u8)instr.rs, (u8)instr.ui, (u8)instr.rs, state.gprs[instr.rs]); break;
    case PPUTypes::Instructions::XORI:      printf("0x%016llx | xori       r%d, r%d, 0x%04x\n", state.pc, (u8)instr.ra, (u8)instr.rs, (u16)instr.ui); break;
    case PPUTypes::Instructions::XORIS:     printf("0x%016llx | xoris      r%d, r%d, 0x%04x\n", state.pc, (u8)instr.ra, (u8)instr.rs, (u16)instr.ui); break;
    case PPUTypes::Instructions::ANDI:      printf("0x%016llx | andi       r%d, r%d, 0x%04x\n", state.pc, (u8)instr.ra, (u8)instr.rs, (u16)instr.ui); break;
    case PPUTypes::Instructions::ANDIS:     printf("0x%016llx | andis      r%d, r%d, 0x%04x\n", state.pc, (u8)instr.ra, (u8)instr.rs, (u16)instr.ui); break;
    case PPUTypes::Instructions::G_1E: {
        switch (instr.g_1e_field) {

        case PPUTypes::G_1EOpcodes::RLDICL: printf("0x%016llx | rldicl%c    r%d, r%d, %d, %d\n", state.pc, instr.rc ? '.' : ' ', (u8)instr.ra, (u8)instr.rs, instr.sh_lo | (instr.sh_hi << 5), ((instr.mb_6 & 1) << 5) | (instr.mb_6 >> 1));    break;
        case PPUTypes::G_1EOpcodes::RLDICR: printf("0x%016llx | rldicr%c    r%d, r%d, %d, %d\n", state.pc, instr.rc ? '.' : ' ', (u8)instr.ra, (u8)instr.rs, instr.sh_lo | (instr.sh_hi << 5), ((instr.mb_6 & 1) << 5) | (instr.mb_6 >> 1));    break;

        default: capstoneDisasm(instr.raw, state.pc); break;
        }
        break;
    }
    case PPUTypes::Instructions::G_1F: {
        switch (instr.g_1f_field) {
        
        case PPUTypes::G_1FOpcodes::CMP:    printf("0x%016llx | cmp        %d, %d, r%d, r%d\n", state.pc, (u8)instr.bf, (u8)instr.l, (u8)instr.ra, (u8)instr.rb);   break;
        case PPUTypes::G_1FOpcodes::CMPL:   printf("0x%016llx | cmpl       %d, %d, r%d, r%d\n", state.pc, (u8)instr.bf, (u8)instr.l, (u8)instr.ra, (u8)instr.rb);   break;
        case PPUTypes::G_1FOpcodes::ADD:    printf("0x%016llx | add%c       r%d, r%d, r%d\n", state.pc, instr.rc ? '.' : ' ', (u8)instr.rt, (u8)instr.ra, (u8)instr.rb);   break;
        case PPUTypes::G_1FOpcodes::MFSPR:  printf("0x%016llx | mfspr\n", state.pc);    break;
        case PPUTypes::G_1FOpcodes::OR:     printf("0x%016llx | or%c        r%d, r%d, r%d\n", state.pc, instr.rc ? '.' : ' ', (u8)instr.ra, (u8)instr.rs, (u8)instr.rb);   break;
        case PPUTypes::G_1FOpcodes::MTSPR:  printf("0x%016llx | mtspr\n", state.pc);    break;
        case PPUTypes::G_1FOpcodes::EXTSH:  printf("0x%016llx | extsh%c     r%d, r%d\n", state.pc, instr.rc ? '.' : ' ', (u8)instr.ra, (u8)instr.rs);   break;
        case PPUTypes::G_1FOpcodes::EXTSW:  printf("0x%016llx | extsw%c     r%d, r%d\n", state.pc, instr.rc ? '.' : ' ', (u8)instr.ra, (u8)instr.rs);   break;

        default: capstoneDisasm(instr.raw, state.pc); break;
        }
        break;
    }
    case PPUTypes::Instructions::LWZ:       printf("0x%016llx | lwz        r%d, %d(r%d)			; r%d <- [0x%08llx]\n", state.pc, (u8)instr.rt, (s32)(s16)instr.d, (u8)instr.ra, (u8)instr.rt, (instr.ra == 0) ? (s32)(s16)instr.d : state.gprs[instr.ra] + (s32)(s16)instr.d); break;
    case PPUTypes::Instructions::STW:       printf("0x%016llx | stw        r%d, %d(r%d)			; [0x%08llx] <- r%d\n", state.pc, (u8)instr.rs, (s32)(s16)instr.d, (u8)instr.ra, (instr.ra == 0) ? (s32)(s16)instr.d : state.gprs[instr.ra] + (s32)(s16)instr.d, (u8)instr.rs); break;
    case PPUTypes::Instructions::LHZ:       printf("0x%016llx | lhz        r%d, %d(r%d)			; r%d <- [0x%08llx]\n", state.pc, (u8)instr.rt, (s32)(s16)instr.d, (u8)instr.ra, (u8)instr.rt, (instr.ra == 0) ? (s32)(s16)instr.d : state.gprs[instr.ra] + (s32)(s16)instr.d); break;
    case PPUTypes::Instructions::G_3A: {
        switch (instr.g_3a_field) {

        case PPUTypes::G_3AOpcodes::LD:    printf("0x%016llx | ld         r%d, %d(r%d)			; [0x%08llx] <- r%d\n", state.pc, (u8)instr.rt, (s32)(s16)(instr.ds << 2), (u8)instr.ra, (instr.ra == 0) ? (s32)(s16)(instr.ds << 2) : (s32)(s16)(instr.ds << 2) + state.gprs[instr.ra], (u8)instr.rs); break;
        case PPUTypes::G_3AOpcodes::LDU:   printf("0x%016llx | ldu        r%d, %d(r%d)			; [0x%08llx] <- r%d\n", state.pc, (u8)instr.rt, (s32)(s16)(instr.ds << 2), (u8)instr.ra, (s32)(s16)(instr.ds << 2) + state.gprs[instr.ra], (u8)instr.rs); break;
        default: capstoneDisasm(instr.raw, state.pc); break;

        }
        break;
    }
    case PPUTypes::Instructions::G_3E: {
        switch (instr.g_3e_field) {

        case PPUTypes::G_3EOpcodes::STD:    printf("0x%016llx | std        r%d, %d(r%d)			; [0x%08llx] <- r%d\n", state.pc, (u8)instr.rs, (s32)(s16)(instr.ds << 2), (u8)instr.ra, (instr.ra == 0) ? (s32)(s16)(instr.ds << 2) : (s32)(s16)(instr.ds << 2) + state.gprs[instr.ra], (u8)instr.rs); break;
        case PPUTypes::G_3EOpcodes::STDU:   printf("0x%016llx | stdu       r%d, %d(r%d)			; [0x%08llx] <- r%d\n", state.pc, (u8)instr.rs, (s32)(s16)(instr.ds << 2), (u8)instr.ra, (s32)(s16)(instr.ds << 2) + state.gprs[instr.ra], (u8)instr.rs); break;
        default: capstoneDisasm(instr.raw, state.pc); break;

        }
        break;
    }
    
    default: capstoneDisasm(instr.raw, state.pc); break;
    }
}