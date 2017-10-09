#include <Arduino.h>

#include <avr/pgmspace.h>

const char code_00[] PROGMEM = "NOP"; 
const char code_01[] PROGMEM = "LD BC,nn"; 
const char code_02[] PROGMEM = "LD (BC),A"; 
const char code_03[] PROGMEM = "INC BC"; 
const char code_04[] PROGMEM = "INC B"; 
const char code_05[] PROGMEM = "DEC B"; 
const char code_06[] PROGMEM = "LD B,n"; 
const char code_07[] PROGMEM = "RLCA"; 
const char code_08[] PROGMEM = "EX AF,AF'"; 
const char code_09[] PROGMEM = "ADD HL,BC"; 
const char code_0A[] PROGMEM = "LD A,(BC)"; 
const char code_0B[] PROGMEM = "DEC BC"; 
const char code_0C[] PROGMEM = "INC C"; 
const char code_0D[] PROGMEM = "DEC C"; 
const char code_0E[] PROGMEM = "LD C,n"; 
const char code_0F[] PROGMEM = "RRCA"; 
const char code_10[] PROGMEM = "DJNZ nn"; 
const char code_11[] PROGMEM = "LD DE,nn"; 
const char code_12[] PROGMEM = "LD (DE),A"; 
const char code_13[] PROGMEM = "INC DE"; 
const char code_14[] PROGMEM = "INC D"; 
const char code_15[] PROGMEM = "DEC D"; 
const char code_16[] PROGMEM = "LD D,n"; 
const char code_17[] PROGMEM = "RLA"; 
const char code_18[] PROGMEM = "JR n"; 
const char code_19[] PROGMEM = "ADD HL,DE"; 
const char code_1A[] PROGMEM = "LD A,(DE)"; 
const char code_1B[] PROGMEM = "DEC DE"; 
const char code_1C[] PROGMEM = "INC E"; 
const char code_1D[] PROGMEM = "DEC E"; 
const char code_1E[] PROGMEM = "LD E,n"; 
const char code_1F[] PROGMEM = "RRA"; 
const char code_20[] PROGMEM = "JR NZ,n"; 
const char code_21[] PROGMEM = "LD HL,nn"; 
const char code_22[] PROGMEM = "LD (nn),HL"; 
const char code_23[] PROGMEM = "INC HL"; 
const char code_24[] PROGMEM = "INC H"; 
const char code_25[] PROGMEM = "DEC H"; 
const char code_26[] PROGMEM = "LD H,n"; 
const char code_27[] PROGMEM = "DAA"; 
const char code_28[] PROGMEM = "JR Z,n"; 
const char code_29[] PROGMEM = "ADD HL,HL"; 
const char code_2A[] PROGMEM = "LD HL,(nn)"; 
const char code_2B[] PROGMEM = "DEC HL"; 
const char code_2C[] PROGMEM = "INC L"; 
const char code_2D[] PROGMEM = "DEC L"; 
const char code_2E[] PROGMEM = "LD L,n"; 
const char code_2F[] PROGMEM = "CPL"; 
const char code_30[] PROGMEM = "JR NC,nn"; 
const char code_31[] PROGMEM = "LD SP,nn"; 
const char code_32[] PROGMEM = "LD (nn),A"; 
const char code_33[] PROGMEM = "INC SP"; 
const char code_34[] PROGMEM = "INC (HL)"; 
const char code_35[] PROGMEM = "DEC (HL)"; 
const char code_36[] PROGMEM = "LD (HL),n"; 
const char code_37[] PROGMEM = "SCF"; 
const char code_38[] PROGMEM = "JR C,nn"; 
const char code_39[] PROGMEM = "ADD HL,SP"; 
const char code_3A[] PROGMEM = "LD A,(nn)"; 
const char code_3B[] PROGMEM = "DEC SP"; 
const char code_3C[] PROGMEM = "INC A"; 
const char code_3D[] PROGMEM = "DEC A"; 
const char code_3E[] PROGMEM = "LD A,n"; 
const char code_3F[] PROGMEM = "CCF"; 
const char code_40[] PROGMEM = "LD B,B"; 
const char code_41[] PROGMEM = "LD B,C"; 
const char code_42[] PROGMEM = "LD B,D"; 
const char code_43[] PROGMEM = "LD B,E"; 
const char code_44[] PROGMEM = "LD B,H"; 
const char code_45[] PROGMEM = "LD B,L"; 
const char code_46[] PROGMEM = "LD B,(HL)"; 
const char code_47[] PROGMEM = "LD B,A"; 
const char code_48[] PROGMEM = "LD C,B"; 
const char code_49[] PROGMEM = "LD C,C"; 
const char code_4A[] PROGMEM = "LD C,D"; 
const char code_4B[] PROGMEM = "LD C,E"; 
const char code_4C[] PROGMEM = "LD C,H"; 
const char code_4D[] PROGMEM = "LD C,L"; 
const char code_4E[] PROGMEM = "LD C,(HL)"; 
const char code_4F[] PROGMEM = "LD C,A"; 
const char code_50[] PROGMEM = "LD D,B"; 
const char code_51[] PROGMEM = "LD D,C"; 
const char code_52[] PROGMEM = "LD D,D"; 
const char code_53[] PROGMEM = "LD D,E"; 
const char code_54[] PROGMEM = "LD D,H"; 
const char code_55[] PROGMEM = "LD D,L"; 
const char code_56[] PROGMEM = "LD D,(HL)"; 
const char code_57[] PROGMEM = "LD D,A"; 
const char code_58[] PROGMEM = "LD E,B"; 
const char code_59[] PROGMEM = "LD E,C"; 
const char code_5A[] PROGMEM = "LD E,D"; 
const char code_5B[] PROGMEM = "LD E,E"; 
const char code_5C[] PROGMEM = "LD E,H"; 
const char code_5D[] PROGMEM = "LD E,L"; 
const char code_5E[] PROGMEM = "LD E,(HL)"; 
const char code_5F[] PROGMEM = "LD E,A"; 
const char code_60[] PROGMEM = "LD H,B"; 
const char code_61[] PROGMEM = "LD H,C"; 
const char code_62[] PROGMEM = "LD H,D"; 
const char code_63[] PROGMEM = "LD H,E"; 
const char code_64[] PROGMEM = "LD H,H"; 
const char code_65[] PROGMEM = "LD H,L"; 
const char code_66[] PROGMEM = "LD H,(HL)"; 
const char code_67[] PROGMEM = "LD H,A"; 
const char code_68[] PROGMEM = "LD L,B"; 
const char code_69[] PROGMEM = "LD L,C"; 
const char code_6A[] PROGMEM = "LD L,D"; 
const char code_6B[] PROGMEM = "LD L,E"; 
const char code_6C[] PROGMEM = "LD L,H"; 
const char code_6D[] PROGMEM = "LD L,L"; 
const char code_6E[] PROGMEM = "\x01LD L,(HL)"; 
const char code_6F[] PROGMEM = "LD L,A"; 
const char code_70[] PROGMEM = "LD (HL),B"; 
const char code_71[] PROGMEM = "LD (HL),C"; 
const char code_72[] PROGMEM = "LD (HL),D"; 
const char code_73[] PROGMEM = "LD (HL),E"; 
const char code_74[] PROGMEM = "LD (HL),H"; 
const char code_75[] PROGMEM = "LD (HL),L"; 
const char code_76[] PROGMEM = "HALT"; 
const char code_77[] PROGMEM = "LD (HL),A"; 
const char code_78[] PROGMEM = "LD A,B"; 
const char code_79[] PROGMEM = "LD A,C"; 
const char code_7A[] PROGMEM = "LD A,D"; 
const char code_7B[] PROGMEM = "LD A,E"; 
const char code_7C[] PROGMEM = "LD A,H"; 
const char code_7D[] PROGMEM = "LD A,L"; 
const char code_7E[] PROGMEM = "LD A,(HL)"; 
const char code_7F[] PROGMEM = "LD A,A"; 
const char code_80[] PROGMEM = "ADD A,B"; 
const char code_81[] PROGMEM = "ADD A,C"; 
const char code_82[] PROGMEM = "ADD A,D"; 
const char code_83[] PROGMEM = "ADD A,E"; 
const char code_84[] PROGMEM = "ADD A,H"; 
const char code_85[] PROGMEM = "ADD A,L"; 
const char code_86[] PROGMEM = "ADD A,(HL)"; 
const char code_87[] PROGMEM = "ADD A,A"; 
const char code_88[] PROGMEM = "ADC A,B"; 
const char code_89[] PROGMEM = "ADC A,C"; 
const char code_8A[] PROGMEM = "ADC A,D"; 
const char code_8B[] PROGMEM = "ADC A,E"; 
const char code_8C[] PROGMEM = "ADC A,H"; 
const char code_8D[] PROGMEM = "ADC A,L"; 
const char code_8E[] PROGMEM = "ADC A,(HL)"; 
const char code_8F[] PROGMEM = "ADC A,A"; 
const char code_90[] PROGMEM = "SUB B"; 
const char code_91[] PROGMEM = "SUB C"; 
const char code_92[] PROGMEM = "SUB D"; 
const char code_93[] PROGMEM = "SUB E"; 
const char code_94[] PROGMEM = "SUB H"; 
const char code_95[] PROGMEM = "SUB L"; 
const char code_96[] PROGMEM = "SUB (HL)"; 
const char code_97[] PROGMEM = "SUB A"; 
const char code_98[] PROGMEM = "SBC B"; 
const char code_99[] PROGMEM = "SBC C"; 
const char code_9A[] PROGMEM = "SBC D"; 
const char code_9B[] PROGMEM = "SBC E"; 
const char code_9C[] PROGMEM = "SBC H"; 
const char code_9D[] PROGMEM = "SBC L"; 
const char code_9E[] PROGMEM = "SBC (HL)"; 
const char code_9F[] PROGMEM = "SBC A"; 
const char code_A0[] PROGMEM = "AND B"; 
const char code_A1[] PROGMEM = "AND C"; 
const char code_A2[] PROGMEM = "AND D"; 
const char code_A3[] PROGMEM = "AND E"; 
const char code_A4[] PROGMEM = "AND H"; 
const char code_A5[] PROGMEM = "AND L"; 
const char code_A6[] PROGMEM = "AND (HL)"; 
const char code_A7[] PROGMEM = "AND A"; 
const char code_A8[] PROGMEM = "XOR B"; 
const char code_A9[] PROGMEM = "XOR C"; 
const char code_AA[] PROGMEM = "XOR D"; 
const char code_AB[] PROGMEM = "XOR E"; 
const char code_AC[] PROGMEM = "XOR H"; 
const char code_AD[] PROGMEM = "XOR L"; 
const char code_AE[] PROGMEM = "XOR (HL)"; 
const char code_AF[] PROGMEM = "XOR A"; 
const char code_B0[] PROGMEM = "OR B"; 
const char code_B1[] PROGMEM = "OR C"; 
const char code_B2[] PROGMEM = "OR D"; 
const char code_B3[] PROGMEM = "OR E"; 
const char code_B4[] PROGMEM = "OR H"; 
const char code_B5[] PROGMEM = "OR L"; 
const char code_B6[] PROGMEM = "OR (HL)"; 
const char code_B7[] PROGMEM = "OR A"; 
const char code_B8[] PROGMEM = "CP B"; 
const char code_B9[] PROGMEM = "CP C"; 
const char code_BA[] PROGMEM = "CP D"; 
const char code_BB[] PROGMEM = "CP E"; 
const char code_BC[] PROGMEM = "CP H"; 
const char code_BD[] PROGMEM = "CP L"; 
const char code_BE[] PROGMEM = "CP (HL)"; 
const char code_BF[] PROGMEM = "CP A"; 
const char code_C0[] PROGMEM = "RET NZ"; 
const char code_C1[] PROGMEM = "POP BC"; 
const char code_C2[] PROGMEM = "JP NZ,nn"; 
const char code_C3[] PROGMEM = "JP nn"; 
const char code_C4[] PROGMEM = "CALL NZ,nn"; 
const char code_C5[] PROGMEM = "PUSH BC"; 
const char code_C6[] PROGMEM = "ADD A,n"; 
const char code_C7[] PROGMEM = "RST 0"; 
const char code_C8[] PROGMEM = "RET Z"; 
const char code_C9[] PROGMEM = "RET"; 
const char code_CA[] PROGMEM = "JP Z,nn"; 
const char code_CB[] PROGMEM = "*CB"; 

const char code_CB00[] PROGMEM = "RLC B"; 
const char code_CB01[] PROGMEM = "RLC C"; 
const char code_CB02[] PROGMEM = "RLC D"; 
const char code_CB03[] PROGMEM = "RLC E"; 
const char code_CB04[] PROGMEM = "RLC H"; 
const char code_CB05[] PROGMEM = "RLC L"; 
const char code_CB06[] PROGMEM = "RLC (HL)"; 
const char code_CB07[] PROGMEM = "RLC A"; 
const char code_CB08[] PROGMEM = "RRC B"; 
const char code_CB09[] PROGMEM = "RRC C"; 
const char code_CB0A[] PROGMEM = "RRC D"; 
const char code_CB0B[] PROGMEM = "RRC E"; 
const char code_CB0C[] PROGMEM = "RRC H"; 
const char code_CB0D[] PROGMEM = "RRC L"; 
const char code_CB0E[] PROGMEM = "RRC (HL)"; 
const char code_CB0F[] PROGMEM = "RRC A"; 
const char code_CB10[] PROGMEM = "RL B"; 
const char code_CB11[] PROGMEM = "RL C"; 
const char code_CB12[] PROGMEM = "RL D"; 
const char code_CB13[] PROGMEM = "RL E"; 
const char code_CB14[] PROGMEM = "RL H"; 
const char code_CB15[] PROGMEM = "RL L"; 
const char code_CB16[] PROGMEM = "RL (HL)"; 
const char code_CB17[] PROGMEM = "RL A"; 
const char code_CB18[] PROGMEM = "RL B"; 
const char code_CB19[] PROGMEM = "RL C"; 
const char code_CB1A[] PROGMEM = "RL D"; 
const char code_CB1B[] PROGMEM = "RL E"; 
const char code_CB1C[] PROGMEM = "RL H"; 
const char code_CB1D[] PROGMEM = "RL L"; 
const char code_CB1E[] PROGMEM = "RL (HL)"; 
const char code_CB1F[] PROGMEM = "RL A"; 
const char code_CB20[] PROGMEM = "SLA B"; 
const char code_CB21[] PROGMEM = "SLA C"; 
const char code_CB22[] PROGMEM = "SLA D"; 
const char code_CB23[] PROGMEM = "SLA E"; 
const char code_CB24[] PROGMEM = "SLA H"; 
const char code_CB25[] PROGMEM = "SLA L"; 
const char code_CB26[] PROGMEM = "SLA (HL)"; 
const char code_CB27[] PROGMEM = "SLA A"; 
const char code_CB28[] PROGMEM = "SRA B"; 
const char code_CB29[] PROGMEM = "SRA C"; 
const char code_CB2A[] PROGMEM = "SRA D"; 
const char code_CB2B[] PROGMEM = "SRA E"; 
const char code_CB2C[] PROGMEM = "SRA H"; 
const char code_CB2D[] PROGMEM = "SRA L"; 
const char code_CB2E[] PROGMEM = "SRA (HL)"; 
const char code_CB2F[] PROGMEM = "SRA A"; 
const char code_CB38[] PROGMEM = "SRL B"; 
const char code_CB39[] PROGMEM = "SRL C"; 
const char code_CB3A[] PROGMEM = "SRL D"; 
const char code_CB3B[] PROGMEM = "SRL E"; 
const char code_CB3C[] PROGMEM = "SRL H"; 
const char code_CB3D[] PROGMEM = "SRL L"; 
const char code_CB3E[] PROGMEM = "SRL (HL)"; 
const char code_CB3F[] PROGMEM = "SRL A"; 
const char code_CB40[] PROGMEM = "BIT 0,B"; 
const char code_CB41[] PROGMEM = "BIT 0,C"; 
const char code_CB42[] PROGMEM = "BIT 0,D"; 
const char code_CB43[] PROGMEM = "BIT 0,E"; 
const char code_CB44[] PROGMEM = "BIT 0,H"; 
const char code_CB45[] PROGMEM = "BIT 0,L"; 
const char code_CB46[] PROGMEM = "BIT 0,(HL)"; 
const char code_CB47[] PROGMEM = "BIT 0,A"; 
const char code_CB48[] PROGMEM = "BIT 1,B"; 
const char code_CB49[] PROGMEM = "BIT 1,C"; 
const char code_CB4A[] PROGMEM = "BIT 1,D"; 
const char code_CB4B[] PROGMEM = "BIT 1,E"; 
const char code_CB4C[] PROGMEM = "BIT 1,H"; 
const char code_CB4D[] PROGMEM = "BIT 1,L"; 
const char code_CB4E[] PROGMEM = "BIT 1,(HL)"; 
const char code_CB4F[] PROGMEM = "BIT 1,A"; 
const char code_CB50[] PROGMEM = "BIT 2,B"; 
const char code_CB51[] PROGMEM = "BIT 2,C"; 
const char code_CB52[] PROGMEM = "BIT 2,D"; 
const char code_CB53[] PROGMEM = "BIT 2,E"; 
const char code_CB54[] PROGMEM = "BIT 2,H"; 
const char code_CB55[] PROGMEM = "BIT 2,L"; 
const char code_CB56[] PROGMEM = "BIT 2,(HL)"; 
const char code_CB57[] PROGMEM = "BIT 2,A"; 
const char code_CB58[] PROGMEM = "BIT 3,B"; 
const char code_CB59[] PROGMEM = "BIT 3,C"; 
const char code_CB5A[] PROGMEM = "BIT 3,D"; 
const char code_CB5B[] PROGMEM = "BIT 3,E"; 
const char code_CB5C[] PROGMEM = "BIT 3,H"; 
const char code_CB5D[] PROGMEM = "BIT 3,L"; 
const char code_CB5E[] PROGMEM = "BIT 3,(HL)"; 
const char code_CB5F[] PROGMEM = "BIT 3,A"; 
const char code_CB60[] PROGMEM = "BIT 4,B"; 
const char code_CB61[] PROGMEM = "BIT 4,C"; 
const char code_CB62[] PROGMEM = "BIT 4,D"; 
const char code_CB63[] PROGMEM = "BIT 4,E"; 
const char code_CB64[] PROGMEM = "BIT 4,H"; 
const char code_CB65[] PROGMEM = "BIT 4,L"; 
const char code_CB66[] PROGMEM = "BIT 4,(HL)"; 
const char code_CB67[] PROGMEM = "BIT 4,A"; 
const char code_CB68[] PROGMEM = "BIT 5,B"; 
const char code_CB69[] PROGMEM = "BIT 5,C"; 
const char code_CB6A[] PROGMEM = "BIT 5,D"; 
const char code_CB6B[] PROGMEM = "BIT 5,E"; 
const char code_CB6C[] PROGMEM = "BIT 5,H"; 
const char code_CB6D[] PROGMEM = "BIT 5,L"; 
const char code_CB6E[] PROGMEM = "BIT 5,(HL)"; 
const char code_CB6F[] PROGMEM = "BIT 5,A"; 
const char code_CB70[] PROGMEM = "BIT 6,B"; 
const char code_CB71[] PROGMEM = "BIT 6,C"; 
const char code_CB72[] PROGMEM = "BIT 6,D"; 
const char code_CB73[] PROGMEM = "BIT 6,E"; 
const char code_CB74[] PROGMEM = "BIT 6,H"; 
const char code_CB75[] PROGMEM = "BIT 6,L"; 
const char code_CB76[] PROGMEM = "BIT 6,(HL)"; 
const char code_CB77[] PROGMEM = "BIT 6,A"; 
const char code_CB78[] PROGMEM = "BIT 7,B"; 
const char code_CB79[] PROGMEM = "BIT 7,C"; 
const char code_CB7A[] PROGMEM = "BIT 7,D"; 
const char code_CB7B[] PROGMEM = "BIT 7,E"; 
const char code_CB7C[] PROGMEM = "BIT 7,H"; 
const char code_CB7D[] PROGMEM = "BIT 7,L"; 
const char code_CB7E[] PROGMEM = "BIT 7,(HL)"; 
const char code_CB7F[] PROGMEM = "BIT 7,A"; 
const char code_CB80[] PROGMEM = "RES 0,B"; 
const char code_CB81[] PROGMEM = "RES 0,C"; 
const char code_CB82[] PROGMEM = "RES 0,D"; 
const char code_CB83[] PROGMEM = "RES 0,E"; 
const char code_CB84[] PROGMEM = "RES 0,H"; 
const char code_CB85[] PROGMEM = "RES 0,L"; 
const char code_CB86[] PROGMEM = "RES 0,(HL)"; 
const char code_CB87[] PROGMEM = "RES 0,A"; 
const char code_CB88[] PROGMEM = "RES 1,B"; 
const char code_CB89[] PROGMEM = "RES 1,C"; 
const char code_CB8A[] PROGMEM = "RES 1,D"; 
const char code_CB8B[] PROGMEM = "RES 1,E"; 
const char code_CB8C[] PROGMEM = "RES 1,H"; 
const char code_CB8D[] PROGMEM = "RES 1,L"; 
const char code_CB8E[] PROGMEM = "RES 1,(HL)"; 
const char code_CB8F[] PROGMEM = "RES 1,A"; 
const char code_CB90[] PROGMEM = "RES 2,B"; 
const char code_CB91[] PROGMEM = "RES 2,C"; 
const char code_CB92[] PROGMEM = "RES 2,D"; 
const char code_CB93[] PROGMEM = "RES 2,E"; 
const char code_CB94[] PROGMEM = "RES 2,H"; 
const char code_CB95[] PROGMEM = "RES 2,L"; 
const char code_CB96[] PROGMEM = "RES 2,(HL)"; 
const char code_CB97[] PROGMEM = "RES 2,A"; 
const char code_CB98[] PROGMEM = "RES 3,B"; 
const char code_CB99[] PROGMEM = "RES 3,C"; 
const char code_CB9A[] PROGMEM = "RES 3,D"; 
const char code_CB9B[] PROGMEM = "RES 3,E"; 
const char code_CB9C[] PROGMEM = "RES 3,H"; 
const char code_CB9D[] PROGMEM = "RES 3,L"; 
const char code_CB9E[] PROGMEM = "RES 3,(HL)"; 
const char code_CB9F[] PROGMEM = "RES 3,A"; 
const char code_CBA0[] PROGMEM = "RES 4,B"; 
const char code_CBA1[] PROGMEM = "RES 4,C"; 
const char code_CBA2[] PROGMEM = "RES 4,D"; 
const char code_CBA3[] PROGMEM = "RES 4,E"; 
const char code_CBA4[] PROGMEM = "RES 4,H"; 
const char code_CBA5[] PROGMEM = "RES 4,L"; 
const char code_CBA6[] PROGMEM = "RES 4,(HL)"; 
const char code_CBA7[] PROGMEM = "RES 4,A"; 
const char code_CBA8[] PROGMEM = "RES 5,B"; 
const char code_CBA9[] PROGMEM = "RES 5,C"; 
const char code_CBAA[] PROGMEM = "RES 5,D"; 
const char code_CBAB[] PROGMEM = "RES 5,E"; 
const char code_CBAC[] PROGMEM = "RES 5,H"; 
const char code_CBAD[] PROGMEM = "RES 5,L"; 
const char code_CBAE[] PROGMEM = "RES 5,(HL)"; 
const char code_CBAF[] PROGMEM = "RES 5,A"; 
const char code_CBB0[] PROGMEM = "RES 6,B"; 
const char code_CBB1[] PROGMEM = "RES 6,C"; 
const char code_CBB2[] PROGMEM = "RES 6,D"; 
const char code_CBB3[] PROGMEM = "RES 6,E"; 
const char code_CBB4[] PROGMEM = "RES 6,H"; 
const char code_CBB5[] PROGMEM = "RES 6,L"; 
const char code_CBB6[] PROGMEM = "RES 6,(HL)"; 
const char code_CBB7[] PROGMEM = "RES 6,A"; 
const char code_CBB8[] PROGMEM = "RES 7,B"; 
const char code_CBB9[] PROGMEM = "RES 7,C"; 
const char code_CBBA[] PROGMEM = "RES 7,D"; 
const char code_CBBB[] PROGMEM = "RES 7,E"; 
const char code_CBBC[] PROGMEM = "RES 7,H"; 
const char code_CBBD[] PROGMEM = "RES 7,L"; 
const char code_CBBE[] PROGMEM = "RES 7,(HL)"; 
const char code_CBBF[] PROGMEM = "RES 7,A"; 
const char code_CBC0[] PROGMEM = "SET 0,B"; 
const char code_CBC1[] PROGMEM = "SET 0,C"; 
const char code_CBC2[] PROGMEM = "SET 0,D"; 
const char code_CBC3[] PROGMEM = "SET 0,E"; 
const char code_CBC4[] PROGMEM = "SET 0,H"; 
const char code_CBC5[] PROGMEM = "SET 0,L"; 
const char code_CBC6[] PROGMEM = "SET 0,(HL)"; 
const char code_CBC7[] PROGMEM = "SET 0,A"; 
const char code_CBC8[] PROGMEM = "SET 1,B"; 
const char code_CBC9[] PROGMEM = "SET 1,C"; 
const char code_CBCA[] PROGMEM = "SET 1,D"; 
const char code_CBCB[] PROGMEM = "SET 1,E"; 
const char code_CBCC[] PROGMEM = "SET 1,H"; 
const char code_CBCD[] PROGMEM = "SET 1,L"; 
const char code_CBCE[] PROGMEM = "SET 1,(HL)"; 
const char code_CBCF[] PROGMEM = "SET 1,A"; 
const char code_CBD0[] PROGMEM = "SET 2,B"; 
const char code_CBD1[] PROGMEM = "SET 2,C"; 
const char code_CBD2[] PROGMEM = "SET 2,D"; 
const char code_CBD3[] PROGMEM = "SET 2,E"; 
const char code_CBD4[] PROGMEM = "SET 2,H"; 
const char code_CBD5[] PROGMEM = "SET 2,L"; 
const char code_CBD6[] PROGMEM = "SET 2,(HL)"; 
const char code_CBD7[] PROGMEM = "SET 2,A"; 
const char code_CBD8[] PROGMEM = "SET 3,B"; 
const char code_CBD9[] PROGMEM = "SET 3,C"; 
const char code_CBDA[] PROGMEM = "SET 3,D"; 
const char code_CBDB[] PROGMEM = "SET 3,E"; 
const char code_CBDC[] PROGMEM = "SET 3,H"; 
const char code_CBDD[] PROGMEM = "SET 3,L"; 
const char code_CBDE[] PROGMEM = "SET 3,(HL)"; 
const char code_CBDF[] PROGMEM = "SET 3,A"; 
const char code_CBE0[] PROGMEM = "SET 4,B"; 
const char code_CBE1[] PROGMEM = "SET 4,C"; 
const char code_CBE2[] PROGMEM = "SET 4,D"; 
const char code_CBE3[] PROGMEM = "SET 4,E"; 
const char code_CBE4[] PROGMEM = "SET 4,H"; 
const char code_CBE5[] PROGMEM = "SET 4,L"; 
const char code_CBE6[] PROGMEM = "SET 4,(HL)"; 
const char code_CBE7[] PROGMEM = "SET 4,A"; 
const char code_CBE8[] PROGMEM = "SET 5,B"; 
const char code_CBE9[] PROGMEM = "SET 5,C"; 
const char code_CBEA[] PROGMEM = "SET 5,D"; 
const char code_CBEB[] PROGMEM = "SET 5,E"; 
const char code_CBEC[] PROGMEM = "SET 5,H"; 
const char code_CBED[] PROGMEM = "SET 5,L"; 
const char code_CBEE[] PROGMEM = "SET 5,(HL)"; 
const char code_CBEF[] PROGMEM = "SET 5,A"; 
const char code_CBF0[] PROGMEM = "SET 6,B"; 
const char code_CBF1[] PROGMEM = "SET 6,C"; 
const char code_CBF2[] PROGMEM = "SET 6,D"; 
const char code_CBF3[] PROGMEM = "SET 6,E"; 
const char code_CBF4[] PROGMEM = "SET 6,H"; 
const char code_CBF5[] PROGMEM = "SET 6,L"; 
const char code_CBF6[] PROGMEM = "SET 6,(HL)"; 
const char code_CBF7[] PROGMEM = "SET 6,A"; 
const char code_CBF8[] PROGMEM = "SET 7,B"; 
const char code_CBF9[] PROGMEM = "SET 7,C"; 
const char code_CBFA[] PROGMEM = "SET 7,D"; 
const char code_CBFB[] PROGMEM = "SET 7,E"; 
const char code_CBFC[] PROGMEM = "SET 7,H"; 
const char code_CBFD[] PROGMEM = "SET 7,L"; 
const char code_CBFE[] PROGMEM = "SET 7,(HL)"; 
const char code_CBFF[] PROGMEM = "SET 7,A"; 

const char code_CC[] PROGMEM = "CALL Z,nn"; 
const char code_CD[] PROGMEM = "CALL nn"; 
const char code_CE[] PROGMEM = "ADC A,n"; 
const char code_CF[] PROGMEM = "RST 8"; 
const char code_D0[] PROGMEM = "RET NC"; 
const char code_D1[] PROGMEM = "POP DE"; 
const char code_D2[] PROGMEM = "JP NC,nn"; 
const char code_D3[] PROGMEM = "OUT (n),A"; 
const char code_D4[] PROGMEM = "CALL NC,nn"; 
const char code_D5[] PROGMEM = "PUSH DE"; 
const char code_D6[] PROGMEM = "SUB n"; 
const char code_D7[] PROGMEM = "RST 10H"; 
const char code_D8[] PROGMEM = "RET C"; 
const char code_D9[] PROGMEM = "EXX"; 
const char code_DA[] PROGMEM = "JP C,nn"; 
const char code_DB[] PROGMEM = "IN A,(n)"; 
const char code_DC[] PROGMEM = "CALL C,nn"; 
const char code_DD[] PROGMEM = "*DD"; 

const char code_DD09[] PROGMEM = "ADD IX,BC"; 
const char code_DD19[] PROGMEM = "ADD IX,DE"; 
const char code_DD21[] PROGMEM = "LD IX,nn"; 
const char code_DD22[] PROGMEM = "LD (nn),IX"; 
const char code_DD23[] PROGMEM = "INC IX"; 
const char code_DD29[] PROGMEM = "ADD IX,IX"; 
const char code_DD2A[] PROGMEM = "LD IX,(nn)"; 
const char code_DD2B[] PROGMEM = "DEC IX"; 
const char code_DD34[] PROGMEM = "INC (IX+nn)"; 
const char code_DD35[] PROGMEM = "DEC (IX+nn)"; 
const char code_DD36[] PROGMEM = "LD (IX+nn),n"; 
const char code_DD39[] PROGMEM = "ADD IX,SP"; 
const char code_DD46[] PROGMEM = "LD B,(IX+nn)"; 
const char code_DD4E[] PROGMEM = "LD C,(IX+nn)"; 
const char code_DD56[] PROGMEM = "LD D,(IX+nn)"; 
const char code_DD5E[] PROGMEM = "LD E,(IX+nn)"; 
const char code_DD66[] PROGMEM = "LD H,(IX+nn)"; 
const char code_DD6E[] PROGMEM = "LD L,(IX+nn)"; 
const char code_DD70[] PROGMEM = "LD (IX+nn),B"; 
const char code_DD71[] PROGMEM = "LD (IX+nn),C"; 
const char code_DD72[] PROGMEM = "LD (IX+nn),D"; 
const char code_DD73[] PROGMEM = "LD (IX+nn),E"; 
const char code_DD74[] PROGMEM = "LD (IX+nn),H"; 
const char code_DD75[] PROGMEM = "LD (IX+nn),L"; 
const char code_DD76[] PROGMEM = "LD (IX+nn),n"; 
const char code_DD77[] PROGMEM = "LD (IX+nn),A"; 
const char code_DD7E[] PROGMEM = "LD A,(IX+nn)"; 
const char code_DD86[] PROGMEM = "ADD A,(IX+nn)"; 
const char code_DD8E[] PROGMEM = "ADC A,(IX+nn)"; 
const char code_DD96[] PROGMEM = "SUB (IX+nn)"; 
const char code_DD9E[] PROGMEM = "SBC (IX+nn)"; 
const char code_DDA6[] PROGMEM = "AND (IX+nn)"; 
const char code_DDAE[] PROGMEM = "XOR (IX+nn)"; 
const char code_DDB6[] PROGMEM = "OR (IX+nn)"; 
const char code_DDBE[] PROGMEM = "CP (IX+nn)"; 
/*
const char code_DDCBnn06[] PROGMEM = "RLC (IX+nn)"; 
const char code_DDCBnn0E[] PROGMEM = "RRC (IX+nn)"; 
const char code_DDCBnn16[] PROGMEM = "RL (IX+nn)"; 
const char code_DDCBnn1E[] PROGMEM = "RL (IX+nn)"; 
const char code_DDCBnn26[] PROGMEM = "SLA (IX+nn)"; 
const char code_DDCBnn2E[] PROGMEM = "SRA (IX+nn)"; 
const char code_DDCBnn3E[] PROGMEM = "SRL (IX+nn)"; 
const char code_DDCBnn46[] PROGMEM = "BIT 0,(IX+nn)"; 
const char code_DDCBnn4E[] PROGMEM = "BIT 1,(IX+nn)"; 
const char code_DDCBnn56[] PROGMEM = "BIT 2,(IX+nn)"; 
const char code_DDCBnn5E[] PROGMEM = "BIT 3,(IX+nn)"; 
const char code_DDCBnn66[] PROGMEM = "BIT 4,(IX+nn)"; 
const char code_DDCBnn6E[] PROGMEM = "BIT 5,(IX+nn)"; 
const char code_DDCBnn76[] PROGMEM = "BIT 6,(IX+nn)"; 
const char code_DDCBnn7E[] PROGMEM = "BIT 7,(IX+nn)"; 
const char code_DDCBnn86[] PROGMEM = "RES 0,(IX+nn)"; 
const char code_DDCBnn8E[] PROGMEM = "RES 1,(IX+nn)"; 
const char code_DDCBnn96[] PROGMEM = "RES 2,(IX+nn)"; 
const char code_DDCBnn9E[] PROGMEM = "RES 3,(IX+nn)"; 
const char code_DDCBnnA6[] PROGMEM = "RES 4,(IX+nn)"; 
const char code_DDCBnnAE[] PROGMEM = "RES 5,(IX+nn)"; 
const char code_DDCBnnB6[] PROGMEM = "RES 6,(IX+nn)"; 
const char code_DDCBnnBE[] PROGMEM = "RES 7,(IX+nn)"; 
const char code_DDCBnnC6[] PROGMEM = "SET 0,(IX+nn)"; 
const char code_DDCBnnCE[] PROGMEM = "SET 1,(IX+nn)"; 
const char code_DDCBnnD6[] PROGMEM = "SET 2,(IX+nn)"; 
const char code_DDCBnnDE[] PROGMEM = "SET 3,(IX+nn)"; 
const char code_DDCBnnE6[] PROGMEM = "SET 4,(IX+nn)"; 
const char code_DDCBnnEE[] PROGMEM = "SET 5,(IX+nn)"; 
const char code_DDCBnnF6[] PROGMEM = "SET 6,(IX+nn)"; 
const char code_DDCBnnFE[] PROGMEM = "SET 7,(IX+nn)"; 
const char code_DDE1[] PROGMEM = "POP IX"; 
const char code_DDE3[] PROGMEM = "EX (SP),IX"; 
const char code_DDE5[] PROGMEM = "PUSH IX"; 
const char code_DDE9[] PROGMEM = "JP (IX)"; 
const char code_DDF9[] PROGMEM = "LD SP,IX"; 
*/
const char code_DE[] PROGMEM = "SBC n"; 
const char code_DF[] PROGMEM = "RST 18H"; 
const char code_E0[] PROGMEM = "RET PO"; 
const char code_E1[] PROGMEM = "POP HL"; 
const char code_E2[] PROGMEM = "JP PO,nn"; 
const char code_E3[] PROGMEM = "EX (SP),HL"; 
const char code_E4[] PROGMEM = "CALL PO,nn"; 
const char code_E5[] PROGMEM = "PUSH HL"; 
const char code_E6[] PROGMEM = "AND n"; 
const char code_E7[] PROGMEM = "RST 20H"; 
const char code_E8[] PROGMEM = "RET PE"; 
const char code_E9[] PROGMEM = "JP (HL)"; 
const char code_EA[] PROGMEM = "JP PE,nn"; 
const char code_EB[] PROGMEM = "EX DE,HL"; 
const char code_EC[] PROGMEM = "CALL PE,nn"; 
const char code_ED[] PROGMEM = "*ED"; 

const char code_ED40[] PROGMEM = "IN B,(C)"; 
const char code_ED41[] PROGMEM = "OUT (C),B"; 
const char code_ED42[] PROGMEM = "SBC HL,BC"; 
const char code_ED43[] PROGMEM = "LD (nn),BC"; 
const char code_ED44[] PROGMEM = "NEG"; 
const char code_ED45[] PROGMEM = "RETN"; 
const char code_ED46[] PROGMEM = "IM 0"; 
const char code_ED47[] PROGMEM = "LD I,A"; 
const char code_ED48[] PROGMEM = "IN C,(C)"; 
const char code_ED49[] PROGMEM = "OUT (C),C"; 
const char code_ED4A[] PROGMEM = "ADC HL,BC"; 
const char code_ED4B[] PROGMEM = "LD BC,(nn)"; 
const char code_ED4D[] PROGMEM = "RETI"; 
const char code_ED4F[] PROGMEM = "LD R,A"; 
const char code_ED50[] PROGMEM = "IN D,(C)"; 
const char code_ED51[] PROGMEM = "OUT (C),D"; 
const char code_ED52[] PROGMEM = "SBC HL,DE"; 
const char code_ED53[] PROGMEM = "LD (nn),DE"; 
const char code_ED56[] PROGMEM = "IM 1"; 
const char code_ED57[] PROGMEM = "LD A,I"; 
const char code_ED58[] PROGMEM = "IN E,(C)"; 
const char code_ED59[] PROGMEM = "OUT (C),E"; 
const char code_ED5A[] PROGMEM = "ADC HL,DE"; 
const char code_ED5B[] PROGMEM = "LD DE,(nn)"; 
const char code_ED5E[] PROGMEM = "IM 2"; 
const char code_ED5F[] PROGMEM = "LD A,R"; 
const char code_ED60[] PROGMEM = "IN H,(C)"; 
const char code_ED61[] PROGMEM = "OUT (C),H"; 
const char code_ED62[] PROGMEM = "SBC HL,HL"; 
const char code_ED67[] PROGMEM = "RRD"; 
const char code_ED68[] PROGMEM = "IN L,(C)"; 
const char code_ED69[] PROGMEM = "OUT (C),L"; 
const char code_ED6A[] PROGMEM = "ADC HL,HL"; 
const char code_ED63[] PROGMEM = "LD (nn),HL"; 
const char code_ED6B[] PROGMEM = "LD HL,(nn)"; 
const char code_ED6F[] PROGMEM = "RLD"; 
const char code_ED72[] PROGMEM = "SBC HL,SP"; 
const char code_ED73[] PROGMEM = "LD (nn),SP"; 
const char code_ED78[] PROGMEM = "IN A,(C)"; 
const char code_ED79[] PROGMEM = "OUT (C),A"; 
const char code_ED7A[] PROGMEM = "ADC HL,SP"; 
const char code_ED7B[] PROGMEM = "LD SP,(nn)"; 
const char code_EDA0[] PROGMEM = "LDI"; 
const char code_EDA1[] PROGMEM = "CPI"; 
const char code_EDA2[] PROGMEM = "INI"; 
const char code_EDA3[] PROGMEM = "OUTI"; 
const char code_EDA8[] PROGMEM = "LDD"; 
const char code_EDA9[] PROGMEM = "CPD"; 
const char code_EDAA[] PROGMEM = "IND"; 
const char code_EDAB[] PROGMEM = "OUTD"; 
const char code_EDB0[] PROGMEM = "LDIR"; 
const char code_EDB1[] PROGMEM = "CPIR"; 
const char code_EDB2[] PROGMEM = "INIR"; 
const char code_EDB3[] PROGMEM = "OTIR"; 
const char code_EDB8[] PROGMEM = "LDDR"; 
const char code_EDB9[] PROGMEM = "CPDR"; 
const char code_EDBA[] PROGMEM = "INDR"; 
const char code_EDBB[] PROGMEM = "OTDR"; 

const char code_EE[] PROGMEM = "XOR n"; 
const char code_EF[] PROGMEM = "RST 28H"; 
const char code_F0[] PROGMEM = "RET P"; 
const char code_F1[] PROGMEM = "POP AF"; 
const char code_F2[] PROGMEM = "JP P,nn"; 
const char code_F3[] PROGMEM = "DI"; 
const char code_F4[] PROGMEM = "CALL P,nn"; 
const char code_F5[] PROGMEM = "PUSH AF"; 
const char code_F6[] PROGMEM = "OR n"; 
const char code_F7[] PROGMEM = "RST 30H"; 
const char code_F8[] PROGMEM = "RET M"; 
const char code_F9[] PROGMEM = "LD SP,HL"; 
const char code_FA[] PROGMEM = "JP M,nn"; 
const char code_FB[] PROGMEM = "EI"; 
const char code_FC[] PROGMEM = "CALL M,nn"; 
const char code_FD[] PROGMEM = "*FD"; 

const char code_FD09[] PROGMEM = "ADD IY,BC"; 
const char code_FD19[] PROGMEM = "ADD IY,DE"; 
const char code_FD21[] PROGMEM = "LD IY,nn"; 
const char code_FD22[] PROGMEM = "LD (nn),IY"; 
const char code_FD23[] PROGMEM = "INC IY"; 
const char code_FD29[] PROGMEM = "ADD IY,IY"; 
const char code_FD2A[] PROGMEM = "LD IY,(nn)"; 
const char code_FD2B[] PROGMEM = "DEC IY"; 
const char code_FD34[] PROGMEM = "INC (IY+nn)"; 
const char code_FD35[] PROGMEM = "DEC (IY+nn)"; 
const char code_FD36[] PROGMEM = "LD (IY+nn),n"; 
const char code_FD39[] PROGMEM = "ADD IY,SP"; 
const char code_FD46[] PROGMEM = "LD B,(IY+nn)"; 
const char code_FD4E[] PROGMEM = "LD C,(IY+nn)"; 
const char code_FD56[] PROGMEM = "LD D,(IY+nn)"; 
const char code_FD5E[] PROGMEM = "LD E,(IY+nn)"; 
const char code_FD66[] PROGMEM = "LD H,(IY+nn)"; 
const char code_FD6E[] PROGMEM = "LD L,(IY+nn)"; 
const char code_FD70[] PROGMEM = "LD (IY+nn),B"; 
const char code_FD71[] PROGMEM = "LD (IY+nn),C"; 
const char code_FD72[] PROGMEM = "LD (IY+nn),D"; 
const char code_FD73[] PROGMEM = "LD (IY+nn),E"; 
const char code_FD74[] PROGMEM = "LD (IY+nn),H"; 
const char code_FD75[] PROGMEM = "LD (IY+nn),L"; 
const char code_FD76[] PROGMEM = "LD (IY+nn),n"; 
const char code_FD77[] PROGMEM = "LD (IY+nn),A"; 
const char code_FD7E[] PROGMEM = "LD A,(IY+nn)"; 
const char code_FD86[] PROGMEM = "ADD A,(IY+nn)"; 
const char code_FD8E[] PROGMEM = "ADC A,(IY+nn)"; 
const char code_FD96[] PROGMEM = "SUB (IY+nn)"; 
const char code_FD9E[] PROGMEM = "SBC (IY+nn)"; 
const char code_FDA6[] PROGMEM = "AND (IY+nn)"; 
const char code_FDAE[] PROGMEM = "XOR (IY+nn)"; 
const char code_FDB6[] PROGMEM = "OR (IY+nn)"; 
const char code_FDBE[] PROGMEM = "CP (IY+nn)"; 
/*
const char code_FDCBnn06[] PROGMEM = "RLC (IY+nn)"; 
const char code_FDCBnn0E[] PROGMEM = "RRC (IY+nn)"; 
const char code_FDCBnn16[] PROGMEM = "RL (IY+nn)"; 
const char code_FDCBnn1E[] PROGMEM = "RL (IY+nn)"; 
const char code_FDCBnn26[] PROGMEM = "SLA (IY+nn)"; 
const char code_FDCBnn2E[] PROGMEM = "SRA (IY+nn)"; 
const char code_FDCBnn3E[] PROGMEM = "SRL (IY+nn)"; 
const char code_FDCBnn46[] PROGMEM = "BIT 0,(IY+nn)"; 
const char code_FDCBnn4E[] PROGMEM = "BIT 1,(IY+nn)"; 
const char code_FDCBnn56[] PROGMEM = "BIT 2,(IY+nn)"; 
const char code_FDCBnn5E[] PROGMEM = "BIT 3,(IY+nn)"; 
const char code_FDCBnn66[] PROGMEM = "BIT 4,(IY+nn)"; 
const char code_FDCBnn6E[] PROGMEM = "BIT 5,(IY+nn)"; 
const char code_FDCBnn76[] PROGMEM = "BIT 6,(IY+nn)"; 
const char code_FDCBnn7E[] PROGMEM = "BIT 7,(IY+nn)"; 
const char code_FDCBnn86[] PROGMEM = "RES 0,(IY+nn)"; 
const char code_FDCBnn8E[] PROGMEM = "RES 1,(IY+nn)"; 
const char code_FDCBnn96[] PROGMEM = "RES 2,(IY+nn)"; 
const char code_FDCBnn9E[] PROGMEM = "RES 3,(IY+nn)"; 
const char code_FDCBnnA6[] PROGMEM = "RES 4,(IY+nn)"; 
const char code_FDCBnnAE[] PROGMEM = "RES 5,(IY+nn)"; 
const char code_FDCBnnB6[] PROGMEM = "RES 6,(IY+nn)"; 
const char code_FDCBnnBE[] PROGMEM = "RES 7,(IY+nn)"; 
const char code_FDCBnnC6[] PROGMEM = "SET 0,(IY+nn)"; 
const char code_FDCBnnCE[] PROGMEM = "SET 1,(IY+nn)"; 
const char code_FDCBnnD6[] PROGMEM = "SET 2,(IY+nn)"; 
const char code_FDCBnnDE[] PROGMEM = "SET 3,(IY+nn)"; 
const char code_FDCBnnE6[] PROGMEM = "SET 4,(IY+nn)"; 
const char code_FDCBnnEE[] PROGMEM = "SET 5,(IY+nn)"; 
const char code_FDCBnnF6[] PROGMEM = "SET 6,(IY+nn)"; 
const char code_FDCBnnFE[] PROGMEM = "SET 7,(IY+nn)"; 
const char code_FDE1[] PROGMEM = "POP IY"; 
const char code_FDE3[] PROGMEM = "EX (SP),IY"; 
const char code_FDE5[] PROGMEM = "PUSH IY"; 
const char code_FDE9[] PROGMEM = "JP (IY)"; 
const char code_FDF9[] PROGMEM = "LD SP,IY"; 
*/
const char code_FE[] PROGMEM = "CP n"; 
const char code_FF[] PROGMEM = "RST 38H"; 

const char* const opcode_table[] PROGMEM = {
  code_00, code_01, code_02, code_03, 
  code_04, code_05, code_06, code_07, 
  code_08, code_09, code_0A, code_0B, 
  code_0C, code_0D, code_0E, code_0F, 
  code_10, code_11, code_12, code_13, 
  code_14, code_15, code_16, code_17, 
  code_18, code_19, code_1A, code_1B, 
  code_1C, code_1D, code_1E, code_1F, 
  code_20, code_21, code_22, code_23, 
  code_24, code_25, code_26, code_27, 
  code_28, code_29, code_2A, code_2B, 
  code_2C, code_2D, code_2E, code_2F, 
  code_30, code_31, code_32, code_33, 
  code_34, code_35, code_36, code_37, 
  code_38, code_39, code_3A, code_3B, 
  code_3C, code_3D, code_3E, code_3F, 
  code_40, code_41, code_42, code_43, 
  code_44, code_45, code_46, code_47, 
  code_48, code_49, code_4A, code_4B, 
  code_4C, code_4D, code_4E, code_4F, 
  code_50, code_51, code_52, code_53, 
  code_54, code_55, code_56, code_57, 
  code_58, code_59, code_5A, code_5B, 
  code_5C, code_5D, code_5E, code_5F, 
  code_60, code_61, code_62, code_63, 
  code_64, code_65, code_66, code_67, 
  code_68, code_69, code_6A, code_6B, 
  code_6C, code_6D, code_6E, code_6F, 
  code_70, code_71, code_72, code_73, 
  code_74, code_75, code_76, code_77, 
  code_78, code_79, code_7A, code_7B, 
  code_7C, code_7D, code_7E, code_7F, 
  code_80, code_81, code_82, code_83, 
  code_84, code_85, code_86, code_87, 
  code_88, code_89, code_8A, code_8B, 
  code_8C, code_8D, code_8E, code_8F, 
  code_90, code_91, code_92, code_93, 
  code_94, code_95, code_96, code_97, 
  code_98, code_99, code_9A, code_9B, 
  code_9C, code_9D, code_9E, code_9F, 
  code_A0, code_A1, code_A2, code_A3, 
  code_A4, code_A5, code_A6, code_A7, 
  code_A8, code_A9, code_AA, code_AB, 
  code_AC, code_AD, code_AE, code_AF, 
  code_B0, code_B1, code_B2, code_B3, 
  code_B4, code_B5, code_B6, code_B7, 
  code_B8, code_B9, code_BA, code_BB, 
  code_BC, code_BD, code_BE, code_BF, 
  code_C0, code_C1, code_C2, code_C3, 
  code_C4, code_C5, code_C6, code_C7, 
  code_C8, code_C9, code_CA, code_CB, 
  code_CC, code_CD, code_CE, code_CF, 
  code_D0, code_D1, code_D2, code_D3, 
  code_D4, code_D5, code_D6, code_D7, 
  code_D8, code_D9, code_DA, code_DB, 
  code_DC, code_DD, code_DE, code_DF, 
  code_E0, code_E1, code_E2, code_E3, 
  code_E4, code_E5, code_E6, code_E7, 
  code_E8, code_E9, code_EA, code_EB, 
  code_EC, code_ED, code_EE, code_EF, 
  code_F0, code_F1, code_F2, code_F3, 
  code_F4, code_F5, code_F6, code_F7, 
  code_F8, code_F9, code_FA, code_FB, 
  code_FC, code_FD, code_FE, code_FF, 
};

const char* const opcode_CB_table[] PROGMEM = {
  code_CB00, code_CB01, code_CB02, code_CB03, 
  code_CB04, code_CB05, code_CB06, code_CB07,
  code_CB08, code_CB09, code_CB0A, code_CB0B,
  code_CB0C, code_CB0D, code_CB0E, code_CB0F,
  code_CB10, code_CB11, code_CB12, code_CB13,
  code_CB14, code_CB15, code_CB16, code_CB17,
  code_CB18, code_CB19, code_CB1A, code_CB1B,
  code_CB1C, code_CB1D, code_CB1E, code_CB1F,
  code_CB20, code_CB21, code_CB22, code_CB23,
  code_CB24, code_CB25, code_CB26, code_CB27,
  code_CB28, code_CB29, code_CB2A, code_CB2B,
  code_CB2C, code_CB2D, code_CB2E, code_CB2F,  
code_CB38,  
code_CB39,  
code_CB3A,  
code_CB3B,  
code_CB3C,  
code_CB3D,  
code_CB3E,  
code_CB3F,  
code_CB40,  
code_CB41,  
code_CB42,  
code_CB43,  
code_CB44,  
code_CB45,  
code_CB46,  
code_CB47,  
code_CB48,  
code_CB49,  
code_CB4A,  
code_CB4B,  
code_CB4C,  
code_CB4D,  
code_CB4E,  
code_CB4F,  
code_CB50,  
code_CB51,  
code_CB52,  
code_CB53,  
code_CB54,  
code_CB55,  
code_CB56,  
code_CB57,  
code_CB58,  
code_CB59,  
code_CB5A,  
code_CB5B,  
code_CB5C,  
code_CB5D,  
code_CB5E,  
code_CB5F,  
code_CB60,  
code_CB61,  
code_CB62,  
code_CB63,  
code_CB64,  
code_CB65,  
code_CB66,  
code_CB67,  
code_CB68,  
code_CB69,  
code_CB6A,  
code_CB6B,  
code_CB6C,  
code_CB6D,  
code_CB6E,  
code_CB6F,  
code_CB70,  
code_CB71,  
code_CB72,  
code_CB73,  
code_CB74,  
code_CB75,  
code_CB76,  
code_CB77,  
code_CB78,  
code_CB79,  
code_CB7A,  
code_CB7B,  
code_CB7C,  
code_CB7D,  
code_CB7E,  
code_CB7F,  
code_CB80,  
code_CB81,  
code_CB82,  
code_CB83,  
code_CB84,  
code_CB85,  
code_CB86,  
code_CB87,  
code_CB88,  
code_CB89,  
code_CB8A,  
code_CB8B,  
code_CB8C,  
code_CB8D,  
code_CB8E,  
code_CB8F,  
code_CB90,  
code_CB91,  
code_CB92,  
code_CB93,  
code_CB94,  
code_CB95,  
code_CB96,  
code_CB97,  
code_CB98,  
code_CB99,  
code_CB9A,  
code_CB9B,  
code_CB9C,  
code_CB9D,  
code_CB9E,  
code_CB9F,  
code_CBA0,  
code_CBA1,  
code_CBA2,  
code_CBA3,  
code_CBA4,  
code_CBA5,  
code_CBA6,  
code_CBA7,  
code_CBA8,  
code_CBA9,  
code_CBAA,  
code_CBAB,  
code_CBAC,  
code_CBAD,  
code_CBAE,  
code_CBAF,  
code_CBB0,  
code_CBB1,  
code_CBB2,  
code_CBB3,  
code_CBB4,  
code_CBB5,  
code_CBB6,  
code_CBB7,  
code_CBB8,  
code_CBB9,  
code_CBBA,  
code_CBBB,  
code_CBBC,  
code_CBBD,  
code_CBBE,  
code_CBBF,  
code_CBC0,  
code_CBC1,  
code_CBC2,  
code_CBC3,  
code_CBC4,  
code_CBC5,  
code_CBC6,  
code_CBC7,  
code_CBC8,  
code_CBC9,  
code_CBCA,  
code_CBCB,  
code_CBCC,  
code_CBCD,  
code_CBCE,  
code_CBCF,  
code_CBD0,  
code_CBD1,  
code_CBD2,  
code_CBD3,  
code_CBD4,  
code_CBD5,  
code_CBD6,  
code_CBD7,  
code_CBD8,  
code_CBD9,  
code_CBDA,  
code_CBDB,  
code_CBDC,  
code_CBDD,  
code_CBDE,  
code_CBDF,  
code_CBE0,  
code_CBE1,  
code_CBE2,  
code_CBE3,  
code_CBE4,  
code_CBE5,  
code_CBE6,  
code_CBE7,  
code_CBE8,  
code_CBE9,  
code_CBEA,  
code_CBEB,  
code_CBEC,  
code_CBED,  
code_CBEE,  
code_CBEF,  
code_CBF0,  
code_CBF1,  
code_CBF2,  
code_CBF3,  
code_CBF4,  
code_CBF5,  
code_CBF6,  
code_CBF7,  
code_CBF8,  
code_CBF9,  
code_CBFA,  
code_CBFB,  
code_CBFC,  
code_CBFD,  
code_CBFE,  
code_CBFF, 
};

const char* const opcode_DD_table[] PROGMEM = {
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
NULL, code_DD09, NULL, NULL, NULL, NULL, NULL, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
NULL, code_DD19, NULL, NULL, NULL, NULL, NULL, NULL, 
NULL, code_DD21, code_DD22, code_DD23, NULL, NULL, NULL, NULL, 
NULL, code_DD29, code_DD2A, code_DD2B, NULL, NULL, NULL, NULL, 
NULL, NULL, NULL, NULL, code_DD34, code_DD35, code_DD36, NULL, 
NULL, code_DD39, NULL, NULL, NULL, NULL, NULL, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DD46, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DD4E, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DD56, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DD5E, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DD66, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DD6E, NULL, 
code_DD70, code_DD71, code_DD72, code_DD73, code_DD74, code_DD75, code_DD76, code_DD77, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DD7E, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DD86, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DD8E, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DD96, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DD9E, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DDA6, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DDAE, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DDB6, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_DDBE, NULL, 
};

const char* const opcode_FD_table[] PROGMEM = {
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
NULL, code_FD09, NULL, NULL, NULL, NULL, NULL, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
NULL, code_FD19, NULL, NULL, NULL, NULL, NULL, NULL, 
NULL, code_FD21, code_FD22, code_FD23, NULL, NULL, NULL, NULL, 
NULL, code_FD29, code_FD2A, code_FD2B, NULL, NULL, NULL, NULL, 
NULL, NULL, NULL, NULL, code_FD34, code_FD35, code_FD36, NULL, 
NULL, code_FD39, NULL, NULL, NULL, NULL, NULL, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FD46, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FD4E, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FD56, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FD5E, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FD66, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FD6E, NULL, 
code_FD70, code_FD71, code_FD72, code_FD73, code_FD74, code_FD75, code_FD76, code_FD77, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FD7E, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FD86, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FD8E, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FD96, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FD9E, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FDA6, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FDAE, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FDB6, NULL, 
NULL, NULL, NULL, NULL, NULL, NULL, code_FDBE, NULL, 
};

const char* const opcode_ED40_table[] PROGMEM = {
  code_ED40, code_ED41, code_ED42, code_ED43, code_ED44, code_ED45, code_ED46, code_ED47, 
  code_ED48, code_ED49, code_ED4A, code_ED4B, NULL,      code_ED4D, NULL,      code_ED4F, 
  code_ED50, code_ED51, code_ED52, code_ED53, NULL,      NULL,      code_ED56, code_ED57, 
  code_ED58, code_ED59, code_ED5A, code_ED5B, NULL,      NULL,      code_ED5E, code_ED5F, 
  code_ED60, code_ED61, code_ED62, code_ED63, NULL,      NULL,      NULL,      code_ED67, 
  code_ED68, code_ED69, code_ED6A, code_ED6B, NULL,      NULL,      NULL,      code_ED6F, 
  NULL,      NULL,      code_ED72, code_ED73, NULL,      NULL,      NULL,      NULL,      
  code_ED78, code_ED79, code_ED7A, code_ED7B, NULL,      NULL,      NULL,      NULL,      
  NULL,      NULL,      NULL,      NULL,      NULL,      NULL,      NULL,      NULL,      
  NULL,      NULL,      NULL,      NULL,      NULL,      NULL,      NULL,      NULL,      
  NULL,      NULL,      NULL,      NULL,      NULL,      NULL,      NULL,      NULL,      
  NULL,      NULL,      NULL,      NULL,      NULL,      NULL,      NULL,      NULL,      
  code_EDA0, code_EDA1, code_EDA2, code_EDA3, NULL,      NULL,      NULL,      NULL,      
  code_EDA8, code_EDA9, code_EDAA, code_EDAB, NULL,      NULL,      NULL,      NULL,      
  code_EDB0, code_EDB1, code_EDB2, code_EDB3, NULL,      NULL,      NULL,      NULL,      
  code_EDB8, code_EDB9, code_EDBA, code_EDBB,
}; 
// Then set up a table to refer to your strings.
//    strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i]))); // Necessary casts and dereferencing, just 

char * opcode(uint32_t code, char * buf) {
  if ( code < 256 ) {
    strcpy_P(buf, (char*)pgm_read_word(&(opcode_table[code])));
  } else {
    buf[0] = 0;
    if ( (code & 0xff00) == 0xcb00 ) {
      strcpy_P(buf, (char*)pgm_read_word(&(opcode_CB_table[code & 0xff])));
    } else if ( (code & 0xff00) == 0xdd00 ) {
      if ( (code & 0xff) < 0xc0 )
        strcpy_P(buf, (char*)pgm_read_word(&(opcode_DD_table[code & 0xff])));
    } else if ( (code & 0xff00) == 0xed00 ) {
      if ( (code & 0xff) <= 0xbb && (code & 0xff) >= 0x40 )
        if ( (char*)pgm_read_word(&(opcode_ED40_table[(code & 0xff) - 0x40])) )
        strcpy_P(buf, (char*)pgm_read_word(&(opcode_ED40_table[(code & 0xff) - 0x40])));
    } else if ( (code & 0xff00) == 0xfd00 ) {
      if ( (code & 0xff) < 0xc0 )
        strcpy_P(buf, (char*)pgm_read_word(&(opcode_FD_table[code & 0xff])));
    }
  }
  return buf;
}

