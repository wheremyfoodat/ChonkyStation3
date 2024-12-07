#include "PPUDisassembler.hpp"


void PPUDisassembler::disasm(PPUTypes::State& state, PPUTypes::Instruction instr) {
    switch (instr.opc) {
    
    case PPUTypes::Instructions::ADDI:		printf("0x%016llx | addi  r%d, r%d, 0x%04x\n", state.pc, (u8)instr.rt, (u8)instr.ra, (u8)instr.si); break;
    case PPUTypes::Instructions::ORI:		printf("0x%016llx | ori   r%d, r%d, 0x%04x\n", state.pc, (u8)instr.ra, (u8)instr.rs, (u8)instr.ui); break;
    case PPUTypes::Instructions::ORIS:		printf("0x%016llx | oris  r%d, r%d, 0x%04x\n", state.pc, (u8)instr.ra, (u8)instr.rs, (u8)instr.ui); break;
    case PPUTypes::Instructions::XORI:		printf("0x%016llx | xori  r%d, r%d, 0x%04x\n", state.pc, (u8)instr.ra, (u8)instr.rs, (u8)instr.ui); break;
    case PPUTypes::Instructions::XORIS:		printf("0x%016llx | xoris r%d, r%d, 0x%04x\n", state.pc, (u8)instr.ra, (u8)instr.rs, (u8)instr.ui); break;
    case PPUTypes::Instructions::ANDI:		printf("0x%016llx | andi  r%d, r%d, 0x%04x\n", state.pc, (u8)instr.ra, (u8)instr.rs, (u8)instr.ui); break;
    case PPUTypes::Instructions::ANDIS:		printf("0x%016llx | andis r%d, r%d, 0x%04x\n", state.pc, (u8)instr.ra, (u8)instr.rs, (u8)instr.ui); break;
    case PPUTypes::Instructions::LWZ:		printf("0x%016llx | lwz   r%d, %d(r%d)			; r%d <- [0x%08llx]\n", state.pc, (u8)instr.rt, (s32)(s16)instr.d, (u8)instr.ra, (u8)instr.rt, (instr.ra == 0) ? (s32)(s16)instr.d : state.gprs[instr.ra] + (s32)(s16)instr.d); break;
    case PPUTypes::Instructions::G_3E: {
        switch (instr.g_3e_field) {
        case PPUTypes::G_3EOpcodes::STD:	printf("0x%016llx | std   r%d, %d(r%d)			; [0x%08llx] <- r%d\n", state.pc, (u8)instr.rs, (s32)(s16)(instr.ds << 2), (u8)instr.ra, (s32)(s16)(instr.ds << 2) + state.gprs[instr.ra], (u8)instr.rs); break;
        case PPUTypes::G_3EOpcodes::STDU:	printf("0x%016llx | stdu  r%d, %d(r%d)			; [0x%08llx] <- r%d\n", state.pc, (u8)instr.rs, (s32)(s16)(instr.ds << 2), (u8)instr.ra, (s32)(s16)(instr.ds << 2) + state.gprs[instr.ra], (u8)instr.rs); break;
        }
        break;
    }
    
    default: printf("0x%016llx | unknown 0x%02x\n", state.pc, (u8)instr.opc); break;
    }
}