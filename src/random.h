#ifndef RANDOM_H_
#define RANDOM_H_

#define CS_RAND_MAX 0x7fff
static unsigned long next = 1;

static inline unsigned cs_rand() {
  next = next * 0x41c64e6d + 0x3039;
  return (next >> 16) & 0x7fff;
}
static inline double cs_drand() {
  return cs_rand() / (double)0x8000;
}
static inline void cs_srand(unsigned int seed) {
  next = seed;
}

#endif //RANDOM_H_
