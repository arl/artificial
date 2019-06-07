#ifndef INCLUDE_SHT_H
#define INCLUDE_SHT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "art.h"

#if 0
/* Load original (or reference) image into a texture.
 */
sht_set_original_image(path *char);

/* Render bitstring into a texture and returns a value that represents how 
 * much the texture differs from the original image. 
 */
sht_diff(bitstring *Bitstring, value *float32);
#endif

int32_t sht_init(uint32_t w, uint32_t h);
int32_t sht_render_image(unsigned char * img_data, size_t * bits, size_t nbits);
int32_t sht_cleanup();

#ifdef __cplusplus
}
#endif

#endif //INCLUDE_SHT_H
