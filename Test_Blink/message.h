struct message {
  const char * msg;
  int length;
  int bitpos;

  message() : msg(NULL), length(0), bitpos(0) {}; 
  message(const char * str) : msg(str), length(strlen(str)<<3), bitpos(0) {};

  void set(const char * str) {
    msg = str;
    bitpos = 0;
  }
  bool nextbit() {
    int pos = bitpos;
    bitpos = (bitpos + 1) % (length);
    return ((msg[pos>>3]>>(pos & 0x07)) & 1) != 0; 
  }
  
};
