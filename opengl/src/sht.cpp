#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdexcept>
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
    float *vertex_data;
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


// bitstring decoding constants
constexpr size_t nbpchannel = 8;              // number of bits per color channel
constexpr size_t nbpcolor   = nbpchannel * 4; // number of bits per color
constexpr size_t nbheader   = nbpcolor;       // number of bits in header

static_assert(nbpchannel == 8, "currently only nbpchannel == 8 is supported");

// vertex data encoding constants
constexpr size_t ncoordspvertex  = 3;  // 3 floats per vertex (3d coords)
constexpr size_t nchannelspcolor = 4;  // 4 color components per vertex (RGBA)
constexpr size_t nfloatspvertex  = ncoordspvertex + nchannelspcolor;
constexpr size_t nfloatsptri     = 3 * nfloatspvertex;

int32_t _sht_alloc_vertex_data()  {
    gshtctx->vertex_data = new float[nfloatsptri * gshtctx->ntris];
    return EARTOK;
}

void _sht_free_vertex_data() {
    if (gshtctx->vertex_data != nullptr)
        delete[] gshtctx;
    gshtctx->vertex_data = nullptr;
}

int32_t _sht_fill_vertex_data(const bitstring & bs, size_t wbits, size_t hbits) {
    const float w = float(1 << wbits);
    const float h = float(1 << hbits);
    const size_t nverts = 3;
    size_t ibit = 0;
    // skip header (background color)
    ibit += nbheader;
    float * verts = gshtctx->vertex_data; 
    for (size_t itri = 0; itri < gshtctx->ntris; ++itri) {
        // note: triangle are monocolor, i.e all vertices share an unique color
        float r = float(bs.gray8(ibit + nbpchannel*0)) / 255.f;
        float g = float(bs.gray8(ibit + nbpchannel*1)) / 255.f;
        float b = float(bs.gray8(ibit + nbpchannel*2)) / 255.f;
        float a = float(bs.gray8(ibit + nbpchannel*3)) / 255.f;
        ibit += nbpcolor;
        for (size_t ivert = 0; ivert < nverts; ++ivert) {
            // decode color from the bistring, into vertex data
            verts[itri*nfloatsptri + (nfloatspvertex * ivert) + ncoordspvertex + 0] = r;
            verts[itri*nfloatsptri + (nfloatspvertex * ivert) + ncoordspvertex + 1] = g;
            verts[itri*nfloatsptri + (nfloatspvertex * ivert) + ncoordspvertex + 2] = b;
            verts[itri*nfloatsptri + (nfloatspvertex * ivert) + ncoordspvertex + 3] = a;

            // decode position from the bistring, into vertex data
            verts[itri*nfloatsptri + (nfloatspvertex * ivert) + 0] = float(bs.grayn(ibit, wbits)) / w; // x
            ibit += wbits;
            verts[itri*nfloatsptri + (nfloatspvertex * ivert) + 1] = float(bs.grayn(ibit, hbits)) / h; // y
            ibit += hbits;
            verts[itri*nfloatsptri + (nfloatspvertex * ivert) + 2] = .0f; // z
        }
    }    
#ifdef DEBUG
	if (ibit != bs.length()) {
        #include <sstream>
        std::stringstream str("");
        str << "ibit:" << ibit << ", bs.length():" << bs.length() << std::endl;
		throw std::runtime_error(str.str());
	}
#endif
    return EARTOK;
}

size_t dbg_count = 0; // DBG: debug-only saved filename counter

int32_t sht_render_image(unsigned char * img_data, size_t * bits, size_t nbits) {
    const size_t wbits = 6, hbits = 6;
    // Deduce the number of triangles from:
    //  - constants
    //  - number of bits, nbits.
    //  - image dimensions, wbits/hbits
    const size_t ntris = (nbits - nbheader) / (3 * (wbits + hbits) + nbpcolor);
    gshtctx->ntris = ntris;
    _sht_alloc_vertex_data();
    bitstring bs(bits, nbits); 
    _sht_fill_vertex_data(bs, wbits, hbits);

    Shader shader("./shaders/shader.vert", "./shaders/shader.frag");

    sht_fill_buffers(&gshtctx->vertex_data[0], nfloatsptri * ntris * sizeof(float));

    // art_set_screen_background(0.9f, 0.9f, 0.9f, 1.0f);
    shader.use();

    // Rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw all triangles at once from our vertex array.
    glBindVertexArray(gshtctx->vao);
    glDrawArrays(GL_TRIANGLES, 0, gshtctx->ntris * 3);

    // Extract from GPU and place into img_data

    // TODO: save to img_data and not to file
    //       (this is just because the 'save to memory' is not implemented yet)!
    // <<DBG
    #include <sstream>
    std::stringstream str("");
    str << "screen_" << dbg_count << ".png";
    ++dbg_count;

    // >>DBG
    return art_save_screen_png(gctx, str.str().c_str());
}

// create vbo and vao, fill them with the vertex data. 
// TODO: pass the sht_ctx instead of using the global one
int32_t sht_fill_buffers(float *vertex_data, size_t nvertex_data) {
    // gshtctx->ntris = nvertex_data / (sizeof(float) * (3 * ncoords_per_vertex + 3 * nchannels_per_color));

    // create a vertex array and bind it.
    glGenVertexArrays(1, &gshtctx->vao);
    glBindVertexArray(gshtctx->vao);

    // create a vertex buffer and bind it.
    glGenBuffers(1, &gshtctx->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gshtctx->vbo);

    // stores vertex data for all triangles at once in the vertex buffer.
    glBufferData(GL_ARRAY_BUFFER, nvertex_data, vertex_data, GL_STATIC_DRAW);

    // specify attribute pointers: position and color
    glVertexAttribPointer(0, ncoordspvertex, GL_FLOAT, GL_FALSE, nfloatspvertex * sizeof(float), (void*)0); // vec3 position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, nchannelspcolor, GL_FLOAT, GL_FALSE, nfloatspvertex * sizeof(float), (void*)(3 * sizeof(float))); // vec4 color
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

