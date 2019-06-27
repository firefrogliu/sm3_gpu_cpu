
This project provides the function of finding a valid nounce. Mixxing this nounce with a given message with sm3 hash will yield a 256 bit number that is smaller than the given boundary. 

The find_valid_nounce() function in test.c will loop over possible nounces to find the valid nounce. The boundary, the message are also predefined in this fucntion. 

The basic sm3 funcitons are:
  void sm3_init(sm3_ctx *c),
  void sm3_update(sm3_ctx *c, const void *in, W len),
  void sm3_final(void *out,sm3_ctx *c),
which locate in sm3.c.

The sm3_correctness_test() in test.c check the correctness of sm3.


