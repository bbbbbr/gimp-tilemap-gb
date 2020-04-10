// hash.c
//
// A minimal xtea & others hashing implementation

// See:
// https://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed

#include <stdio.h>
#include "hash.h"


 // Arbitrary key 4 x uint32_t
static uint32_t xtea_key[4] = {0x3326D2BB, 0x86F7E7BB, 0xD1A4C2D5, 0x5C9E8974};


// Hash some data using the xtea cryptographic algorithm
//
// Loop through each pair of uint32_t, encrypting
// them and then using the encrypted output
// as the new key for encrypting the next pair.
//
// --> Result gets returned as a uint64_t
uint64_t xtea_hash(uint32_t u64count, uint32_t * p_source_data)
{
    uint32_t c;
    uint32_t i;

    uint32_t v0;
    uint32_t v1;
    uint32_t sum;
    uint32_t delta;
    uint32_t working_key[4];

    // Initialize key
    working_key[0] = xtea_key[0];
    working_key[1] = xtea_key[1];
    working_key[2] = xtea_key[2];
    working_key[3] = xtea_key[3];

    // Initialize delta
    delta = 0x9E3779B9;

    // Loop through the data and hash it
    for (c = 0;c < u64count;c++)
    {
        // == START XTEA BLOCK ENCRYPT ==
        v0=p_source_data[(c*2) + 0];
        v1=p_source_data[(c*2) + 1];

        sum = 0; // reset sum for each encrypted block

        // 32 rounds
        for(i=0; i<32; i++) {
           v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + working_key[sum & 3]);
           sum += delta;
           v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + working_key[(sum>>11) & 3]);
        }
        // == END XTEA BLOCK ENCRYPT ==


        // Final step: hash by xoring encrypted data with unencrypted data
        // The hash output now becomes the key for next block to encrypt / hash
        working_key[0] = v0 ^ p_source_data[(c*2) + 0];
        working_key[1] = v1 ^ p_source_data[(c*2) + 1];
        working_key[2] = 0x00;
        working_key[3] = 0x00;
    }

    // Return final result into hash output that gets returned
    return( (uint64_t)working_key[0] | ((uint64_t)working_key[1] >> 32) );
}


// Hash some data using the xtea cryptographic algorithm
//
// *** IMPORTANT: Misuse xtea by making it operate on
//                32 input thats duplicated to meet the
//                required 64 bit block size.
//                Then increment by 32 bits instead of 64 bits.
//
//                Hopefully it still produces sufficient entropy.
//
// Loop through all the uint32_t's, encrypting
// them and then using the encrypted output
// as the new key for encrypting the next pair.
//
// --> Result gets returned as a uint64_t
uint64_t xtea_hash_u32(uint32_t u32count, uint32_t * p_source_data)
{
    uint32_t c;
    uint32_t i;

    uint32_t v0;
    uint32_t v1;
    uint32_t sum;
    uint32_t delta;
    uint32_t working_key[4];

    // Initialize key
    working_key[0] = xtea_key[0];
    working_key[1] = xtea_key[1];
    working_key[2] = xtea_key[2];
    working_key[3] = xtea_key[3];

    // Initialize delta
    delta = 0x9E3779B9;

//printf("--- u32count=%d (u8=%d)\n", u32count, u32count * 4);
    // Loop through the data and hash it
    for (c = 0;c < u32count;c++)
    {
        // == START XTEA BLOCK ENCRYPT ==
        // v0=p_source_data[(c*2) + 0];
        // v1=p_source_data[(c*2) + 1];
        v0=p_source_data[c];
        v1=p_source_data[c];

//printf(" %04x\n", p_source_data[c]);
        sum = 0; // reset sum for each encrypted block

        // 32 rounds
        for(i=0; i<32; i++) {
           v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + working_key[sum & 3]);
           sum += delta;
           v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + working_key[(sum>>11) & 3]);
        }
        // == END XTEA BLOCK ENCRYPT ==


        // Final step: hash by xoring encrypted data with unencrypted data
        // The hash output now becomes the key for next block to encrypt / hash

        // working_key[0] = v0 ^ p_source_data[(c*2) + 0];
        // working_key[1] = v1 ^ p_source_data[(c*2) + 1];
        working_key[0] = v0 ^ p_source_data[c];
        working_key[1] = v1 ^ p_source_data[c];
        working_key[2] = 0x00;
        working_key[3] = 0x00;
    }

//printf("* Hash::0x%08lx \n-----\n", (uint64_t)working_key[0] | ((uint64_t)working_key[1] >> 32));

    // Return final result into hash output that gets returned
    return( (uint64_t)working_key[0] | ((uint64_t)working_key[1] >> 32) );
}



// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
//
// * len: is u8count
// * About 8x faster than xtea_hash_u32()
//
uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = seed ^ len;

  // Mix 4 bytes at a time into the hash

  const unsigned char * data = (const unsigned char *)key;

  while(len >= 4)
  {
    uint32_t k = *(uint32_t*)data;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  switch(len)
  {
  case 3: h ^= data[2] << 16;
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}


