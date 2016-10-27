#define _XOPEN_SOURCE 700
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sodium.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <assert.h>
#include "equihash.h"




int main(int argc, char **argv)
{
  if (argc < 2) {
    fprintf(stderr, "%s <ierations>\n", argv[0]);
    exit(1);
  }

  gpu_config_t *config = calloc(sizeof(gpu_config_t), 1);
  equihash_init(config);

  sodium_init();
  size_t n_iterations = strtoul(argv[1], NULL, 10);

  crypto_generichash_blake2b_state curr_state;
  crypto_generichash_blake2b_init(&curr_state, (const unsigned char *) "", 0,
                                  (512 / 200) * 200 / 8);

  for (uint32_t i = 0; i < n_iterations; ++i) {
    crypto_generichash_blake2b_state state = curr_state;
    crypto_generichash_blake2b_update(&state, (const uint8_t *) &i, 4);
    uint32_t dst_indices[20 * 512];
    size_t n_solutions = equihash(dst_indices, &state, config);

    for (size_t i = 0; i < n_solutions; ++i) {
      for (size_t k = 0; k < 512; ++k) {
        printf("%u ", dst_indices[i * 512 + k]);
      }
      printf("\n\n");
    }
  }
  equihash_cleanup(config);
  free(config);
  return 0;
}
