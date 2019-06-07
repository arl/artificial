#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "sht.h"
#include "art.h"
#include "shader.h"
#include "bitstring.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
    unsigned int vao;
    unsigned int vbo;
    unsigned int ntris;
} sht_ctx;

// TODO: remove those global contexts
struct art_ctx * gctx;
// TODO: remove those global contexts
sht_ctx * gshtctx;

int32_t sht_fill_buffers(float *vertex_data, size_t nvertex_data);

int32_t sht_init(uint32_t w, uint32_t h) {
    int32_t rc = art_create_ctx(&gctx, w, h);
    if (rc != EARTOK) {
        printf("Error %d: can't create art context\n", rc);
        return rc;
    }

    gshtctx = new sht_ctx;
    memset(gshtctx, 0, sizeof(sht_ctx));
    return EARTOK;
}

int32_t sht_render_image(unsigned char * img_data, size_t * bits, size_t nbits) {
    printf("we are in sht_render_image\n");
    bitstring bs(bits, 100);

    Shader shader("./shaders/shader.vert", "./shaders/shader.frag");

    float vertex_data[] = {
        // positions          // colors
        -0.9f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.05f,  // bottom left 
        -0.0f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.05f,  // bottom right
        -0.45f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.05f,  // top 

        // positions         // colors
        0.0f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, 0.05f,  // bottom left 
        0.9f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, 0.05f,  // bottom right
        0.45f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f, 0.05f,  // top 

        // positions         // colors
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.05f,  // bottom left 
        0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 0.05f, // bottom right
        0.f, 0.45f, 0.0f,0.0f, 0.0f, 1.0f, 0.05f,      // top 
    };

    sht_fill_buffers(&vertex_data[0], sizeof(vertex_data));
    art_set_screen_background(0.9f, 0.9f, 0.9f, 1.0f);
    shader.use();

    // Rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw all triangles at once from our vertex array.
    glBindVertexArray(gshtctx->vao);
    glDrawArrays(GL_TRIANGLES, 0, gshtctx->ntris * 3);

    // Extract from GPU and place into img_data
    return art_save_screen_png(gctx, "screen.png");
}

const unsigned int ncoords_per_vertex = 3;  // 3 floats per vertex (3d coords)
const unsigned int nchannels_per_color = 4; // 4 color components per vertex (RGBA)

// create vbo and vao, fill them with the vertex data. 
// TODO: pass the sht_ctx instead of using the global one
int32_t sht_fill_buffers(float *vertex_data, size_t nvertex_data) {
    gshtctx->ntris = nvertex_data / (sizeof(float) * (3 * ncoords_per_vertex + 3 * nchannels_per_color));

    // create a vertex array and bind it.
    glGenVertexArrays(1, &gshtctx->vao);
    glBindVertexArray(gshtctx->vao);

    // create a vertex buffer and bind it.
    glGenBuffers(1, &gshtctx->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gshtctx->vbo);

    // stores vertex data for all triangles at once in the vertex buffer.
    glBufferData(GL_ARRAY_BUFFER, nvertex_data, vertex_data, GL_STATIC_DRAW);

    // specify attribute pointers: position and color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); // vec3 position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))); // vec3 color
    glEnableVertexAttribArray(1);

    // unbind the vertex buffer and the vertex array.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return EARTOK;
}

int32_t sht_cleanup() {
    glDeleteVertexArrays(1, &gshtctx->vao);
    glDeleteBuffers(1, &gshtctx->vbo);
    delete gshtctx;    
    gshtctx = nullptr;
    int32_t rc = art_destroy_context(gctx);
    if (rc != EARTOK) {
        printf("Error %d: can't destroy art context\n", rc);
        return rc;
    }
    return EARTOK;
}

