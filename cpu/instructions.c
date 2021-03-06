#include "instructions.h"
#include "helper.h"
#include "../common/log.h"
#include "../common/bitwise.h"

const CPUInstruction lookup[0x10][0x10] = {
        {{"NOP", 1, 4, ins_00}, {"LD BC,d16", 3, 12, ins_01}, {"LD (BC),A", 1, 8, ins_02}, {"INC BC", 1, 8, ins_03}, {"INC B", 1, 4, ins_04}, {"DEC B", 1, 4, ins_05}, {"LD B,d8", 2, 8, ins_06}, {"RLCA", 1, 4, ins_07}, {"LD (a16),SP", 3, 20, ins_08}, {"ADD HL,BC", 1, 8, ins_09}, {"LD A,(BC)", 1, 8, ins_0A}, {"DEC BC", 1, 8, ins_0B}, {"INC C", 1, 4, ins_0C}, {"DEC C", 1, 4, ins_0D}, {"LD C,d8", 2, 8, ins_0E}, {"RRCA", 1, 4, ins_0F}, },
        {{"STOP d8", 2, 4, ins_10}, {"LD DE,d16", 3, 12, ins_11}, {"LD (DE),A", 1, 8, ins_12}, {"INC DE", 1, 8, ins_13}, {"INC D", 1, 4, ins_14}, {"DEC D", 1, 4, ins_15}, {"LD D,d8", 2, 8, ins_16}, {"RLA", 1, 4, ins_17}, {"JR r8", 2, 12, ins_18}, {"ADD HL,DE", 1, 8, ins_19}, {"LD A,(DE)", 1, 8, ins_1A}, {"DEC DE", 1, 8, ins_1B}, {"INC E", 1, 4, ins_1C}, {"DEC E", 1, 4, ins_1D}, {"LD E,d8", 2, 8, ins_1E}, {"RRA", 1, 4, ins_1F}, },
        {{"JR NZ,r8", 2, 12, ins_20}, {"LD HL,d16", 3, 12, ins_21}, {"LD (HL+),A", 1, 8, ins_22}, {"INC HL", 1, 8, ins_23}, {"INC H", 1, 4, ins_24}, {"DEC H", 1, 4, ins_25}, {"LD H,d8", 2, 8, ins_26}, {"DAA", 1, 4, ins_27}, {"JR Z,r8", 2, 12, ins_28}, {"ADD HL,HL", 1, 8, ins_29}, {"LD A,(HL+)", 1, 8, ins_2A}, {"DEC HL", 1, 8, ins_2B}, {"INC L", 1, 4, ins_2C}, {"DEC L", 1, 4, ins_2D}, {"LD L,d8", 2, 8, ins_2E}, {"CPL", 1, 4, ins_2F}, },
        {{"JR NC,r8", 2, 12, ins_30}, {"LD SP,d16", 3, 12, ins_31}, {"LD (HL-),A", 1, 8, ins_32}, {"INC SP", 1, 8, ins_33}, {"INC (HL)", 1, 12, ins_34}, {"DEC (HL)", 1, 12, ins_35}, {"LD (HL),d8", 2, 12, ins_36}, {"SCF", 1, 4, ins_37}, {"JR C,r8", 2, 12, ins_38}, {"ADD HL,SP", 1, 8, ins_39}, {"LD A,(HL-)", 1, 8, ins_3A}, {"DEC SP", 1, 8, ins_3B}, {"INC A", 1, 4, ins_3C}, {"DEC A", 1, 4, ins_3D}, {"LD A,d8", 2, 8, ins_3E}, {"CCF", 1, 4, ins_3F}, },
        {{"LD B,B", 1, 4, ins_40}, {"LD B,C", 1, 4, ins_41}, {"LD B,D", 1, 4, ins_42}, {"LD B,E", 1, 4, ins_43}, {"LD B,H", 1, 4, ins_44}, {"LD B,L", 1, 4, ins_45}, {"LD B,(HL)", 1, 8, ins_46}, {"LD B,A", 1, 4, ins_47}, {"LD C,B", 1, 4, ins_48}, {"LD C,C", 1, 4, ins_49}, {"LD C,D", 1, 4, ins_4A}, {"LD C,E", 1, 4, ins_4B}, {"LD C,H", 1, 4, ins_4C}, {"LD C,L", 1, 4, ins_4D}, {"LD C,(HL)", 1, 8, ins_4E}, {"LD C,A", 1, 4, ins_4F}, },
        {{"LD D,B", 1, 4, ins_50}, {"LD D,C", 1, 4, ins_51}, {"LD D,D", 1, 4, ins_52}, {"LD D,E", 1, 4, ins_53}, {"LD D,H", 1, 4, ins_54}, {"LD D,L", 1, 4, ins_55}, {"LD D,(HL)", 1, 8, ins_56}, {"LD D,A", 1, 4, ins_57}, {"LD E,B", 1, 4, ins_58}, {"LD E,C", 1, 4, ins_59}, {"LD E,D", 1, 4, ins_5A}, {"LD E,E", 1, 4, ins_5B}, {"LD E,H", 1, 4, ins_5C}, {"LD E,L", 1, 4, ins_5D}, {"LD E,(HL)", 1, 8, ins_5E}, {"LD E,A", 1, 4, ins_5F}, },
        {{"LD H,B", 1, 4, ins_60}, {"LD H,C", 1, 4, ins_61}, {"LD H,D", 1, 4, ins_62}, {"LD H,E", 1, 4, ins_63}, {"LD H,H", 1, 4, ins_64}, {"LD H,L", 1, 4, ins_65}, {"LD H,(HL)", 1, 8, ins_66}, {"LD H,A", 1, 4, ins_67}, {"LD L,B", 1, 4, ins_68}, {"LD L,C", 1, 4, ins_69}, {"LD L,D", 1, 4, ins_6A}, {"LD L,E", 1, 4, ins_6B}, {"LD L,H", 1, 4, ins_6C}, {"LD L,L", 1, 4, ins_6D}, {"LD L,(HL)", 1, 8, ins_6E}, {"LD L,A", 1, 4, ins_6F}, },
        {{"LD (HL),B", 1, 8, ins_70}, {"LD (HL),C", 1, 8, ins_71}, {"LD (HL),D", 1, 8, ins_72}, {"LD (HL),E", 1, 8, ins_73}, {"LD (HL),H", 1, 8, ins_74}, {"LD (HL),L", 1, 8, ins_75}, {"HALT", 1, 4, ins_76}, {"LD (HL),A", 1, 8, ins_77}, {"LD A,B", 1, 4, ins_78}, {"LD A,C", 1, 4, ins_79}, {"LD A,D", 1, 4, ins_7A}, {"LD A,E", 1, 4, ins_7B}, {"LD A,H", 1, 4, ins_7C}, {"LD A,L", 1, 4, ins_7D}, {"LD A,(HL)", 1, 8, ins_7E}, {"LD A,A", 1, 4, ins_7F}, },
        {{"ADD A,B", 1, 4, ins_80}, {"ADD A,C", 1, 4, ins_81}, {"ADD A,D", 1, 4, ins_82}, {"ADD A,E", 1, 4, ins_83}, {"ADD A,H", 1, 4, ins_84}, {"ADD A,L", 1, 4, ins_85}, {"ADD A,(HL)", 1, 8, ins_86}, {"ADD A,A", 1, 4, ins_87}, {"ADC A,B", 1, 4, ins_88}, {"ADC A,C", 1, 4, ins_89}, {"ADC A,D", 1, 4, ins_8A}, {"ADC A,E", 1, 4, ins_8B}, {"ADC A,H", 1, 4, ins_8C}, {"ADC A,L", 1, 4, ins_8D}, {"ADC A,(HL)", 1, 8, ins_8E}, {"ADC A,A", 1, 4, ins_8F}, },
        {{"SUB B", 1, 4, ins_90}, {"SUB C", 1, 4, ins_91}, {"SUB D", 1, 4, ins_92}, {"SUB E", 1, 4, ins_93}, {"SUB H", 1, 4, ins_94}, {"SUB L", 1, 4, ins_95}, {"SUB (HL)", 1, 8, ins_96}, {"SUB A", 1, 4, ins_97}, {"SBC A,B", 1, 4, ins_98}, {"SBC A,C", 1, 4, ins_99}, {"SBC A,D", 1, 4, ins_9A}, {"SBC A,E", 1, 4, ins_9B}, {"SBC A,H", 1, 4, ins_9C}, {"SBC A,L", 1, 4, ins_9D}, {"SBC A,(HL)", 1, 8, ins_9E}, {"SBC A,A", 1, 4, ins_9F}, },
        {{"AND B", 1, 4, ins_A0}, {"AND C", 1, 4, ins_A1}, {"AND D", 1, 4, ins_A2}, {"AND E", 1, 4, ins_A3}, {"AND H", 1, 4, ins_A4}, {"AND L", 1, 4, ins_A5}, {"AND (HL)", 1, 8, ins_A6}, {"AND A", 1, 4, ins_A7}, {"XOR B", 1, 4, ins_A8}, {"XOR C", 1, 4, ins_A9}, {"XOR D", 1, 4, ins_AA}, {"XOR E", 1, 4, ins_AB}, {"XOR H", 1, 4, ins_AC}, {"XOR L", 1, 4, ins_AD}, {"XOR (HL)", 1, 8, ins_AE}, {"XOR A", 1, 4, ins_AF}, },
        {{"OR B", 1, 4, ins_B0}, {"OR C", 1, 4, ins_B1}, {"OR D", 1, 4, ins_B2}, {"OR E", 1, 4, ins_B3}, {"OR H", 1, 4, ins_B4}, {"OR L", 1, 4, ins_B5}, {"OR (HL)", 1, 8, ins_B6}, {"OR A", 1, 4, ins_B7}, {"CP B", 1, 4, ins_B8}, {"CP C", 1, 4, ins_B9}, {"CP D", 1, 4, ins_BA}, {"CP E", 1, 4, ins_BB}, {"CP H", 1, 4, ins_BC}, {"CP L", 1, 4, ins_BD}, {"CP (HL)", 1, 8, ins_BE}, {"CP A", 1, 4, ins_BF}, },
        {{"RET NZ", 1, 20, ins_C0}, {"POP BC", 1, 12, ins_C1}, {"JP NZ,a16", 3, 16, ins_C2}, {"JP a16", 3, 16, ins_C3}, {"CALL NZ,a16", 3, 24, ins_C4}, {"PUSH BC", 1, 16, ins_C5}, {"ADD A,d8", 2, 8, ins_C6}, {"RST 00H", 1, 16, ins_C7}, {"RET Z", 1, 20, ins_C8}, {"RET", 1, 16, ins_C9}, {"JP Z,a16", 3, 16, ins_CA}, {"PREFIX", 1, 4, ins_CB}, {"CALL Z,a16", 3, 24, ins_CC}, {"CALL a16", 3, 24, ins_CD}, {"ADC A,d8", 2, 8, ins_CE}, {"RST 08H", 1, 16, ins_CF}, },
        {{"RET NC", 1, 20, ins_D0}, {"POP DE", 1, 12, ins_D1}, {"JP NC,a16", 3, 16, ins_D2}, {"ILLEGAL_D3", 1, 4, ins_D3}, {"CALL NC,a16", 3, 24, ins_D4}, {"PUSH DE", 1, 16, ins_D5}, {"SUB d8", 2, 8, ins_D6}, {"RST 10H", 1, 16, ins_D7}, {"RET C", 1, 20, ins_D8}, {"RETI", 1, 16, ins_D9}, {"JP C,a16", 3, 16, ins_DA}, {"ILLEGAL_DB", 1, 4, ins_DB}, {"CALL C,a16", 3, 24, ins_DC}, {"ILLEGAL_DD", 1, 4, ins_DD}, {"SBC A,d8", 2, 8, ins_DE}, {"RST 18H", 1, 16, ins_DF}, },
        {{"LDH (a8),A", 2, 12, ins_E0}, {"POP HL", 1, 12, ins_E1}, {"LD ($FF00+C),A", 1, 8, ins_E2}, {"ILLEGAL_E3", 1, 4, ins_E3}, {"ILLEGAL_E4", 1, 4, ins_E4}, {"PUSH HL", 1, 16, ins_E5}, {"AND d8", 2, 8, ins_E6}, {"RST 20H", 1, 16, ins_E7}, {"ADD SP,r8", 2, 16, ins_E8}, {"JP HL", 1, 4, ins_E9}, {"LD (a16),A", 3, 16, ins_EA}, {"ILLEGAL_EB", 1, 4, ins_EB}, {"ILLEGAL_EC", 1, 4, ins_EC}, {"ILLEGAL_ED", 1, 4, ins_ED}, {"XOR d8", 2, 8, ins_EE}, {"RST 28H", 1, 16, ins_EF}, },
        {{"LDH A,(a8)", 2, 12, ins_F0}, {"POP AF", 1, 12, ins_F1}, {"LD A,($FF00+C)", 1, 8, ins_F2}, {"DI", 1, 4, ins_F3}, {"ILLEGAL_F4", 1, 4, ins_F4}, {"PUSH AF", 1, 16, ins_F5}, {"OR d8", 2, 8, ins_F6}, {"RST 30H", 1, 16, ins_F7}, {"LD HL,SP+r8", 2, 12, ins_F8}, {"LD SP,HL", 1, 8, ins_F9}, {"LD A,(a16)", 3, 16, ins_FA}, {"EI", 1, 4, ins_FB}, {"ILLEGAL_FC", 1, 4, ins_FC}, {"ILLEGAL_FD", 1, 4, ins_FD}, {"CP d8", 2, 8, ins_FE}, {"RST 38H", 1, 16, ins_FF}}
};

const CPUInstruction lookup_cb[0x10][0x10] = {
        {{"RLC B", 2, 8, ins_CB_00}, {"RLC C", 2, 8, ins_CB_01}, {"RLC D", 2, 8, ins_CB_02}, {"RLC E", 2, 8, ins_CB_03}, {"RLC H", 2, 8, ins_CB_04}, {"RLC L", 2, 8, ins_CB_05}, {"RLC (HL)", 2, 16, ins_CB_06}, {"RLC A", 2, 8, ins_CB_07}, {"RRC B", 2, 8, ins_CB_08}, {"RRC C", 2, 8, ins_CB_09}, {"RRC D", 2, 8, ins_CB_0A}, {"RRC E", 2, 8, ins_CB_0B}, {"RRC H", 2, 8, ins_CB_0C}, {"RRC L", 2, 8, ins_CB_0D}, {"RRC (HL)", 2, 16, ins_CB_0E}, {"RRC A", 2, 8, ins_CB_0F}, },
        {{"RL B", 2, 8, ins_CB_10}, {"RL C", 2, 8, ins_CB_11}, {"RL D", 2, 8, ins_CB_12}, {"RL E", 2, 8, ins_CB_13}, {"RL H", 2, 8, ins_CB_14}, {"RL L", 2, 8, ins_CB_15}, {"RL (HL)", 2, 16, ins_CB_16}, {"RL A", 2, 8, ins_CB_17}, {"RR B", 2, 8, ins_CB_18}, {"RR C", 2, 8, ins_CB_19}, {"RR D", 2, 8, ins_CB_1A}, {"RR E", 2, 8, ins_CB_1B}, {"RR H", 2, 8, ins_CB_1C}, {"RR L", 2, 8, ins_CB_1D}, {"RR (HL)", 2, 16, ins_CB_1E}, {"RR A", 2, 8, ins_CB_1F}, },
        {{"SLA B", 2, 8, ins_CB_20}, {"SLA C", 2, 8, ins_CB_21}, {"SLA D", 2, 8, ins_CB_22}, {"SLA E", 2, 8, ins_CB_23}, {"SLA H", 2, 8, ins_CB_24}, {"SLA L", 2, 8, ins_CB_25}, {"SLA (HL)", 2, 16, ins_CB_26}, {"SLA A", 2, 8, ins_CB_27}, {"SRA B", 2, 8, ins_CB_28}, {"SRA C", 2, 8, ins_CB_29}, {"SRA D", 2, 8, ins_CB_2A}, {"SRA E", 2, 8, ins_CB_2B}, {"SRA H", 2, 8, ins_CB_2C}, {"SRA L", 2, 8, ins_CB_2D}, {"SRA (HL)", 2, 16, ins_CB_2E}, {"SRA A", 2, 8, ins_CB_2F}, },
        {{"SWAP B", 2, 8, ins_CB_30}, {"SWAP C", 2, 8, ins_CB_31}, {"SWAP D", 2, 8, ins_CB_32}, {"SWAP E", 2, 8, ins_CB_33}, {"SWAP H", 2, 8, ins_CB_34}, {"SWAP L", 2, 8, ins_CB_35}, {"SWAP (HL)", 2, 16, ins_CB_36}, {"SWAP A", 2, 8, ins_CB_37}, {"SRL B", 2, 8, ins_CB_38}, {"SRL C", 2, 8, ins_CB_39}, {"SRL D", 2, 8, ins_CB_3A}, {"SRL E", 2, 8, ins_CB_3B}, {"SRL H", 2, 8, ins_CB_3C}, {"SRL L", 2, 8, ins_CB_3D}, {"SRL (HL)", 2, 16, ins_CB_3E}, {"SRL A", 2, 8, ins_CB_3F}, },
        {{"BIT 0,B", 2, 8, ins_CB_40}, {"BIT 0,C", 2, 8, ins_CB_41}, {"BIT 0,D", 2, 8, ins_CB_42}, {"BIT 0,E", 2, 8, ins_CB_43}, {"BIT 0,H", 2, 8, ins_CB_44}, {"BIT 0,L", 2, 8, ins_CB_45}, {"BIT 0,(HL)", 2, 12, ins_CB_46}, {"BIT 0,A", 2, 8, ins_CB_47}, {"BIT 1,B", 2, 8, ins_CB_48}, {"BIT 1,C", 2, 8, ins_CB_49}, {"BIT 1,D", 2, 8, ins_CB_4A}, {"BIT 1,E", 2, 8, ins_CB_4B}, {"BIT 1,H", 2, 8, ins_CB_4C}, {"BIT 1,L", 2, 8, ins_CB_4D}, {"BIT 1,(HL)", 2, 12, ins_CB_4E}, {"BIT 1,A", 2, 8, ins_CB_4F}, },
        {{"BIT 2,B", 2, 8, ins_CB_50}, {"BIT 2,C", 2, 8, ins_CB_51}, {"BIT 2,D", 2, 8, ins_CB_52}, {"BIT 2,E", 2, 8, ins_CB_53}, {"BIT 2,H", 2, 8, ins_CB_54}, {"BIT 2,L", 2, 8, ins_CB_55}, {"BIT 2,(HL)", 2, 12, ins_CB_56}, {"BIT 2,A", 2, 8, ins_CB_57}, {"BIT 3,B", 2, 8, ins_CB_58}, {"BIT 3,C", 2, 8, ins_CB_59}, {"BIT 3,D", 2, 8, ins_CB_5A}, {"BIT 3,E", 2, 8, ins_CB_5B}, {"BIT 3,H", 2, 8, ins_CB_5C}, {"BIT 3,L", 2, 8, ins_CB_5D}, {"BIT 3,(HL)", 2, 12, ins_CB_5E}, {"BIT 3,A", 2, 8, ins_CB_5F}, },
        {{"BIT 4,B", 2, 8, ins_CB_60}, {"BIT 4,C", 2, 8, ins_CB_61}, {"BIT 4,D", 2, 8, ins_CB_62}, {"BIT 4,E", 2, 8, ins_CB_63}, {"BIT 4,H", 2, 8, ins_CB_64}, {"BIT 4,L", 2, 8, ins_CB_65}, {"BIT 4,(HL)", 2, 12, ins_CB_66}, {"BIT 4,A", 2, 8, ins_CB_67}, {"BIT 5,B", 2, 8, ins_CB_68}, {"BIT 5,C", 2, 8, ins_CB_69}, {"BIT 5,D", 2, 8, ins_CB_6A}, {"BIT 5,E", 2, 8, ins_CB_6B}, {"BIT 5,H", 2, 8, ins_CB_6C}, {"BIT 5,L", 2, 8, ins_CB_6D}, {"BIT 5,(HL)", 2, 12, ins_CB_6E}, {"BIT 5,A", 2, 8, ins_CB_6F}, },
        {{"BIT 6,B", 2, 8, ins_CB_70}, {"BIT 6,C", 2, 8, ins_CB_71}, {"BIT 6,D", 2, 8, ins_CB_72}, {"BIT 6,E", 2, 8, ins_CB_73}, {"BIT 6,H", 2, 8, ins_CB_74}, {"BIT 6,L", 2, 8, ins_CB_75}, {"BIT 6,(HL)", 2, 12, ins_CB_76}, {"BIT 6,A", 2, 8, ins_CB_77}, {"BIT 7,B", 2, 8, ins_CB_78}, {"BIT 7,C", 2, 8, ins_CB_79}, {"BIT 7,D", 2, 8, ins_CB_7A}, {"BIT 7,E", 2, 8, ins_CB_7B}, {"BIT 7,H", 2, 8, ins_CB_7C}, {"BIT 7,L", 2, 8, ins_CB_7D}, {"BIT 7,(HL)", 2, 12, ins_CB_7E}, {"BIT 7,A", 2, 8, ins_CB_7F}, },
        {{"RES 0,B", 2, 8, ins_CB_80}, {"RES 0,C", 2, 8, ins_CB_81}, {"RES 0,D", 2, 8, ins_CB_82}, {"RES 0,E", 2, 8, ins_CB_83}, {"RES 0,H", 2, 8, ins_CB_84}, {"RES 0,L", 2, 8, ins_CB_85}, {"RES 0,(HL)", 2, 16, ins_CB_86}, {"RES 0,A", 2, 8, ins_CB_87}, {"RES 1,B", 2, 8, ins_CB_88}, {"RES 1,C", 2, 8, ins_CB_89}, {"RES 1,D", 2, 8, ins_CB_8A}, {"RES 1,E", 2, 8, ins_CB_8B}, {"RES 1,H", 2, 8, ins_CB_8C}, {"RES 1,L", 2, 8, ins_CB_8D}, {"RES 1,(HL)", 2, 16, ins_CB_8E}, {"RES 1,A", 2, 8, ins_CB_8F}, },
        {{"RES 2,B", 2, 8, ins_CB_90}, {"RES 2,C", 2, 8, ins_CB_91}, {"RES 2,D", 2, 8, ins_CB_92}, {"RES 2,E", 2, 8, ins_CB_93}, {"RES 2,H", 2, 8, ins_CB_94}, {"RES 2,L", 2, 8, ins_CB_95}, {"RES 2,(HL)", 2, 16, ins_CB_96}, {"RES 2,A", 2, 8, ins_CB_97}, {"RES 3,B", 2, 8, ins_CB_98}, {"RES 3,C", 2, 8, ins_CB_99}, {"RES 3,D", 2, 8, ins_CB_9A}, {"RES 3,E", 2, 8, ins_CB_9B}, {"RES 3,H", 2, 8, ins_CB_9C}, {"RES 3,L", 2, 8, ins_CB_9D}, {"RES 3,(HL)", 2, 16, ins_CB_9E}, {"RES 3,A", 2, 8, ins_CB_9F}, },
        {{"RES 4,B", 2, 8, ins_CB_A0}, {"RES 4,C", 2, 8, ins_CB_A1}, {"RES 4,D", 2, 8, ins_CB_A2}, {"RES 4,E", 2, 8, ins_CB_A3}, {"RES 4,H", 2, 8, ins_CB_A4}, {"RES 4,L", 2, 8, ins_CB_A5}, {"RES 4,(HL)", 2, 16, ins_CB_A6}, {"RES 4,A", 2, 8, ins_CB_A7}, {"RES 5,B", 2, 8, ins_CB_A8}, {"RES 5,C", 2, 8, ins_CB_A9}, {"RES 5,D", 2, 8, ins_CB_AA}, {"RES 5,E", 2, 8, ins_CB_AB}, {"RES 5,H", 2, 8, ins_CB_AC}, {"RES 5,L", 2, 8, ins_CB_AD}, {"RES 5,(HL)", 2, 16, ins_CB_AE}, {"RES 5,A", 2, 8, ins_CB_AF}, },
        {{"RES 6,B", 2, 8, ins_CB_B0}, {"RES 6,C", 2, 8, ins_CB_B1}, {"RES 6,D", 2, 8, ins_CB_B2}, {"RES 6,E", 2, 8, ins_CB_B3}, {"RES 6,H", 2, 8, ins_CB_B4}, {"RES 6,L", 2, 8, ins_CB_B5}, {"RES 6,(HL)", 2, 16, ins_CB_B6}, {"RES 6,A", 2, 8, ins_CB_B7}, {"RES 7,B", 2, 8, ins_CB_B8}, {"RES 7,C", 2, 8, ins_CB_B9}, {"RES 7,D", 2, 8, ins_CB_BA}, {"RES 7,E", 2, 8, ins_CB_BB}, {"RES 7,H", 2, 8, ins_CB_BC}, {"RES 7,L", 2, 8, ins_CB_BD}, {"RES 7,(HL)", 2, 16, ins_CB_BE}, {"RES 7,A", 2, 8, ins_CB_BF}, },
        {{"SET 0,B", 2, 8, ins_CB_C0}, {"SET 0,C", 2, 8, ins_CB_C1}, {"SET 0,D", 2, 8, ins_CB_C2}, {"SET 0,E", 2, 8, ins_CB_C3}, {"SET 0,H", 2, 8, ins_CB_C4}, {"SET 0,L", 2, 8, ins_CB_C5}, {"SET 0,(HL)", 2, 16, ins_CB_C6}, {"SET 0,A", 2, 8, ins_CB_C7}, {"SET 1,B", 2, 8, ins_CB_C8}, {"SET 1,C", 2, 8, ins_CB_C9}, {"SET 1,D", 2, 8, ins_CB_CA}, {"SET 1,E", 2, 8, ins_CB_CB}, {"SET 1,H", 2, 8, ins_CB_CC}, {"SET 1,L", 2, 8, ins_CB_CD}, {"SET 1,(HL)", 2, 16, ins_CB_CE}, {"SET 1,A", 2, 8, ins_CB_CF}, },
        {{"SET 2,B", 2, 8, ins_CB_D0}, {"SET 2,C", 2, 8, ins_CB_D1}, {"SET 2,D", 2, 8, ins_CB_D2}, {"SET 2,E", 2, 8, ins_CB_D3}, {"SET 2,H", 2, 8, ins_CB_D4}, {"SET 2,L", 2, 8, ins_CB_D5}, {"SET 2,(HL)", 2, 16, ins_CB_D6}, {"SET 2,A", 2, 8, ins_CB_D7}, {"SET 3,B", 2, 8, ins_CB_D8}, {"SET 3,C", 2, 8, ins_CB_D9}, {"SET 3,D", 2, 8, ins_CB_DA}, {"SET 3,E", 2, 8, ins_CB_DB}, {"SET 3,H", 2, 8, ins_CB_DC}, {"SET 3,L", 2, 8, ins_CB_DD}, {"SET 3,(HL)", 2, 16, ins_CB_DE}, {"SET 3,A", 2, 8, ins_CB_DF}, },
        {{"SET 4,B", 2, 8, ins_CB_E0}, {"SET 4,C", 2, 8, ins_CB_E1}, {"SET 4,D", 2, 8, ins_CB_E2}, {"SET 4,E", 2, 8, ins_CB_E3}, {"SET 4,H", 2, 8, ins_CB_E4}, {"SET 4,L", 2, 8, ins_CB_E5}, {"SET 4,(HL)", 2, 16, ins_CB_E6}, {"SET 4,A", 2, 8, ins_CB_E7}, {"SET 5,B", 2, 8, ins_CB_E8}, {"SET 5,C", 2, 8, ins_CB_E9}, {"SET 5,D", 2, 8, ins_CB_EA}, {"SET 5,E", 2, 8, ins_CB_EB}, {"SET 5,H", 2, 8, ins_CB_EC}, {"SET 5,L", 2, 8, ins_CB_ED}, {"SET 5,(HL)", 2, 16, ins_CB_EE}, {"SET 5,A", 2, 8, ins_CB_EF}, },
        {{"SET 6,B", 2, 8, ins_CB_F0}, {"SET 6,C", 2, 8, ins_CB_F1}, {"SET 6,D", 2, 8, ins_CB_F2}, {"SET 6,E", 2, 8, ins_CB_F3}, {"SET 6,H", 2, 8, ins_CB_F4}, {"SET 6,L", 2, 8, ins_CB_F5}, {"SET 6,(HL)", 2, 16, ins_CB_F6}, {"SET 6,A", 2, 8, ins_CB_F7}, {"SET 7,B", 2, 8, ins_CB_F8}, {"SET 7,C", 2, 8, ins_CB_F9}, {"SET 7,D", 2, 8, ins_CB_FA}, {"SET 7,E", 2, 8, ins_CB_FB}, {"SET 7,H", 2, 8, ins_CB_FC}, {"SET 7,L", 2, 8, ins_CB_FD}, {"SET 7,(HL)", 2, 16, ins_CB_FE}, {"SET 7,A", 2, 8, ins_CB_FF},}
};

const CPUInstruction *cpu_fetch_instruction(u8 ins){
    return &lookup[left_nibble(ins)][right_nibble(ins)];
}

const CPUInstruction *cpu_fetch_instruction_cb(u8 a0){
    return &lookup_cb[left_nibble(a0)][right_nibble(a0)];
}

#define UNIMPLEMENTED() log_fatal("PC: %04x UNIMPLEMENTED INSTRUCTION %02x%02x",cpu->PC,ins,a0)
#define ILLEGAL_INS() log_fatal("PC: %04x reached illegal instruction %02x%02x",cpu->PC,ins,a0);


//NOP  | Z:- N:- H:- C:-
void ins_00(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){}

//LD BC,d16 | Z:- N:- H:- C:-
void ins_01(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    BC_SET(D16());
}

//LD (BC),A | Z:- N:- H:- C:-
void ins_02(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(BC(),A());
}

//INC BC | Z:- N:- H:- C:-
void ins_03(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    BC_SET(BC()+1);
}

//INC B | Z:Z N:0 H:H C:-
void ins_04(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(B(),1));
    B_SET(B()+1);
    FZ_SET(B()==0);
    FN_SET(0);
}

//DEC B | Z:Z N:1 H:H C:-
void ins_05(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(B(),1));
    B_SET(B()-1);
    FN_SET(1);FZ_SET(B()==0);
}

//LD B,d8 | Z:- N:- H:- C:-
void ins_06(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(D8());
}

//RLCA  | Z:0 N:0 H:0 C:C
void ins_07(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = A();
    u8 v = rotl_u8(o,1);
    A_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//LD (a16),SP | Z:- N:- H:- C:-
void ins_08(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(A16(),SP());
}

//ADD HL,BC | Z:- N:0 H:H C:C
void ins_09(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(HL(),BC()));
    FC_SET(CARRY_16(HL(),BC()));
    FN_SET(0);
    HL_SET(HL()+BC());
}

//LD A,(BC) | Z:- N:- H:- C:-
void ins_0A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(BC()));
}

//DEC BC | Z:- N:- H:- C:-
void ins_0B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    BC_SET(BC()-1);
}

//INC C | Z:Z N:0 H:H C:-
void ins_0C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(C(),1));
    C_SET(C()+1);
    FZ_SET(C()); FN_SET(0);
}

//DEC C | Z:Z N:1 H:H C:-
void ins_0D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(C(),1));
    C_SET(C()-1);
    FN_SET(1);FZ_SET(C()==0);
}

//LD C,d8 | Z:- N:- H:- C:-
void ins_0E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(D8());
}

//RRCA  | Z:0 N:0 H:0 C:C
void ins_0F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FC_SET(A()&1);
    A_SET(rotr_u8(A(),1));
    FZ_SET(0);
    FN_SET(0);
    FH_SET(0);
}

//STOP d8 | Z:- N:- H:- C:-
void ins_10(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD DE,d16 | Z:- N:- H:- C:-
void ins_11(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    DE_SET(D16());
}

//LD (DE),A | Z:- N:- H:- C:-
void ins_12(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(DE(),A());
}

//INC DE | Z:- N:- H:- C:-
void ins_13(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    DE_SET(DE()+1);
}

//INC D | Z:Z N:0 H:H C:-
void ins_14(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(CARRY_4(D(),1));
    D_SET(D()+1);
    FZ_SET(D()==0);
}

//DEC D | Z:Z N:1 H:H C:-
void ins_15(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(D(),1));
    D_SET(D()-1);
    FN_SET(1);FZ_SET(D()==0);
}

//LD D,d8 | Z:- N:- H:- C:-
void ins_16(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D8());
}

//RLA  | Z:0 N:0 H:0 C:C
void ins_17(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC();
    FC_SET(A()&0x80);
    A_SET((A()<<1)|c);
    FZ_SET(0);FN_SET(0);FH_SET(0);
}

//JR r8 | Z:- N:- H:- C:-
void ins_18(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    PC_SET(PC()+R8());
}

//ADD HL,DE | Z:- N:0 H:H C:C
void ins_19(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(HL(),DE()));
    FC_SET(CARRY_16(HL(),DE()));
    FN_SET(0);
    HL_SET(HL()+DE());
}

//LD A,(DE) | Z:- N:- H:- C:-
void ins_1A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 v = M_GET(DE());
    A_SET(v);
}

//DEC DE | Z:- N:- H:- C:-
void ins_1B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    DE_SET(DE()-1);
}

//INC E | Z:Z N:0 H:H C:-
void ins_1C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(CARRY_4(E(),1));
    E_SET(E()+1);
    FZ_SET(E()==0);
}

//DEC E | Z:Z N:1 H:H C:-
void ins_1D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(E(),1));
    E_SET(E()-1);
    FN_SET(1);FZ_SET(E()==0);
}

//LD E,d8 | Z:- N:- H:- C:-
void ins_1E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(D8());
}

//RRA  | Z:0 N:0 H:0 C:C
void ins_1F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = (FC())<<7;
    FC_SET(A()&1);
    A_SET((A()>>1)|c);
    FZ_SET(0);FN_SET(0);FH_SET(0);
}

//JR NZ,r8 | Z:- N:- H:- C:-
void ins_20(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(!FZ()){
        PC_SET(PC()+R8());
    }
}

//LD HL,d16 | Z:- N:- H:- C:-
void ins_21(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    HL_SET(D16());
}

//LD (HL+),A | Z:- N:- H:- C:-
void ins_22(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),A());
    HL_SET(HL()+1);
}

//INC HL | Z:- N:- H:- C:-
void ins_23(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u16 v =HL()+1;
    HL_SET(v);
}

//INC H | Z:Z N:0 H:H C:-
void ins_24(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(H(),1));FN_SET(0);
    H_SET(H()+1);
    FZ_SET(H()==0);

}

//DEC H | Z:Z N:1 H:H C:-
void ins_25(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(H(),1));
    H_SET(H()-1);
    FN_SET(1);FZ_SET(H()==0);
}

//LD H,d8 | Z:- N:- H:- C:-
void ins_26(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(D8());
}

//DAA  | Z:Z N:- H:0 C:C
void ins_27(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 adjust = 0;
    if(FC())
        adjust = 0x60;
    if(FH())
        adjust |= 0x06;
    if(!FN()){
        if((A()&0x0f) >0x09){
            adjust |= 0x06;
        }
        if(A()>0x99){
            adjust |= 0x60;
        }
        A_SET(A()+adjust);
    }else{
        A_SET(A()-adjust);
    }
    FC_SET(adjust>=0x60);
    FH_SET(0);
    FZ_SET(A()==0);
}

//JR Z,r8 | Z:- N:- H:- C:-
void ins_28(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(FZ()){
        PC_SET(PC()+R8());
    }
}

//ADD HL,HL | Z:- N:0 H:H C:C
void ins_29(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(HL(),HL()));
    FC_SET(CARRY_16(HL(),HL()));
    FN_SET(0);
    u16 hl = HL();
    HL_SET(hl+hl);
}

//LD A,(HL+) | Z:- N:- H:- C:-
void ins_2A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(HL()));
    HL_SET(HL()+1);
}

//DEC HL | Z:- N:- H:- C:-
void ins_2B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    HL_SET(HL()-1);
}

//INC L | Z:Z N:0 H:H C:-
void ins_2C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(CARRY_4(L(),1));
    L_SET(L()+1);
    FZ_SET(L()==0);
}

//DEC L | Z:Z N:1 H:H C:-
void ins_2D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(L(),1));
    L_SET(L()-1);
    FN_SET(0);FZ_SET(L()==0);
}

//LD L,d8 | Z:- N:- H:- C:-
void ins_2E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(D8());
}

//CPL  | Z:- N:1 H:1 C:-
void ins_2F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^0xff);
    FN_SET(1);FH_SET(1);
}

//JR NC,r8 | Z:- N:- H:- C:-
void ins_30(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(!FC()){
        PC_SET(PC()+R8());
    }
}

//LD SP,d16 | Z:- N:- H:- C:-
void ins_31(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    SP_SET(D16());
}

//LD (HL-),A | Z:- N:- H:- C:-
void ins_32(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), A());
    HL_SET(HL() - 1);
}

//INC SP | Z:- N:- H:- C:-
void ins_33(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    SP_SET(SP()+1);
}

//INC (HL) | Z:Z N:0 H:H C:-
void ins_34(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = M_GET(HL());
    FH_SET(CARRY_4(c,1));
    M_SET(HL(),c+1);
    FN_SET(0);FZ_SET((c+1)==0);
}

//DEC (HL) | Z:Z N:1 H:H C:-
void ins_35(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 m = M_GET(HL());
    FH_SET(CARRY_4_SUB(m,1));
    M_SET(HL(),m-1);
    FN_SET(0);FZ_SET((m-1)==0);
}

//LD (HL),d8 | Z:- N:- H:- C:-
void ins_36(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),D8());
}

//SCF  | Z:- N:0 H:0 C:1
void ins_37(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);FH_SET(0); FC_SET(1);
}

//JR C,r8 | Z:- N:- H:- C:-
void ins_38(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(FC()){
        PC_SET(PC()+R8());
    }
}

//ADD HL,SP | Z:- N:0 H:H C:C
void ins_39(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(HL(),SP()));
    FC_SET(CARRY_16(HL(),SP()));
    FN_SET(0);
    HL_SET(HL()+SP());
}

//LD A,(HL-) | Z:- N:- H:- C:-
void ins_3A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(HL()));
    HL_SET(HL()-1);
}

//DEC SP | Z:- N:- H:- C:-
void ins_3B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    SP_SET(SP()-1);
}

//INC A | Z:Z N:0 H:H C:-
void ins_3C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(CARRY_4(A(),1));
    A_SET(A()+1);
    FZ_SET(A()==0);
}

//DEC A | Z:Z N:1 H:H C:-
void ins_3D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(A(),1));
    A_SET(A()-1);
    FN_SET(1);FZ_SET(A()==0);
}

//LD A,d8 | Z:- N:- H:- C:-
void ins_3E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(D8());
}

//CCF  | Z:- N:0 H:0 C:C
void ins_3F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);FH_SET(0);
    FC_SET(!FC());
}

//LD B,B | Z:- N:- H:- C:-
void ins_40(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//LD B,C | Z:- N:- H:- C:-
void ins_41(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(C());
}

//LD B,D | Z:- N:- H:- C:-
void ins_42(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(D());
}

//LD B,E | Z:- N:- H:- C:-
void ins_43(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(E());
}

//LD B,H | Z:- N:- H:- C:-
void ins_44(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(H());
}

//LD B,L | Z:- N:- H:- C:-
void ins_45(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(L());
}

//LD B,(HL) | Z:- N:- H:- C:-
void ins_46(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(M_GET(HL()));
}

//LD B,A | Z:- N:- H:- C:-
void ins_47(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(A());
}

//LD C,B | Z:- N:- H:- C:-
void ins_48(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(B());
}

//LD C,C | Z:- N:- H:- C:-
void ins_49(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//LD C,D | Z:- N:- H:- C:-
void ins_4A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(D());
}

//LD C,E | Z:- N:- H:- C:-
void ins_4B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(E());
}

//LD C,H | Z:- N:- H:- C:-
void ins_4C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(H());
}

//LD C,L | Z:- N:- H:- C:-
void ins_4D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(L());
}

//LD C,(HL) | Z:- N:- H:- C:-
void ins_4E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(M_GET(HL()));
}

//LD C,A | Z:- N:- H:- C:-
void ins_4F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(A());
}

//LD D,B | Z:- N:- H:- C:-
void ins_50(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(B());
}

//LD D,C | Z:- N:- H:- C:-
void ins_51(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(C());
}

//LD D,D | Z:- N:- H:- C:-
void ins_52(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//LD D,E | Z:- N:- H:- C:-
void ins_53(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(E());
}

//LD D,H | Z:- N:- H:- C:-
void ins_54(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(H());
}

//LD D,L | Z:- N:- H:- C:-
void ins_55(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(L());
}

//LD D,(HL) | Z:- N:- H:- C:-
void ins_56(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(M_GET(HL()));
}

//LD D,A | Z:- N:- H:- C:-
void ins_57(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(A());
}

//LD E,B | Z:- N:- H:- C:-
void ins_58(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(B());
}

//LD E,C | Z:- N:- H:- C:-
void ins_59(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(C());
}

//LD E,D | Z:- N:- H:- C:-
void ins_5A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(D());
}

//LD E,E | Z:- N:- H:- C:-
void ins_5B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//LD E,H | Z:- N:- H:- C:-
void ins_5C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(H());
}

//LD E,L | Z:- N:- H:- C:-
void ins_5D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(L());
}

//LD E,(HL) | Z:- N:- H:- C:-
void ins_5E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(M_GET(HL()));
}

//LD E,A | Z:- N:- H:- C:-
void ins_5F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(A());
}

//LD H,B | Z:- N:- H:- C:-
void ins_60(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(B());
}

//LD H,C | Z:- N:- H:- C:-
void ins_61(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(C());
}

//LD H,D | Z:- N:- H:- C:-
void ins_62(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(D());
}

//LD H,E | Z:- N:- H:- C:-
void ins_63(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(E());
}

//LD H,H | Z:- N:- H:- C:-
void ins_64(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//LD H,L | Z:- N:- H:- C:-
void ins_65(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(L());
}

//LD H,(HL) | Z:- N:- H:- C:-
void ins_66(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(M_GET(HL()));
}

//LD H,A | Z:- N:- H:- C:-
void ins_67(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(A());
}

//LD L,B | Z:- N:- H:- C:-
void ins_68(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(B());
}

//LD L,C | Z:- N:- H:- C:-
void ins_69(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(C());
}

//LD L,D | Z:- N:- H:- C:-
void ins_6A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(D());
}

//LD L,E | Z:- N:- H:- C:-
void ins_6B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(E());
}

//LD L,H | Z:- N:- H:- C:-
void ins_6C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(H());
}

//LD L,L | Z:- N:- H:- C:-
void ins_6D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//LD L,(HL) | Z:- N:- H:- C:-
void ins_6E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(M_GET(HL()));
}

//LD L,A | Z:- N:- H:- C:-
void ins_6F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(A());
}

//LD (HL),B | Z:- N:- H:- C:-
void ins_70(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),B());
}

//LD (HL),C | Z:- N:- H:- C:-
void ins_71(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),C());
}

//LD (HL),D | Z:- N:- H:- C:-
void ins_72(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),D());
}

//LD (HL),E | Z:- N:- H:- C:-
void ins_73(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),E());
}

//LD (HL),H | Z:- N:- H:- C:-
void ins_74(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),H());
}

//LD (HL),L | Z:- N:- H:- C:-
void ins_75(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),L());
}

//HALT  | Z:- N:- H:- C:-
void ins_76(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD (HL),A | Z:- N:- H:- C:-
void ins_77(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),A());
}

//LD A,B | Z:- N:- H:- C:-
void ins_78(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(B());
}

//LD A,C | Z:- N:- H:- C:-
void ins_79(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(C());
}

//LD A,D | Z:- N:- H:- C:-
void ins_7A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(D());
}

//LD A,E | Z:- N:- H:- C:-
void ins_7B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(E());
}

//LD A,H | Z:- N:- H:- C:-
void ins_7C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(H());
}

//LD A,L | Z:- N:- H:- C:-
void ins_7D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(L());
}

//LD A,(HL) | Z:- N:- H:- C:-
void ins_7E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(HL()));
}

//LD A,A | Z:- N:- H:- C:-
void ins_7F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//ADD A,B | Z:Z N:0 H:H C:C
void ins_80(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(A(),B()));
    FC_SET(CARRY_8(A(),B()));
    FN_SET(0);
    A_SET(A()+B());
    FZ_SET(A()==0);
}

//ADD A,C | Z:Z N:0 H:H C:C
void ins_81(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(A(),C()));
    FC_SET(CARRY_8(A(),C()));
    FN_SET(0);
    A_SET(A()+C());
    FZ_SET(A()==0);
}

//ADD A,D | Z:Z N:0 H:H C:C
void ins_82(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(A(),D()));
    FC_SET(CARRY_8(A(),D()));
    FN_SET(0);
    A_SET(A()+D());
    FZ_SET(A()==0);
}

//ADD A,E | Z:Z N:0 H:H C:C
void ins_83(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(A(),E()));
    FC_SET(CARRY_8(A(),E()));
    FN_SET(0);
    A_SET(A()+E());
    FZ_SET(A()==0);
}

//ADD A,H | Z:Z N:0 H:H C:C
void ins_84(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(A(),H()));
    FC_SET(CARRY_8(A(),H()));
    FN_SET(0);
    A_SET(A()+H());
    FZ_SET(A()==0);
}

//ADD A,L | Z:Z N:0 H:H C:C
void ins_85(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(A(),L()));
    FC_SET(CARRY_8(A(),L()));
    FN_SET(0);
    A_SET(A()+L());
    FZ_SET(A()==0);
}

//ADD A,(HL) | Z:Z N:0 H:H C:C
void ins_86(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 m = M_GET(HL());
    FH_SET(CARRY_4(A(),m));
    FC_SET(CARRY_8(A(),m));
    FN_SET(0);
    A_SET(A()+m);
}

//ADD A,A | Z:Z N:0 H:H C:C
void ins_87(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(A(),A()));
    FC_SET(CARRY_8(A(),A()));
    FN_SET(0);
    A_SET(A()+A());
    FZ_SET(A()==0);
}

//ADC A,B | Z:Z N:0 H:H C:C
void ins_88(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    u8 c = FC();
    FH_SET(CARRY_4C(A(),B(),c));
    FC_SET(CARRY_8C(A(),B(),c));
    A_SET(A()+B()+c);
    FZ_SET(A()==0);
}

//ADC A,C | Z:Z N:0 H:H C:C
void ins_89(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    u8 c = FC();
    FH_SET(CARRY_4C(A(),C(),c));
    FC_SET(CARRY_8C(A(),C(),c));
    A_SET(A()+C()+c);
    FZ_SET(A()==0);
}

//ADC A,D | Z:Z N:0 H:H C:C
void ins_8A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    u8 c = FC();
    FH_SET(CARRY_4C(A(),D(),c));
    FC_SET(CARRY_8C(A(),D(),c));
    A_SET(A()+D()+c);
    FZ_SET(A()==0);
}

//ADC A,E | Z:Z N:0 H:H C:C
void ins_8B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    u8 c = FC();
    FH_SET(CARRY_4C(A(),E(),c));
    FC_SET(CARRY_8C(A(),E(),c));
    A_SET(A()+E()+c);
    FZ_SET(A()==0);
}

//ADC A,H | Z:Z N:0 H:H C:C
void ins_8C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    u8 c = FC();
    FH_SET(CARRY_4C(A(),H(),c));
    FC_SET(CARRY_8C(A(),H(),c));
    A_SET(A()+H()+c);
    FZ_SET(A()==0);
}

//ADC A,L | Z:Z N:0 H:H C:C
void ins_8D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    u8 c = FC();
    FH_SET(CARRY_4C(A(),L(),c));
    FC_SET(CARRY_8C(A(),L(),c));
    A_SET(A()+L()+c);
    FZ_SET(A()==0);
}

//ADC A,(HL) | Z:Z N:0 H:H C:C
void ins_8E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    u8 m = M_GET(HL());
    u8 c = FC();
    FH_SET(CARRY_4C(A(),m,c));
    FC_SET(CARRY_8C(A(),m,c));
    A_SET(A()+m+c);
    FZ_SET(A()==0);
}

//ADC A,A | Z:Z N:0 H:H C:C
void ins_8F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    u8 c = FC();
    FH_SET(CARRY_4C(A(),A(),c));
    FC_SET(CARRY_8C(A(),A(),c));
    A_SET(A()+A()+c);
    FZ_SET(A()==0);
}

//SUB B | Z:Z N:1 H:H C:C
void ins_90(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FC_SET(CARRY_8_SUB(A(),B()));
    FH_SET(CARRY_4_SUB(A(),B()));
    FN_SET(1);
    A_SET(A()-B());
    FZ_SET(A()==0);
}

//SUB C | Z:Z N:1 H:H C:C
void ins_91(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FC_SET(CARRY_8_SUB(A(),C()));
    FH_SET(CARRY_4_SUB(A(),C()));
    FN_SET(1);
    A_SET(A()-C());
    FZ_SET(A()==0);
}

//SUB D | Z:Z N:1 H:H C:C
void ins_92(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FC_SET(CARRY_8_SUB(A(),D()));
    FH_SET(CARRY_4_SUB(A(),D()));
    FN_SET(1);
    A_SET(A()-D());
    FZ_SET(A()==0);
}

//SUB E | Z:Z N:1 H:H C:C
void ins_93(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FC_SET(CARRY_8_SUB(A(),E()));
    FH_SET(CARRY_4_SUB(A(),E()));
    FN_SET(1);
    A_SET(A()-E());
    FZ_SET(A()==0);
}

//SUB H | Z:Z N:1 H:H C:C
void ins_94(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FC_SET(CARRY_8_SUB(A(),H()));
    FH_SET(CARRY_4_SUB(A(),H()));
    FN_SET(1);
    A_SET(A()-H());
    FZ_SET(A()==0);
}

//SUB L | Z:Z N:1 H:H C:C
void ins_95(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FC_SET(CARRY_8_SUB(A(),L()));
    FH_SET(CARRY_4_SUB(A(),L()));
    FN_SET(1);
    A_SET(A()-L());
    FZ_SET(A()==0);
}

//SUB (HL) | Z:Z N:1 H:H C:C
void ins_96(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 m = M_GET(HL());
    FC_SET(CARRY_8_SUB(A(),m));
    FH_SET(CARRY_4_SUB(A(),m));
    FN_SET(1);
    A_SET(A()-m);
    FZ_SET(A()==0);
}

//SUB A | Z:1 N:1 H:0 C:0
void ins_97(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FC_SET(0);
    FH_SET(0);
    FN_SET(1);
    A_SET(0);
    FZ_SET(1);
}

//SBC A,B | Z:Z N:1 H:H C:C
void ins_98(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(1);
    u8 c = FC();
    FH_SET(CARRY_4_SUBC(A(),B(),c));
    FC_SET(CARRY_8_SUBC(A(),B(),c));
    A_SET(A()-B()-c);
    FZ_SET(A()==0);
}

//SBC A,C | Z:Z N:1 H:H C:C
void ins_99(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(1);
    u8 c = FC();
    FH_SET(CARRY_4_SUBC(A(),C(),c));
    FC_SET(CARRY_8_SUBC(A(),C(),c));
    A_SET(A()-C()-c);
    FZ_SET(A()==0);
}

//SBC A,D | Z:Z N:1 H:H C:C
void ins_9A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(1);
    u8 c = FC();
    FH_SET(CARRY_4_SUBC(A(),D(),c));
    FC_SET(CARRY_8_SUBC(A(),D(),c));
    A_SET(A()-D()-c);
    FZ_SET(A()==0);
}

//SBC A,E | Z:Z N:1 H:H C:C
void ins_9B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(1);
    u8 c = FC();
    FH_SET(CARRY_4_SUBC(A(),E(),c));
    FC_SET(CARRY_8_SUBC(A(),E(),c));
    A_SET(A()-E()-c);
    FZ_SET(A()==0);
}

//SBC A,H | Z:Z N:1 H:H C:C
void ins_9C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(1);
    u8 c = FC();
    FH_SET(CARRY_4_SUBC(A(),H(),c));
    FC_SET(CARRY_8_SUBC(A(),H(),c));
    A_SET(A()-H()-c);
    FZ_SET(A()==0);
}

//SBC A,L | Z:Z N:1 H:H C:C
void ins_9D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(1);
    u8 c = FC();
    FH_SET(CARRY_4_SUBC(A(),L(),c));
    FC_SET(CARRY_8_SUBC(A(),L(),c));
    A_SET(A()-L()-c);
    FZ_SET(A()==0);
}

//SBC A,(HL) | Z:Z N:1 H:H C:C
void ins_9E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(1);
    u8 c = FC();
    u8 m = M_GET(HL());
    FH_SET(CARRY_4_SUBC(A(),m,c));
    FC_SET(CARRY_8_SUBC(A(),m,c));
    A_SET(A()-m-c);
    FZ_SET(A()==0);
}

//SBC A,A | Z:Z N:1 H:H C:C
void ins_9F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(1);
    u8 c = FC();
    FH_SET(CARRY_4_SUBC(A(),A(),c));
    FC_SET(CARRY_8_SUBC(A(),A(),c));
    A_SET(A()-A()-c);
    FZ_SET(A()==0);
}

//AND B | Z:Z N:0 H:1 C:0
void ins_A0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&B());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND C | Z:Z N:0 H:1 C:0
void ins_A1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&C());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND D | Z:Z N:0 H:1 C:0
void ins_A2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&D());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND E | Z:Z N:0 H:1 C:0
void ins_A3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&E());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND H | Z:Z N:0 H:1 C:0
void ins_A4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&H());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND L | Z:Z N:0 H:1 C:0
void ins_A5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&L());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND (HL) | Z:Z N:0 H:1 C:0
void ins_A6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 v = M_GET(HL());
    A_SET(A()&v);
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND A | Z:Z N:0 H:1 C:0
void ins_A7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&A());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//XOR B | Z:Z N:0 H:0 C:0
void ins_A8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^B());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR C | Z:Z N:0 H:0 C:0
void ins_A9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^C());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR D | Z:Z N:0 H:0 C:0
void ins_AA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^D());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR E | Z:Z N:0 H:0 C:0
void ins_AB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^E());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR H | Z:Z N:0 H:0 C:0
void ins_AC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^H());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR L | Z:Z N:0 H:0 C:0
void ins_AD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^L());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR (HL) | Z:Z N:0 H:0 C:0
void ins_AE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 m = M_GET(HL());
    A_SET(A()^m);
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR A | Z:1 N:0 H:0 C:0
void ins_AF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A() ^ A());
    FZ_SET(1); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR B | Z:Z N:0 H:0 C:0
void ins_B0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|B());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR C | Z:Z N:0 H:0 C:0
void ins_B1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|C());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR D | Z:Z N:0 H:0 C:0
void ins_B2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|D());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR E | Z:Z N:0 H:0 C:0
void ins_B3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|E());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR H | Z:Z N:0 H:0 C:0
void ins_B4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|H());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR L | Z:Z N:0 H:0 C:0
void ins_B5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|L());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR (HL) | Z:Z N:0 H:0 C:0
void ins_B6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|M_GET(HL()));
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR A | Z:Z N:0 H:0 C:0
void ins_B7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|A());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//CP B | Z:Z N:1 H:H C:C
void ins_B8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-B();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),B()));FC_SET(CARRY_8_SUB(A(),B()));
}

//CP C | Z:Z N:1 H:H C:C
void ins_B9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-C();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),C()));FC_SET(CARRY_8_SUB(A(),C()));
}

//CP D | Z:Z N:1 H:H C:C
void ins_BA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-D();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),D()));FC_SET(CARRY_8_SUB(A(),D()));
}

//CP E | Z:Z N:1 H:H C:C
void ins_BB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-E();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),E()));FC_SET(CARRY_8_SUB(A(),E()));
}

//CP H | Z:Z N:1 H:H C:C
void ins_BC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-H();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),H()));FC_SET(CARRY_8_SUB(A(),H()));
}

//CP L | Z:Z N:1 H:H C:C
void ins_BD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-L();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),L()));FC_SET(CARRY_8_SUB(A(),L()));
}

//CP (HL) | Z:Z N:1 H:H C:C
void ins_BE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 m = M_GET(HL());
    u8 sub = A()-m;
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),m));FC_SET(CARRY_8_SUB(A(),m));
}

//CP A | Z:1 N:1 H:0 C:0
void ins_BF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(1);FN_SET(1);FH_SET(0);FC_SET(0);
}

//RET NZ | Z:- N:- H:- C:-
void ins_C0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(!FZ()){
        PC_SET(stack_pop_u16(cpu));
    }
}

//POP BC | Z:- N:- H:- C:-
void ins_C1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u16 v = stack_pop_u16(cpu);
    BC_SET(v);
}

//JP NZ,a16 | Z:- N:- H:- C:-
void ins_C2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(!FZ()){
        PC_SET(A16());
    }
}

//JP a16 | Z:- N:- H:- C:-
void ins_C3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    PC_SET(A16());
}

//CALL NZ,a16 | Z:- N:- H:- C:-
void ins_C4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(!FZ()){
        stack_push_u16(cpu,PC());
        PC_SET(A16());
    }
}

//PUSH BC | Z:- N:- H:- C:-
void ins_C5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,BC());
}

//ADD A,d8 | Z:Z N:0 H:H C:C
void ins_C6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(A(),D8()));
    FC_SET(CARRY_8(A(),D8()));
    FN_SET(0);
    A_SET(A()+D8());
    FZ_SET(A()==0);
}

//RST 00H | Z:- N:- H:- C:-
void ins_C7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,PC());
    PC_SET(0x00);
}

//RET Z | Z:- N:- H:- C:-
void ins_C8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(FZ()){
        PC_SET(stack_pop_u16(cpu));
    }
}

//RET  | Z:- N:- H:- C:-
void ins_C9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    PC_SET(stack_pop_u16(cpu));
}

//JP Z,a16 | Z:- N:- H:- C:-
void ins_CA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(FZ()){
        PC_SET(A16());
    }
}

//PREFIX  | Z:- N:- H:- C:-
void ins_CB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    //how did we even get here?
    ILLEGAL_INS();
}

//CALL Z,a16 | Z:- N:- H:- C:-
void ins_CC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(FZ()){
        stack_push_u16(cpu,PC());
        PC_SET(A16());
    }
}

//CALL a16 | Z:- N:- H:- C:-
void ins_CD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,PC());
    PC_SET(A16());
}

//ADC A,d8 | Z:Z N:0 H:H C:C
void ins_CE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    u8 c = FC();
    FH_SET(CARRY_4C(A(),D8(),c));
    FC_SET(CARRY_8C(A(),D8(),c));
    A_SET(A()+D8()+c);
    FZ_SET(A()==0);
}

//RST 08H | Z:- N:- H:- C:-
void ins_CF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,PC());
    PC_SET(0x08);
}

//RET NC | Z:- N:- H:- C:-
void ins_D0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(!FC()){
        PC_SET(stack_pop_u16(cpu));
    }
}

//POP DE | Z:- N:- H:- C:-
void ins_D1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u16 v =stack_pop_u16(cpu);
    DE_SET(v);
}

//JP NC,a16 | Z:- N:- H:- C:-
void ins_D2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(!FC()){
        PC_SET(A16());
    }
}

//ILLEGAL_D3  | Z:- N:- H:- C:-
void ins_D3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    ILLEGAL_INS();
}

//CALL NC,a16 | Z:- N:- H:- C:-
void ins_D4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(!FC()){
        stack_push_u16(cpu,PC());
        PC_SET(A16());
    }
}

//PUSH DE | Z:- N:- H:- C:-
void ins_D5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,DE());
}

//SUB d8 | Z:Z N:1 H:H C:C
void ins_D6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FC_SET(CARRY_8_SUB(A(),D8()));
    FH_SET(CARRY_4_SUB(A(),D8()));
    FN_SET(1);
    A_SET(A()-D8());
    FZ_SET(A()==0);
}

//RST 10H | Z:- N:- H:- C:-
void ins_D7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,PC());
    PC_SET(0x10);
}

//RET C | Z:- N:- H:- C:-
void ins_D8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(FC()){
        PC_SET(stack_pop_u16(cpu));
    }
}

//RETI  | Z:- N:- H:- C:-
void ins_D9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    PC_SET(stack_pop_u16(cpu));
    cpu->IME = 1;
}

//JP C,a16 | Z:- N:- H:- C:-
void ins_DA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(FC()){
        PC_SET(A16());
    }
}

//ILLEGAL_DB  | Z:- N:- H:- C:-
void ins_DB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    ILLEGAL_INS();
}

//CALL C,a16 | Z:- N:- H:- C:-
void ins_DC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(FC()){
        stack_push_u16(cpu,PC());
        PC_SET(A16());
    }
}

//ILLEGAL_DD  | Z:- N:- H:- C:-
void ins_DD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    ILLEGAL_INS();
}

//SBC A,d8 | Z:Z N:1 H:H C:C
void ins_DE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(1);
    u8 c = FC();
    FH_SET(CARRY_4_SUBC(A(),D8(),c));
    FC_SET(CARRY_8_SUBC(A(),D8(),c));
    A_SET(A()-D8()-c);
    FZ_SET(A()==0);
}

//RST 18H | Z:- N:- H:- C:-
void ins_DF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,PC());
    PC_SET(0x18);
}

//LDH (a8),A | Z:- N:- H:- C:-
void ins_E0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(0xff00+D8(), A());
}

//POP HL | Z:- N:- H:- C:-
void ins_E1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u16 v =stack_pop_u16(cpu);
    HL_SET(v);
}

//LD ($FF00+C),A | Z:- N:- H:- C:-
void ins_E2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(0xFF00+C(),A());
}

//ILLEGAL_E3  | Z:- N:- H:- C:-
void ins_E3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    ILLEGAL_INS();
}

//ILLEGAL_E4  | Z:- N:- H:- C:-
void ins_E4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    ILLEGAL_INS();
}

//PUSH HL | Z:- N:- H:- C:-
void ins_E5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,HL());
}

//AND d8 | Z:Z N:0 H:1 C:0
void ins_E6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(D8() == 3){
        log_info("in mothafucking: %04x",cpu->PC);
    }
    A_SET(A()&D8());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//RST 20H | Z:- N:- H:- C:-
void ins_E7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,PC());
    PC_SET(0x20);
}

//ADD SP,r8 | Z:0 N:0 H:H C:C
void ins_E8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(SP(),R8()));
    FC_SET(CARRY_16(SP(),R8()));
    FN_SET(0);
    SP_SET(SP()+R8());
    FZ_SET(0);
}

//JP HL | Z:- N:- H:- C:-
void ins_E9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    PC_SET(HL());
}

//LD (a16),A | Z:- N:- H:- C:-
void ins_EA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(A16(),A());
}

//ILLEGAL_EB  | Z:- N:- H:- C:-
void ins_EB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    ILLEGAL_INS();
}

//ILLEGAL_EC  | Z:- N:- H:- C:-
void ins_EC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    ILLEGAL_INS();
}

//ILLEGAL_ED  | Z:- N:- H:- C:-
void ins_ED(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    ILLEGAL_INS();
}

//XOR d8 | Z:Z N:0 H:0 C:0
void ins_EE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^D8());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//RST 28H | Z:- N:- H:- C:-
void ins_EF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,PC());
    PC_SET(0x28);
}

//LDH A,($ff00+a8) | Z:- N:- H:- C:-
void ins_F0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(0xff00+D8()));
}

//POP AF | Z:Z N:N H:H C:C
void ins_F1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u16 v =stack_pop_u16(cpu);
    AF_SET(v);
}

//LD A,($ff00+C) | Z:- N:- H:- C:-
void ins_F2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(0xff00+C()));
}

//DI  | Z:- N:- H:- C:-
void ins_F3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    cpu->IME = 0;
}

//ILLEGAL_F4  | Z:- N:- H:- C:-
void ins_F4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    ILLEGAL_INS();
}

//PUSH AF | Z:- N:- H:- C:-
void ins_F5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,AF());
}

//OR d8 | Z:Z N:0 H:0 C:0
void ins_F6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|D8());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//RST 30H | Z:- N:- H:- C:-
void ins_F7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,PC());
    PC_SET(0x30);
}

//LD HL,SP+r8 | Z:0 N:0 H:H C:C
void ins_F8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FC_SET(CARRY_16(SP(),R8()));
    FH_SET(CARRY_4(SP(),R8()));
    FZ_SET(0);
    FN_SET(0);
    HL_SET(SP()+R8());
}

//LD SP,HL | Z:- N:- H:- C:-
void ins_F9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    SP_SET(HL());
}

//LD A,(a16) | Z:- N:- H:- C:-
void ins_FA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 v = M_GET(A16());
    A_SET(v);
}

//EI  | Z:- N:- H:- C:-
void ins_FB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    cpu->IME=1;
}

//ILLEGAL_FC  | Z:- N:- H:- C:-
void ins_FC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    ILLEGAL_INS();
}

//ILLEGAL_FD  | Z:- N:- H:- C:-
void ins_FD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    ILLEGAL_INS();
}

//CP d8 | Z:Z N:1 H:H C:C
void ins_FE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-D8();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),D8()));FC_SET(CARRY_8_SUB(A(),D8()));
}

//RST 38H | Z:- N:- H:- C:-
void ins_FF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,PC());
    PC_SET(0x38);
}


//RLC B | Z:Z N:0 H:0 C:C
void ins_CB_00(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = B();
    u8 v = rotl_u8(o,1);
    B_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC C | Z:Z N:0 H:0 C:C
void ins_CB_01(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = C();
    u8 v = rotl_u8(o,1);
    C_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC D | Z:Z N:0 H:0 C:C
void ins_CB_02(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = E();
    u8 v = rotl_u8(o,1);
    E_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC E | Z:Z N:0 H:0 C:C
void ins_CB_03(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = E();
    u8 v = rotl_u8(o,1);
    E_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC H | Z:Z N:0 H:0 C:C
void ins_CB_04(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = H();
    u8 v = rotl_u8(o,1);
    H_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC L | Z:Z N:0 H:0 C:C
void ins_CB_05(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = L();
    u8 v = rotl_u8(o,1);
    L_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC (HL) | Z:Z N:0 H:0 C:C
void ins_CB_06(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = M_GET(HL());
    u8 v = rotl_u8(o,1);
    M_SET(HL(),v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC A | Z:Z N:0 H:0 C:C
void ins_CB_07(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = A();
    u8 v = rotl_u8(o,1);
    A_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RRC B | Z:Z N:0 H:0 C:C
void ins_CB_08(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(B()&1);
    B_SET(rotr_u8(B(),1));
    FZ_SET(B()==0);
}

//RRC C | Z:Z N:0 H:0 C:C
void ins_CB_09(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(C()&1);
    C_SET(rotr_u8(C(),1));
    FZ_SET(C()==0);
}

//RRC D | Z:Z N:0 H:0 C:C
void ins_CB_0A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(D()&1);
    D_SET(rotr_u8(D(),1));
    FZ_SET(D()==0);
}

//RRC E | Z:Z N:0 H:0 C:C
void ins_CB_0B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(E()&1);
    E_SET(rotr_u8(E(),1));
    FZ_SET(E()==0);
}

//RRC H | Z:Z N:0 H:0 C:C
void ins_CB_0C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(H()&1);
    H_SET(rotr_u8(H(),1));
    FZ_SET(H()==0);
}

//RRC L | Z:Z N:0 H:0 C:C
void ins_CB_0D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(L()&1);
    L_SET(rotr_u8(L(),1));
    FZ_SET(L()==0);
}

//RRC (HL) | Z:Z N:0 H:0 C:C
void ins_CB_0E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = M_GET(HL());
    u8 v = rotr_u8(o,1);
    M_SET(HL(),v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&1);
}

//RRC A | Z:Z N:0 H:0 C:C
void ins_CB_0F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(A()&1);
    A_SET(rotr_u8(A(),1));
    FZ_SET(A()==0);
}

//RL B | Z:Z N:0 H:0 C:C
void ins_CB_10(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC();
    FC_SET(B()&0x80);
    B_SET((B()<<1)|c);
    FZ_SET(B()==0);FN_SET(0);FH_SET(0);
}

//RL C | Z:Z N:0 H:0 C:C
void ins_CB_11(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC();
    FC_SET(C()&0x80);
    C_SET((C()<<1)|c);
    FZ_SET(C()==0);FN_SET(0);FH_SET(0);
}

//RL D | Z:Z N:0 H:0 C:C
void ins_CB_12(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC();
    FC_SET(D()&0x80);
    D_SET((D()<<1)|c);
    FZ_SET(D()==0);FN_SET(0);FH_SET(0);
}

//RL E | Z:Z N:0 H:0 C:C
void ins_CB_13(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC();
    FC_SET(E()&0x80);
    E_SET((E()<<1)|c);
    FZ_SET(E()==0);FN_SET(0);FH_SET(0);
}

//RL H | Z:Z N:0 H:0 C:C
void ins_CB_14(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC();
    FC_SET(H()&0x80);
    H_SET((H()<<1)|c);
    FZ_SET(H()==0);FN_SET(0);FH_SET(0);
}

//RL L | Z:Z N:0 H:0 C:C
void ins_CB_15(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC();
    FC_SET(L()&0x80);
    L_SET((L()<<1)|c);
    FZ_SET(L()==0);FN_SET(0);FH_SET(0);
}

//RL (HL) | Z:Z N:0 H:0 C:C
void ins_CB_16(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC();
    u8 m = M_GET(HL());
    u8 v = (m<<1)|c;
    FC_SET(m&0x80);
    M_SET(HL(),v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);
}

//RL A | Z:Z N:0 H:0 C:C
void ins_CB_17(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC();
    FC_SET(A()&0x80);
    A_SET((A()<<1)|c);
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);
}

//RR B | Z:Z N:0 H:0 C:C
void ins_CB_18(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC()<<7;
    FC_SET(B()&1);
    B_SET((B()>>1)|c);
    FZ_SET(B()==0);FN_SET(0);FH_SET(0);
}

//RR C | Z:Z N:0 H:0 C:C
void ins_CB_19(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC()<<7;
    FC_SET(C()&1);
    C_SET((C()>>1)|c);
    FZ_SET(C()==0);FN_SET(0);FH_SET(0);
}

//RR D | Z:Z N:0 H:0 C:C
void ins_CB_1A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC()<<7;
    FC_SET(D()&1);
    D_SET((D()>>1)|c);
    FZ_SET(D()==0);FN_SET(0);FH_SET(0);
}

//RR E | Z:Z N:0 H:0 C:C
void ins_CB_1B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC()<<7;
    FC_SET(E()&1);
    E_SET((E()>>1)|c);
    FZ_SET(E()==0);FN_SET(0);FH_SET(0);
}

//RR H | Z:Z N:0 H:0 C:C
void ins_CB_1C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC()<<7;
    FC_SET(H()&1);
    H_SET((H()>>1)|c);
    FZ_SET(H()==0);FN_SET(0);FH_SET(0);
}

//RR L | Z:Z N:0 H:0 C:C
void ins_CB_1D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC()<<7;
    FC_SET(L()&1);
    L_SET((L()>>1)|c);
    FZ_SET(L()==0);FN_SET(0);FH_SET(0);
}

//RR (HL) | Z:Z N:0 H:0 C:C
void ins_CB_1E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC()<<7;
    u8 m = M_GET(HL());
    FC_SET(m&1);
    m = (m>>1)|c;
    M_SET(HL(),m);
    FZ_SET(m==0);FN_SET(0);FH_SET(0);
}

//RR A | Z:Z N:0 H:0 C:C
void ins_CB_1F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = FC()<<7;
    FC_SET(A()&1);
    A_SET((A()>>1)|c);
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);
}

//SLA B | Z:Z N:0 H:0 C:C
void ins_CB_20(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(B()&0x80);
    B_SET (B()<<1);
    FZ_SET(B()==0);
}

//SLA C | Z:Z N:0 H:0 C:C
void ins_CB_21(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(C()&0x80);
    C_SET (C()<<1);
    FZ_SET(C()==0);
}

//SLA D | Z:Z N:0 H:0 C:C
void ins_CB_22(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(D()&0x80);
    D_SET (D()<<1);
    FZ_SET(D()==0);
}

//SLA E | Z:Z N:0 H:0 C:C
void ins_CB_23(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(E()&0x80);
    E_SET (E()<<1);
    FZ_SET(E()==0);
}

//SLA H | Z:Z N:0 H:0 C:C
void ins_CB_24(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(H()&0x80);
    H_SET (H()<<1);
    FZ_SET(H()==0);
}

//SLA L | Z:Z N:0 H:0 C:C
void ins_CB_25(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(L()&0x80);
    L_SET (L()<<1);
    FZ_SET(L()==0);
}

//SLA (HL) | Z:Z N:0 H:0 C:C
void ins_CB_26(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 m = M_GET(HL());
    FN_SET(0);
    FH_SET(0);
    FC_SET(m&0x80);
    M_SET (HL(),m<<1);
    FZ_SET((m<<1)==0);
}

//SLA A | Z:Z N:0 H:0 C:C
void ins_CB_27(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(A()&0x80);
    A_SET (A()<<1);
    FZ_SET(A()==0);
}

//SRA B | Z:Z N:0 H:0 C:C
void ins_CB_28(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0); FH_SET(0);
    u8 l = B()&0x80;
    FC_SET(B()&1);
    B_SET((B()>>1)|l);
    FZ_SET(B()==0);
}

//SRA C | Z:Z N:0 H:0 C:C
void ins_CB_29(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0); FH_SET(0);
    u8 l = C()&0x80;
    FC_SET(C()&1);
    C_SET((C()>>1)|l);
    FZ_SET(C()==0);
}

//SRA D | Z:Z N:0 H:0 C:C
void ins_CB_2A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0); FH_SET(0);
    u8 l = D()&0x80;
    FC_SET(D()&1);
    D_SET((D()>>1)|l);
    FZ_SET(D()==0);
}

//SRA E | Z:Z N:0 H:0 C:C
void ins_CB_2B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0); FH_SET(0);
    u8 l = E()&0x80;
    FC_SET(E()&1);
    E_SET((E()>>1)|l);
    FZ_SET(E()==0);
}

//SRA H | Z:Z N:0 H:0 C:C
void ins_CB_2C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0); FH_SET(0);
    u8 l = H()&0x80;
    FC_SET(H()&1);
    H_SET((H()>>1)|l);
    FZ_SET(H()==0);
}

//SRA L | Z:Z N:0 H:0 C:C
void ins_CB_2D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0); FH_SET(0);
    u8 l = L()&0x80;
    FC_SET(L()&1);
    L_SET((L()>>1)|l);
    FZ_SET(L()==0);
}

//SRA (HL) | Z:Z N:0 H:0 C:C
void ins_CB_2E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0); FH_SET(0);
    u8 m = M_GET(HL());
    u8 l = m&0x80;
    FC_SET(m&1);
    M_SET(HL(),(m>>1)|l);
    FZ_SET(((m>>1)|l)==0);
}

//SRA A | Z:Z N:0 H:0 C:C
void ins_CB_2F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0); FH_SET(0);
    u8 l = A()&0x80;
    FC_SET(A()&1);
    A_SET((A()>>1)|l);
    FZ_SET(A()==0);
}

//SWAP B | Z:Z N:0 H:0 C:0
void ins_CB_30(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(0);
    B_SET(SWAP8(B()));
    FZ_SET(B()==0);
}

//SWAP C | Z:Z N:0 H:0 C:0
void ins_CB_31(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(0);
    C_SET(SWAP8(C()));
    FZ_SET(C()==0);
}

//SWAP D | Z:Z N:0 H:0 C:0
void ins_CB_32(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(0);
    D_SET(SWAP8(D()));
    FZ_SET(D()==0);
}

//SWAP E | Z:Z N:0 H:0 C:0
void ins_CB_33(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(0);
    E_SET(SWAP8(E()));
    FZ_SET(E()==0);
}

//SWAP H | Z:Z N:0 H:0 C:0
void ins_CB_34(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(0);
    H_SET(SWAP8(H()));
    FZ_SET(H()==0);
}

//SWAP L | Z:Z N:0 H:0 C:0
void ins_CB_35(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(0);
    L_SET(SWAP8(L()));
    FZ_SET(L()==0);
}

//SWAP (HL) | Z:Z N:0 H:0 C:0
void ins_CB_36(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 m = M_GET(HL());
    FN_SET(0);
    FH_SET(0);
    FC_SET(0);
    M_SET(HL(),SWAP8(m));
    FZ_SET(m==0); //swap is only 0 if it has been 0 before so yea
}

//SWAP A | Z:Z N:0 H:0 C:0
void ins_CB_37(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(0);
    A_SET(SWAP8(A()));
    FZ_SET(A()==0);
}

//SRL B | Z:Z N:0 H:0 C:C
void ins_CB_38(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(B()&1);
    B_SET(B()>>1);
    FZ_SET(B()==0);
}

//SRL C | Z:Z N:0 H:0 C:C
void ins_CB_39(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(C()&1);
    C_SET(C()>>1);
    FZ_SET(C()==0);
}

//SRL D | Z:Z N:0 H:0 C:C
void ins_CB_3A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(D()&1);
    D_SET(D()>>1);
    FZ_SET(D()==0);
}

//SRL E | Z:Z N:0 H:0 C:C
void ins_CB_3B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(E()&1);
    E_SET(E()>>1);
    FZ_SET(E()==0);
}

//SRL H | Z:Z N:0 H:0 C:C
void ins_CB_3C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(H()&1);
    H_SET(H()>>1);
    FZ_SET(H()==0);
}

//SRL L | Z:Z N:0 H:0 C:C
void ins_CB_3D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(L()&1);
    L_SET(L()>>1);
    FZ_SET(L()==0);
}

//SRL (HL) | Z:Z N:0 H:0 C:C
void ins_CB_3E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 m = M_GET(HL());
    FN_SET(0);
    FH_SET(0);
    FC_SET(m&1);
    M_SET(HL(),m>>1);
    FZ_SET((m>>1)==0);
}

//SRL A | Z:Z N:0 H:0 C:C
void ins_CB_3F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FN_SET(0);
    FH_SET(0);
    FC_SET(A()&1);
    A_SET(A()>>1);
    FZ_SET(A()==0);
}

//BIT 0,B | Z:Z N:0 H:1 C:-
void ins_CB_40(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(B()&(1<<0))); FN_SET(0); FH_SET(1);
}

//BIT 0,C | Z:Z N:0 H:1 C:-
void ins_CB_41(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(C()&(1<<0))); FN_SET(0); FH_SET(1);
}

//BIT 0,D | Z:Z N:0 H:1 C:-
void ins_CB_42(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(D()&(1<<0))); FN_SET(0); FH_SET(1);
}

//BIT 0,E | Z:Z N:0 H:1 C:-
void ins_CB_43(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(E()&(1<<0))); FN_SET(0); FH_SET(1);
}

//BIT 0,H | Z:Z N:0 H:1 C:-
void ins_CB_44(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(H()&(1<<0))); FN_SET(0); FH_SET(1);
}

//BIT 0,L | Z:Z N:0 H:1 C:-
void ins_CB_45(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(L()&(1<<0))); FN_SET(0); FH_SET(1);
}

//BIT 0,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_46(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<0)); FN_SET(0); FH_SET(1);
}

//BIT 0,A | Z:Z N:0 H:1 C:-
void ins_CB_47(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(A()&(1<<0))); FN_SET(0); FH_SET(1);
}

//BIT 1,B | Z:Z N:0 H:1 C:-
void ins_CB_48(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(B()&(1<<1))); FN_SET(0); FH_SET(1);
}

//BIT 1,C | Z:Z N:0 H:1 C:-
void ins_CB_49(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(C()&(1<<1))); FN_SET(0); FH_SET(1);
}

//BIT 1,D | Z:Z N:0 H:1 C:-
void ins_CB_4A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(D()&(1<<1))); FN_SET(0); FH_SET(1);
}

//BIT 1,E | Z:Z N:0 H:1 C:-
void ins_CB_4B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(E()&(1<<1))); FN_SET(0); FH_SET(1);
}

//BIT 1,H | Z:Z N:0 H:1 C:-
void ins_CB_4C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(H()&(1<<1))); FN_SET(0); FH_SET(1);
}

//BIT 1,L | Z:Z N:0 H:1 C:-
void ins_CB_4D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(L()&(1<<1))); FN_SET(0); FH_SET(1);
}

//BIT 1,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_4E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<1)); FN_SET(0); FH_SET(1);
}

//BIT 1,A | Z:Z N:0 H:1 C:-
void ins_CB_4F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(A()&(1<<1))); FN_SET(0); FH_SET(1);
}

//BIT 2,B | Z:Z N:0 H:1 C:-
void ins_CB_50(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(B()&(1<<2))); FN_SET(0); FH_SET(1);
}

//BIT 2,C | Z:Z N:0 H:1 C:-
void ins_CB_51(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(C()&(1<<2))); FN_SET(0); FH_SET(1);
}

//BIT 2,D | Z:Z N:0 H:1 C:-
void ins_CB_52(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(D()&(1<<2))); FN_SET(0); FH_SET(1);
}

//BIT 2,E | Z:Z N:0 H:1 C:-
void ins_CB_53(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(E()&(1<<2))); FN_SET(0); FH_SET(1);
}

//BIT 2,H | Z:Z N:0 H:1 C:-
void ins_CB_54(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(H()&(1<<2))); FN_SET(0); FH_SET(1);
}

//BIT 2,L | Z:Z N:0 H:1 C:-
void ins_CB_55(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(L()&(1<<2))); FN_SET(0); FH_SET(1);
}

//BIT 2,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_56(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<2)); FN_SET(0); FH_SET(1);
}

//BIT 2,A | Z:Z N:0 H:1 C:-
void ins_CB_57(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(A()&(1<<2))); FN_SET(0); FH_SET(1);
}

//BIT 3,B | Z:Z N:0 H:1 C:-
void ins_CB_58(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(B()&(1<<3))); FN_SET(0); FH_SET(1);
}

//BIT 3,C | Z:Z N:0 H:1 C:-
void ins_CB_59(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(C()&(1<<3))); FN_SET(0); FH_SET(1);
}

//BIT 3,D | Z:Z N:0 H:1 C:-
void ins_CB_5A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(D()&(1<<3))); FN_SET(0); FH_SET(1);
}

//BIT 3,E | Z:Z N:0 H:1 C:-
void ins_CB_5B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(E()&(1<<3))); FN_SET(0); FH_SET(1);
}

//BIT 3,H | Z:Z N:0 H:1 C:-
void ins_CB_5C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(H()&(1<<3))); FN_SET(0); FH_SET(1);
}

//BIT 3,L | Z:Z N:0 H:1 C:-
void ins_CB_5D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(L()&(1<<3))); FN_SET(0); FH_SET(1);
}

//BIT 3,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_5E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<3)); FN_SET(0); FH_SET(1);
}

//BIT 3,A | Z:Z N:0 H:1 C:-
void ins_CB_5F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(A()&(1<<3))); FN_SET(0); FH_SET(1);
}

//BIT 4,B | Z:Z N:0 H:1 C:-
void ins_CB_60(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(B()&(1<<4))); FN_SET(0); FH_SET(1);
}

//BIT 4,C | Z:Z N:0 H:1 C:-
void ins_CB_61(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(C()&(1<<4))); FN_SET(0); FH_SET(1);
}

//BIT 4,D | Z:Z N:0 H:1 C:-
void ins_CB_62(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(D()&(1<<4))); FN_SET(0); FH_SET(1);
}

//BIT 4,E | Z:Z N:0 H:1 C:-
void ins_CB_63(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(E()&(1<<4))); FN_SET(0); FH_SET(1);
}

//BIT 4,H | Z:Z N:0 H:1 C:-
void ins_CB_64(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(H()&(1<<4))); FN_SET(0); FH_SET(1);
}

//BIT 4,L | Z:Z N:0 H:1 C:-
void ins_CB_65(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(L()&(1<<4))); FN_SET(0); FH_SET(1);
}

//BIT 4,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_66(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<4)); FN_SET(0); FH_SET(1);
}

//BIT 4,A | Z:Z N:0 H:1 C:-
void ins_CB_67(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(A()&(1<<4))); FN_SET(0); FH_SET(1);
}

//BIT 5,B | Z:Z N:0 H:1 C:-
void ins_CB_68(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(B()&(1<<5))); FN_SET(0); FH_SET(1);
}

//BIT 5,C | Z:Z N:0 H:1 C:-
void ins_CB_69(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(C()&(1<<5))); FN_SET(0); FH_SET(1);
}

//BIT 5,D | Z:Z N:0 H:1 C:-
void ins_CB_6A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(D()&(1<<5))); FN_SET(0); FH_SET(1);
}

//BIT 5,E | Z:Z N:0 H:1 C:-
void ins_CB_6B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(E()&(1<<5))); FN_SET(0); FH_SET(1);
}

//BIT 5,H | Z:Z N:0 H:1 C:-
void ins_CB_6C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(H()&(1<<5))); FN_SET(0); FH_SET(1);
}

//BIT 5,L | Z:Z N:0 H:1 C:-
void ins_CB_6D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(L()&(1<<5))); FN_SET(0); FH_SET(1);
}

//BIT 5,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_6E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<5)); FN_SET(0); FH_SET(1);
}

//BIT 5,A | Z:Z N:0 H:1 C:-
void ins_CB_6F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(A()&(1<<5))); FN_SET(0); FH_SET(1);
}

//BIT 6,B | Z:Z N:0 H:1 C:-
void ins_CB_70(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(B()&(1<<6))); FN_SET(0); FH_SET(1);
}

//BIT 6,C | Z:Z N:0 H:1 C:-
void ins_CB_71(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(C()&(1<<6))); FN_SET(0); FH_SET(1);
}

//BIT 6,D | Z:Z N:0 H:1 C:-
void ins_CB_72(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(D()&(1<<6))); FN_SET(0); FH_SET(1);
}

//BIT 6,E | Z:Z N:0 H:1 C:-
void ins_CB_73(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(E()&(1<<6))); FN_SET(0); FH_SET(1);
}

//BIT 6,H | Z:Z N:0 H:1 C:-
void ins_CB_74(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(H()&(1<<6))); FN_SET(0); FH_SET(1);
}

//BIT 6,L | Z:Z N:0 H:1 C:-
void ins_CB_75(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(L()&(1<<6))); FN_SET(0); FH_SET(1);
}

//BIT 6,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_76(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<6)); FN_SET(0); FH_SET(1);
}

//BIT 6,A | Z:Z N:0 H:1 C:-
void ins_CB_77(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(A()&(1<<6))); FN_SET(0); FH_SET(1);
}

//BIT 7,B | Z:Z N:0 H:1 C:-
void ins_CB_78(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(B()&(1<<7))); FN_SET(0); FH_SET(1);
}

//BIT 7,C | Z:Z N:0 H:1 C:-
void ins_CB_79(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(C()&(1<<7))); FN_SET(0); FH_SET(1);
}

//BIT 7,D | Z:Z N:0 H:1 C:-
void ins_CB_7A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(D()&(1<<7))); FN_SET(0); FH_SET(1);
}

//BIT 7,E | Z:Z N:0 H:1 C:-
void ins_CB_7B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(E()&(1<<7))); FN_SET(0); FH_SET(1);
}

//BIT 7,H | Z:Z N:0 H:1 C:-
void ins_CB_7C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(H()&(1<<7))); FN_SET(0); FH_SET(1);
}

//BIT 7,L | Z:Z N:0 H:1 C:-
void ins_CB_7D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(L()&(1<<7))); FN_SET(0); FH_SET(1);
}

//BIT 7,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_7E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<7)); FN_SET(0); FH_SET(1);
}

//BIT 7,A | Z:Z N:0 H:1 C:-
void ins_CB_7F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(!(A()&(1<<7))); FN_SET(0); FH_SET(1);
}

//RES 0,B | Z:- N:- H:- C:-
void ins_CB_80(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B() &(~(1<<0)));
}

//RES 0,C | Z:- N:- H:- C:-
void ins_CB_81(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C() &(~(1<<0)));
}

//RES 0,D | Z:- N:- H:- C:-
void ins_CB_82(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D() &(~(1<<0)));
}

//RES 0,E | Z:- N:- H:- C:-
void ins_CB_83(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E() &(~(1<<0)));
}

//RES 0,H | Z:- N:- H:- C:-
void ins_CB_84(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H() &(~(1<<0)));
}

//RES 0,L | Z:- N:- H:- C:-
void ins_CB_85(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L() &(~(1<<0)));
}

//RES 0,(HL) | Z:- N:- H:- C:-
void ins_CB_86(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 m = M_GET(HL());
    M_SET(HL(),m&(~(1<<0)));
}

//RES 0,A | Z:- N:- H:- C:-
void ins_CB_87(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A() &(~(1<<0)));
}

//RES 1,B | Z:- N:- H:- C:-
void ins_CB_88(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B() &(~(1<<1)));
}

//RES 1,C | Z:- N:- H:- C:-
void ins_CB_89(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C() &(~(1<<1)));
}

//RES 1,D | Z:- N:- H:- C:-
void ins_CB_8A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D() &(~(1<<1)));
}

//RES 1,E | Z:- N:- H:- C:-
void ins_CB_8B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E() &(~(1<<1)));
}

//RES 1,H | Z:- N:- H:- C:-
void ins_CB_8C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H() &(~(1<<1)));
}

//RES 1,L | Z:- N:- H:- C:-
void ins_CB_8D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L() &(~(1<<1)));
}

//RES 1,(HL) | Z:- N:- H:- C:-
void ins_CB_8E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())&(~(1<<1)));
}

//RES 1,A | Z:- N:- H:- C:-
void ins_CB_8F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A() &(~(1<<1)));
}

//RES 2,B | Z:- N:- H:- C:-
void ins_CB_90(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B() &(~(1<<2)));
}

//RES 2,C | Z:- N:- H:- C:-
void ins_CB_91(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C() &(~(1<<2)));
}

//RES 2,D | Z:- N:- H:- C:-
void ins_CB_92(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D() &(~(1<<2)));
}

//RES 2,E | Z:- N:- H:- C:-
void ins_CB_93(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E() &(~(1<<2)));
}

//RES 2,H | Z:- N:- H:- C:-
void ins_CB_94(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H() &(~(1<<2)));
}

//RES 2,L | Z:- N:- H:- C:-
void ins_CB_95(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L() &(~(1<<2)));
}

//RES 2,(HL) | Z:- N:- H:- C:-
void ins_CB_96(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())&(~(1<<2)));
}

//RES 2,A | Z:- N:- H:- C:-
void ins_CB_97(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A() &(~(1<<2)));
}

//RES 3,B | Z:- N:- H:- C:-
void ins_CB_98(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B() &(~(1<<3)));
}

//RES 3,C | Z:- N:- H:- C:-
void ins_CB_99(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C() &(~(1<<3)));
}

//RES 3,D | Z:- N:- H:- C:-
void ins_CB_9A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D() &(~(1<<3)));
}

//RES 3,E | Z:- N:- H:- C:-
void ins_CB_9B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E() &(~(1<<3)));
}

//RES 3,H | Z:- N:- H:- C:-
void ins_CB_9C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H() &(~(1<<3)));
}

//RES 3,L | Z:- N:- H:- C:-
void ins_CB_9D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L() &(~(1<<3)));
}

//RES 3,(HL) | Z:- N:- H:- C:-
void ins_CB_9E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())&(~(1<<3)));
}

//RES 3,A | Z:- N:- H:- C:-
void ins_CB_9F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A() &(~(1<<3)));
}

//RES 4,B | Z:- N:- H:- C:-
void ins_CB_A0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B() &(~(1<<4)));
}

//RES 4,C | Z:- N:- H:- C:-
void ins_CB_A1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C() &(~(1<<4)));
}

//RES 4,D | Z:- N:- H:- C:-
void ins_CB_A2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D() &(~(1<<4)));
}

//RES 4,E | Z:- N:- H:- C:-
void ins_CB_A3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E() &(~(1<<4)));
}

//RES 4,H | Z:- N:- H:- C:-
void ins_CB_A4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H() &(~(1<<4)));
}

//RES 4,L | Z:- N:- H:- C:-
void ins_CB_A5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L() &(~(1<<4)));
}

//RES 4,(HL) | Z:- N:- H:- C:-
void ins_CB_A6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())&(~(1<<4)));
}

//RES 4,A | Z:- N:- H:- C:-
void ins_CB_A7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A() &(~(1<<4)));
}

//RES 5,B | Z:- N:- H:- C:-
void ins_CB_A8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B() &(~(1<<5)));
}

//RES 5,C | Z:- N:- H:- C:-
void ins_CB_A9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C() &(~(1<<5)));
}

//RES 5,D | Z:- N:- H:- C:-
void ins_CB_AA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D() &(~(1<<5)));
}

//RES 5,E | Z:- N:- H:- C:-
void ins_CB_AB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E() &(~(1<<5)));
}

//RES 5,H | Z:- N:- H:- C:-
void ins_CB_AC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H() &(~(1<<5)));
}

//RES 5,L | Z:- N:- H:- C:-
void ins_CB_AD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L() &(~(1<<5)));
}

//RES 5,(HL) | Z:- N:- H:- C:-
void ins_CB_AE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())&(~(1<<5)));
}

//RES 5,A | Z:- N:- H:- C:-
void ins_CB_AF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A() &(~(1<<5)));
}

//RES 6,B | Z:- N:- H:- C:-
void ins_CB_B0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B() &(~(1<<6)));
}

//RES 6,C | Z:- N:- H:- C:-
void ins_CB_B1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C() &(~(1<<6)));
}

//RES 6,D | Z:- N:- H:- C:-
void ins_CB_B2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D() &(~(1<<6)));
}

//RES 6,E | Z:- N:- H:- C:-
void ins_CB_B3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E() &(~(1<<6)));
}

//RES 6,H | Z:- N:- H:- C:-
void ins_CB_B4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H() &(~(1<<6)));
}

//RES 6,L | Z:- N:- H:- C:-
void ins_CB_B5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L() &(~(1<<6)));
}

//RES 6,(HL) | Z:- N:- H:- C:-
void ins_CB_B6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())&(~(1<<6)));
}

//RES 6,A | Z:- N:- H:- C:-
void ins_CB_B7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A() &(~(1<<6)));
}

//RES 7,B | Z:- N:- H:- C:-
void ins_CB_B8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B() &(~(1<<7)));
}

//RES 7,C | Z:- N:- H:- C:-
void ins_CB_B9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C() &(~(1<<7)));
}

//RES 7,D | Z:- N:- H:- C:-
void ins_CB_BA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D() &(~(1<<7)));
}

//RES 7,E | Z:- N:- H:- C:-
void ins_CB_BB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E() &(~(1<<7)));
}

//RES 7,H | Z:- N:- H:- C:-
void ins_CB_BC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H() &(~(1<<7)));
}

//RES 7,L | Z:- N:- H:- C:-
void ins_CB_BD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L() &(~(1<<7)));
}

//RES 7,(HL) | Z:- N:- H:- C:-
void ins_CB_BE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())&(~(1<<7)));
}

//RES 7,A | Z:- N:- H:- C:-
void ins_CB_BF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A() &(~(1<<7)));
}

//SET 0,B | Z:- N:- H:- C:-
void ins_CB_C0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B()|(1<<0));
}

//SET 0,C | Z:- N:- H:- C:-
void ins_CB_C1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C()|(1<<0));
}

//SET 0,D | Z:- N:- H:- C:-
void ins_CB_C2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D()|(1<<0));
}

//SET 0,E | Z:- N:- H:- C:-
void ins_CB_C3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E()|(1<<0));
}

//SET 0,H | Z:- N:- H:- C:-
void ins_CB_C4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H()|(1<<0));
}

//SET 0,L | Z:- N:- H:- C:-
void ins_CB_C5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L()|(1<<0));
}

//SET 0,(HL) | Z:- N:- H:- C:-
void ins_CB_C6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())|(1<<0));
}

//SET 0,A | Z:- N:- H:- C:-
void ins_CB_C7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|(1<<0));
}

//SET 1,B | Z:- N:- H:- C:-
void ins_CB_C8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B()|(1<<1));
}

//SET 1,C | Z:- N:- H:- C:-
void ins_CB_C9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C()|(1<<1));
}

//SET 1,D | Z:- N:- H:- C:-
void ins_CB_CA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D()|(1<<1));
}

//SET 1,E | Z:- N:- H:- C:-
void ins_CB_CB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E()|(1<<1));
}

//SET 1,H | Z:- N:- H:- C:-
void ins_CB_CC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E()|(1<<1));
}

//SET 1,L | Z:- N:- H:- C:-
void ins_CB_CD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L()|(1<<1));
}

//SET 1,(HL) | Z:- N:- H:- C:-
void ins_CB_CE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())|(1<<1));
}

//SET 1,A | Z:- N:- H:- C:-
void ins_CB_CF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|(1<<1));
}

//SET 2,B | Z:- N:- H:- C:-
void ins_CB_D0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B()|(1<<2));
}

//SET 2,C | Z:- N:- H:- C:-
void ins_CB_D1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C()|(1<<2));
}

//SET 2,D | Z:- N:- H:- C:-
void ins_CB_D2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D()|(1<<2));
}

//SET 2,E | Z:- N:- H:- C:-
void ins_CB_D3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E()|(1<<2));
}

//SET 2,H | Z:- N:- H:- C:-
void ins_CB_D4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H()|(1<<2));
}

//SET 2,L | Z:- N:- H:- C:-
void ins_CB_D5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L()|(1<<2));
}

//SET 2,(HL) | Z:- N:- H:- C:-
void ins_CB_D6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())|(1<<2));
}

//SET 2,A | Z:- N:- H:- C:-
void ins_CB_D7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|(1<<2));
}

//SET 3,B | Z:- N:- H:- C:-
void ins_CB_D8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B()|(1<<3));
}

//SET 3,C | Z:- N:- H:- C:-
void ins_CB_D9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C()|(1<<3));
}

//SET 3,D | Z:- N:- H:- C:-
void ins_CB_DA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D()|(1<<3));
}

//SET 3,E | Z:- N:- H:- C:-
void ins_CB_DB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E()|(1<<3));
}

//SET 3,H | Z:- N:- H:- C:-
void ins_CB_DC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H()|(1<<3));
}

//SET 3,L | Z:- N:- H:- C:-
void ins_CB_DD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L()|(1<<3));
}

//SET 3,(HL) | Z:- N:- H:- C:-
void ins_CB_DE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())|(1<<3));
}

//SET 3,A | Z:- N:- H:- C:-
void ins_CB_DF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|(1<<3));
}

//SET 4,B | Z:- N:- H:- C:-
void ins_CB_E0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B()|(1<<4));
}

//SET 4,C | Z:- N:- H:- C:-
void ins_CB_E1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C()|(1<<4));
}

//SET 4,D | Z:- N:- H:- C:-
void ins_CB_E2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D()|(1<<4));
}

//SET 4,E | Z:- N:- H:- C:-
void ins_CB_E3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E()|(1<<4));
}

//SET 4,H | Z:- N:- H:- C:-
void ins_CB_E4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H()|(1<<4));
}

//SET 4,L | Z:- N:- H:- C:-
void ins_CB_E5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L()|(1<<4));
}

//SET 4,(HL) | Z:- N:- H:- C:-
void ins_CB_E6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())|(1<<4));
}

//SET 4,A | Z:- N:- H:- C:-
void ins_CB_E7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|(1<<4));
}

//SET 5,B | Z:- N:- H:- C:-
void ins_CB_E8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B()|(1<<5));
}

//SET 5,C | Z:- N:- H:- C:-
void ins_CB_E9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C()|(1<<5));
}

//SET 5,D | Z:- N:- H:- C:-
void ins_CB_EA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D()|(1<<5));
}

//SET 5,E | Z:- N:- H:- C:-
void ins_CB_EB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E()|(1<<5));
}

//SET 5,H | Z:- N:- H:- C:-
void ins_CB_EC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H()|(1<<5));
}

//SET 5,L | Z:- N:- H:- C:-
void ins_CB_ED(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L()|(1<<5));
}

//SET 5,(HL) | Z:- N:- H:- C:-
void ins_CB_EE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())|(1<<5));
}

//SET 5,A | Z:- N:- H:- C:-
void ins_CB_EF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|(1<<5));
}

//SET 6,B | Z:- N:- H:- C:-
void ins_CB_F0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B()|(1<<6));
}

//SET 6,C | Z:- N:- H:- C:-
void ins_CB_F1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C()|(1<<6));
}

//SET 6,D | Z:- N:- H:- C:-
void ins_CB_F2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D()|(1<<6));
}

//SET 6,E | Z:- N:- H:- C:-
void ins_CB_F3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E()|(1<<6));
}

//SET 6,H | Z:- N:- H:- C:-
void ins_CB_F4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H()|(1<<6));
}

//SET 6,L | Z:- N:- H:- C:-
void ins_CB_F5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L()|(1<<6));
}

//SET 6,(HL) | Z:- N:- H:- C:-
void ins_CB_F6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())|(1<<6));
}

//SET 6,A | Z:- N:- H:- C:-
void ins_CB_F7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|(1<<6));
}

//SET 7,B | Z:- N:- H:- C:-
void ins_CB_F8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(B()|(1<<7));
}

//SET 7,C | Z:- N:- H:- C:-
void ins_CB_F9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(C()|(1<<7));
}

//SET 7,D | Z:- N:- H:- C:-
void ins_CB_FA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D()|(1<<7));
}

//SET 7,E | Z:- N:- H:- C:-
void ins_CB_FB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(E()|(1<<7));
}

//SET 7,H | Z:- N:- H:- C:-
void ins_CB_FC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(H()|(1<<7));
}

//SET 7,L | Z:- N:- H:- C:-
void ins_CB_FD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(L()|(1<<7));
}

//SET 7,(HL) | Z:- N:- H:- C:-
void ins_CB_FE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(), M_GET(HL())|(1<<7));
}

//SET 7,A | Z:- N:- H:- C:-
void ins_CB_FF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|(1<<7));
}