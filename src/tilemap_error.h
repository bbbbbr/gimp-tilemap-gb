// tilemap_error.h

#include <stdint.h>

void tilemap_error_clear(void);
void tilemap_error_set(uint16_t error_num);
uint16_t tilemap_error_get(void);
const char * tilemap_error_get_string(void);