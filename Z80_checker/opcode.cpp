#include <Arduino.h>

#include <avr/pgmspace.h>

const char code_00[] PROGMEM = "     ";
const char code_01[] PROGMEM = "     ";
const char code_02[] PROGMEM = "     ";
const char code_03[] PROGMEM = "     ";
const char code_04[] PROGMEM = "\1INC B";
const char code_05[] PROGMEM = "     ";
const char code_06[] PROGMEM = "\2LD B,n";
const char code_07[] PROGMEM = "     ";
const char code_08[] PROGMEM = "     ";
const char code_09[] PROGMEM = "     ";
const char code_0A[] PROGMEM = "     ";
const char code_0B[] PROGMEM = "     ";
const char code_0C[] PROGMEM = "     ";
const char code_0D[] PROGMEM = "     ";
const char code_0E[] PROGMEM = "     ";
const char code_0F[] PROGMEM = "     ";
const char code_10[] PROGMEM = "     ";
const char code_11[] PROGMEM = "     ";
const char code_12[] PROGMEM = "     ";
const char code_13[] PROGMEM = "     ";
const char code_14[] PROGMEM = "     ";
const char code_15[] PROGMEM = "     ";
const char code_16[] PROGMEM = "     ";
const char code_17[] PROGMEM = "     ";
const char code_18[] PROGMEM = "\2JR n";
const char code_19[] PROGMEM = "     ";
const char code_1A[] PROGMEM = "     ";
const char code_1B[] PROGMEM = "     ";
const char code_1C[] PROGMEM = "     ";
const char code_1D[] PROGMEM = "     ";
const char code_1E[] PROGMEM = "     ";
const char code_1F[] PROGMEM = "     ";
const char code_20[] PROGMEM = "     ";
const char code_21[] PROGMEM = "\3LD HL,nn";
const char code_22[] PROGMEM = "     ";
const char code_23[] PROGMEM = "\1INC HL";
const char code_24[] PROGMEM = "     ";
const char code_25[] PROGMEM = "     ";
const char code_26[] PROGMEM = "     ";
const char code_27[] PROGMEM = "     ";
const char code_28[] PROGMEM = "\2JR Z,n";
const char code_29[] PROGMEM = "     ";
const char code_2A[] PROGMEM = "     ";
const char code_2B[] PROGMEM = "     ";
const char code_2C[] PROGMEM = "     ";
const char code_2D[] PROGMEM = "     ";
const char code_2E[] PROGMEM = "     ";
const char code_2F[] PROGMEM = "     ";
const char code_30[] PROGMEM = "     ";
const char code_31[] PROGMEM = "\3LD SP, nn";
const char code_32[] PROGMEM = "\3LD (nn),A";
const char code_33[] PROGMEM = "     ";
const char code_34[] PROGMEM = "     ";
const char code_35[] PROGMEM = "     ";
const char code_36[] PROGMEM = "     ";
const char code_37[] PROGMEM = "     ";
const char code_38[] PROGMEM = "     ";
const char code_39[] PROGMEM = "     ";
const char code_3A[] PROGMEM = "\3LD A,(nn)";
const char code_3B[] PROGMEM = "     ";
const char code_3C[] PROGMEM = "\1INC A";
const char code_3D[] PROGMEM = "\1DEC A";
const char code_3E[] PROGMEM = "\2LD A,n";
const char code_3F[] PROGMEM = "     ";
const char code_40[] PROGMEM = "     ";
const char code_41[] PROGMEM = "     ";
const char code_42[] PROGMEM = "     ";
const char code_43[] PROGMEM = "     ";
const char code_44[] PROGMEM = "     ";
const char code_45[] PROGMEM = "     ";
const char code_46[] PROGMEM = "     ";
const char code_47[] PROGMEM = "\1LD B,A";
const char code_48[] PROGMEM = "     ";
const char code_49[] PROGMEM = "     ";
const char code_4A[] PROGMEM = "     ";
const char code_4B[] PROGMEM = "     ";
const char code_4C[] PROGMEM = "     ";
const char code_4D[] PROGMEM = "     ";
const char code_4E[] PROGMEM = "     ";
const char code_4F[] PROGMEM = "     ";
const char code_50[] PROGMEM = "     ";
const char code_51[] PROGMEM = "     ";
const char code_52[] PROGMEM = "     ";
const char code_53[] PROGMEM = "     ";
const char code_54[] PROGMEM = "     ";
const char code_55[] PROGMEM = "     ";
const char code_56[] PROGMEM = "     ";
const char code_57[] PROGMEM = "     ";
const char code_58[] PROGMEM = "     ";
const char code_59[] PROGMEM = "     ";
const char code_5A[] PROGMEM = "     ";
const char code_5B[] PROGMEM = "     ";
const char code_5C[] PROGMEM = "     ";
const char code_5D[] PROGMEM = "     ";
const char code_5E[] PROGMEM = "     ";
const char code_5F[] PROGMEM = "     ";
const char code_60[] PROGMEM = "     ";
const char code_61[] PROGMEM = "     ";
const char code_62[] PROGMEM = "     ";
const char code_63[] PROGMEM = "     ";
const char code_64[] PROGMEM = "     ";
const char code_65[] PROGMEM = "     ";
const char code_66[] PROGMEM = "     ";
const char code_67[] PROGMEM = "     ";
const char code_68[] PROGMEM = "     ";
const char code_69[] PROGMEM = "     ";
const char code_6A[] PROGMEM = "     ";
const char code_6B[] PROGMEM = "     ";
const char code_6C[] PROGMEM = "     ";
const char code_6D[] PROGMEM = "     ";
const char code_6E[] PROGMEM = "     ";
const char code_6F[] PROGMEM = "     ";
const char code_70[] PROGMEM = "     ";
const char code_71[] PROGMEM = "     ";
const char code_72[] PROGMEM = "     ";
const char code_73[] PROGMEM = "     ";
const char code_74[] PROGMEM = "     ";
const char code_75[] PROGMEM = "     ";
const char code_76[] PROGMEM = "\x1HALT";
const char code_77[] PROGMEM = "     ";
const char code_78[] PROGMEM = "\x1LD A,B";
const char code_79[] PROGMEM = "     ";
const char code_7A[] PROGMEM = "     ";
const char code_7B[] PROGMEM = "     ";
const char code_7C[] PROGMEM = "     ";
const char code_7D[] PROGMEM = "     ";
const char code_7E[] PROGMEM = "\x1LD A,(HL)";
const char code_7F[] PROGMEM = "     ";
const char code_80[] PROGMEM = "     ";
const char code_81[] PROGMEM = "     ";
const char code_82[] PROGMEM = "     ";
const char code_83[] PROGMEM = "     ";
const char code_84[] PROGMEM = "     ";
const char code_85[] PROGMEM = "     ";
const char code_86[] PROGMEM = "     ";
const char code_87[] PROGMEM = "     ";
const char code_88[] PROGMEM = "     ";
const char code_89[] PROGMEM = "     ";
const char code_8A[] PROGMEM = "     ";
const char code_8B[] PROGMEM = "     ";
const char code_8C[] PROGMEM = "     ";
const char code_8D[] PROGMEM = "     ";
const char code_8E[] PROGMEM = "     ";
const char code_8F[] PROGMEM = "     ";
const char code_90[] PROGMEM = "     ";
const char code_91[] PROGMEM = "     ";
const char code_92[] PROGMEM = "     ";
const char code_93[] PROGMEM = "     ";
const char code_94[] PROGMEM = "     ";
const char code_95[] PROGMEM = "     ";
const char code_96[] PROGMEM = "     ";
const char code_97[] PROGMEM = "     ";
const char code_98[] PROGMEM = "     ";
const char code_99[] PROGMEM = "     ";
const char code_9A[] PROGMEM = "     ";
const char code_9B[] PROGMEM = "     ";
const char code_9C[] PROGMEM = "     ";
const char code_9D[] PROGMEM = "     ";
const char code_9E[] PROGMEM = "     ";
const char code_9F[] PROGMEM = "     ";
const char code_A0[] PROGMEM = "     ";
const char code_A1[] PROGMEM = "     ";
const char code_A2[] PROGMEM = "     ";
const char code_A3[] PROGMEM = "     ";
const char code_A4[] PROGMEM = "     ";
const char code_A5[] PROGMEM = "     ";
const char code_A6[] PROGMEM = "     ";
const char code_A7[] PROGMEM = "\1AND A";
const char code_A8[] PROGMEM = "     ";
const char code_A9[] PROGMEM = "     ";
const char code_AA[] PROGMEM = "     ";
const char code_AB[] PROGMEM = "     ";
const char code_AC[] PROGMEM = "     ";
const char code_AD[] PROGMEM = "     ";
const char code_AE[] PROGMEM = "     ";
const char code_AF[] PROGMEM = "     ";
const char code_B0[] PROGMEM = "     ";
const char code_B1[] PROGMEM = "     ";
const char code_B2[] PROGMEM = "     ";
const char code_B3[] PROGMEM = "     ";
const char code_B4[] PROGMEM = "     ";
const char code_B5[] PROGMEM = "     ";
const char code_B6[] PROGMEM = "     ";
const char code_B7[] PROGMEM = "     ";
const char code_B8[] PROGMEM = "     ";
const char code_B9[] PROGMEM = "     ";
const char code_BA[] PROGMEM = "     ";
const char code_BB[] PROGMEM = "     ";
const char code_BC[] PROGMEM = "     ";
const char code_BD[] PROGMEM = "     ";
const char code_BE[] PROGMEM = "     ";
const char code_BF[] PROGMEM = "     ";
const char code_C0[] PROGMEM = "     ";
const char code_C1[] PROGMEM = "     ";
const char code_C2[] PROGMEM = "     ";
const char code_C3[] PROGMEM = "\x3JP nn";
const char code_C4[] PROGMEM = "     ";
const char code_C5[] PROGMEM = "     ";
const char code_C6[] PROGMEM = "     ";
const char code_C7[] PROGMEM = "     ";
const char code_C8[] PROGMEM = "     ";
const char code_C9[] PROGMEM = "     ";
const char code_CA[] PROGMEM = "     ";
const char code_CB[] PROGMEM = "     ";
const char code_CC[] PROGMEM = "     ";
const char code_CD[] PROGMEM = "     ";
const char code_CE[] PROGMEM = "     ";
const char code_CF[] PROGMEM = "     ";
const char code_D0[] PROGMEM = "     ";
const char code_D1[] PROGMEM = "     ";
const char code_D2[] PROGMEM = "     ";
const char code_D3[] PROGMEM = "\x2OUT (n),A";
const char code_D4[] PROGMEM = "     ";
const char code_D5[] PROGMEM = "     ";
const char code_D6[] PROGMEM = "     ";
const char code_D7[] PROGMEM = "     ";
const char code_D8[] PROGMEM = "     ";
const char code_D9[] PROGMEM = "     ";
const char code_DA[] PROGMEM = "     ";
const char code_DB[] PROGMEM = "\x2IN A,(n)";
const char code_DC[] PROGMEM = "     ";
const char code_DD[] PROGMEM = "     ";
const char code_DE[] PROGMEM = "     ";
const char code_DF[] PROGMEM = "     ";
const char code_E0[] PROGMEM = "     ";
const char code_E1[] PROGMEM = "     ";
const char code_E2[] PROGMEM = "     ";
const char code_E3[] PROGMEM = "     ";
const char code_E4[] PROGMEM = "     ";
const char code_E5[] PROGMEM = "     ";
const char code_E6[] PROGMEM = "     ";
const char code_E7[] PROGMEM = "     ";
const char code_E8[] PROGMEM = "     ";
const char code_E9[] PROGMEM = "     ";
const char code_EA[] PROGMEM = "     ";
const char code_EB[] PROGMEM = "     ";
const char code_EC[] PROGMEM = "     ";
const char code_ED[] PROGMEM = "     ";
const char code_EE[] PROGMEM = "     ";
const char code_EF[] PROGMEM = "     ";
const char code_F0[] PROGMEM = "     ";
const char code_F1[] PROGMEM = "     ";
const char code_F2[] PROGMEM = "     ";
const char code_F3[] PROGMEM = "     ";
const char code_F4[] PROGMEM = "     ";
const char code_F5[] PROGMEM = "     ";
const char code_F6[] PROGMEM = "     ";
const char code_F7[] PROGMEM = "     ";
const char code_F8[] PROGMEM = "     ";
const char code_F9[] PROGMEM = "     ";
const char code_FA[] PROGMEM = "     ";
const char code_FB[] PROGMEM = "     ";
const char code_FC[] PROGMEM = "     ";
const char code_FD[] PROGMEM = "     ";
const char code_FE[] PROGMEM = "     ";
const char code_FF[] PROGMEM = "     ";

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

// Then set up a table to refer to your strings.
//    strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i]))); // Necessary casts and dereferencing, just 

char * opcode(uint32_t code, char * buf) {
  int i = code & 0xff;
  strcpy_P(buf, (char*)pgm_read_word(&(opcode_table[i])));
  return buf;
}

