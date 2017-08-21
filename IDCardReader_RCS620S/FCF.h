#ifndef _FCF_H_
#define _FCF_H_

struct FCF_IDCard {
  uint8_t division;
  char pid[12];
  uint8_t issue;
  uint8_t gend;
  uint32_t orgid;
  uint32_t doi;
  uint32_t gdthru;
  
  uint8_t type;
  uint8_t id[8];

  enum {
    MFID = 1,
    FCF = 2,
    UNKNOWN = 0
  };
  
  char gender(void) { return (gend == 0 ? '?' : (gend == 1 ? 'M' : 'F')); }
};

#endif

