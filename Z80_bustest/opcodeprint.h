
size_t opcode(uint8_t code) {
  switch(code) {
  case 0x04:
    return Serial.print(F("INC B"));
    break;

  case 0x05:
    return Serial.print(F("DEC B"));
    break;

    case 0x06:
      return Serial.print(F("LD B,n"));
      break;
        
    case 0x0e:
      return Serial.print(F("LD C,n"));
      break;
        
    case 0x14:
      return Serial.print(F("LD B,A"));
      break;
        
    case 0x20:
      return Serial.print(F("JR NZ,"));
      break;
        
    case 0x21:
      return Serial.print(F("LD HL,nn"));
      break;
        
    case 0x22:
      return Serial.print(F("LD (nn),HL"));
      break;
        
    case 0x23:
      return Serial.print(F("INC HL"));
      break;
        
    case 0x28:
      return Serial.print(F("JR Z,"));
      break;
        
    case 0x32:
      return Serial.print(F("LD (nn),A"));
      break;
        
    case 0x36:
      return Serial.print(F("LD (HL),n"));
      break;
        
    case 0x3a:
      return Serial.print(F("LD A,(nn)"));
      break;
        
    case 0x3c:
      return Serial.print(F("INC A"));
      break;

    case 0x3d:
      return Serial.print(F("DEC A"));
      break;

    case 0x3e:
      return Serial.print(F("LD A, n"));
      break;

    case 0x47:
      return Serial.print(F("LD B,A"));
      break;

    case 0x70:
      return Serial.print(F("LD (HL),B"));
      break;

    case 0x76:
      return Serial.print(F("HALT"));
      break;

    case 0x78:
      return Serial.print(F("LD A,B"));
      break;

    case 0x79:
      return Serial.print(F("(C), A"));
      break;

    case 0x7e:
      return Serial.print(F("LD A,(HL)"));
      break;

    case 0xa7:
      return Serial.print(F("AND A"));
      break;
    
    case 0xc2:
      return Serial.print(F("JP NZ,nn"));
      break;
    
    case 0xc3:
      return Serial.print(F("JP"));
      break;
    
    case 0xca:
      return Serial.print(F("JP Z,nn"));
      break;

    case 0xd3:
      return Serial.print(F("OUT (n),A"));
      break;
        
    case 0xdb:
      return Serial.print(F("IN A,(n)"));
      break;

    case 0xed:
      return Serial.print(F("OUT "));
      break;
        
    default:
      return Serial.print(F(""));
      break;
  }
  return 0;
};

