#include <Arduino.h>
#include "Random.h"

/* Seed variables */
static uint64_t jkx=159753,jky=456258,jkz=321987,jkc=6549173; 

uint64_t jkiss()
{
  uint64_t tmp;
  jkx=314527869*jkx+1234567;
  jky^=jky<<5;
  jky^=jky>>7;
  jky^=jky<<22;
  tmp=4294584393ULL*jkz+jkc;
  jkc=tmp>>32;
  jkz=tmp;
  return jkx+jky+jkz;
}

template<typename T>
T random(T min, T max) {
  return (min + jkiss()) % max;
}