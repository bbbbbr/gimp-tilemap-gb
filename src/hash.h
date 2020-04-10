// hash.h

#include "stdint.h"

uint64_t xtea_hash(uint32_t u32count, uint32_t * p_source_data);
uint64_t xtea_hash_u32(uint32_t u32count, uint32_t * p_source_data);

uint32_t MurmurHash2( const void * key, int len, uint32_t seed);