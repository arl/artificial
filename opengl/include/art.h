#ifndef INCLUDE_ART_H
#define INCLUDE_ART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define EARTOK 0        // success
#define EARTGEN 1  // generic failure
#define EARTIO 2       // input-output failure
#define EARTFB 3       // OpenGL framebuffer failure
#define EARTARG 4      // argument failure

struct art_ctx;

int32_t art_create_ctx(struct art_ctx ** ctx, uint32_t screen_width, uint32_t screen_height);
int32_t art_destroy_context(struct art_ctx * ctx);
// int art_set_background(float r, float g , float b, float a);
// int art_save_png(art_ctx * ctx, const char * filename);

#ifdef __cplusplus
}
#endif

#endif//INCLUDE_ART_H