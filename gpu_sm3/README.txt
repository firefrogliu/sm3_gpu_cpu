
This project provides the function of finding a valid nounce. Mixxing this nounce with a given message with sm3 hash will yield a 256 bit number that is smaller than the given boundary. 

The host_find_valid_nounce() function in sm3.cu will loop over possible nounces to find the valid nounce. The searching process is fully parallelized using CUDA. It provides a 200x time efficiency over its cpu counterpart. The utilization of this function is demonstrated in the main() funciton of test.cu.

The basic sm3 funcitons are:
  void sm3_init(sm3_ctx *c),
  void sm3_update(sm3_ctx *c, const void *in, W len),
  void sm3_final(void *out,sm3_ctx *c),
which locate in sm3.cu.

The test_gpu_sm3_working_fine() in test.cu checks the correctness the CUDA version of sm3.


