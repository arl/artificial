/*
#include "artificial.h"
#include "artificial_priv.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define USE_NATIVE_OSMESA 0
#if USE_NATIVE_OSMESA
 #define GLFW_EXPOSE_NATIVE_OSMESA
 #include <GLFW/glfw3native.h>
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static void _error_callback(int error, const char* description);

int art_create_ctx(art_ctx ** ctx, unsigned int screen_width, unsigned screen_height) {
    *ctx = (art_ctx *) malloc(sizeof(art_ctx));
    (*ctx)->screen_w = screen_width;
    (*ctx)->screen_h = screen_height;
    (*ctx)->window = nullptr;

    // initialize and configure glfw
    glfwInit();
    glfwSetErrorCallback(_error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // creation our hidden glfw window
    (*ctx)->window = glfwCreateWindow((*ctx)->screen_w, (*ctx)->screen_h, "artificial", nullptr, nullptr);
    if ((*ctx)->window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EARTFAILURE;
    }
    glfwMakeContextCurrent((*ctx)->window);

    // load all OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return EARTFAILURE;
    }

    return EARTOK;
}

int art_destroy_context(art_ctx * ctx) {
    glfwDestroyWindow(ctx->window);
    glfwTerminate();
    free(ctx);
    ctx = nullptr;
    return EARTOK;
}

int art_set_background(float r, float g , float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    return EARTOK;
}

int art_load_texture(char * filename, unsigned int * texid) {
    glGenTextures(1, texid);
    glBindTexture(GL_TEXTURE_2D, *texid);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    int width, height, nchannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nchannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Error: failed to load texture" << filename << std::endl;
        return EARTFAILURE;
    }
    stbi_image_free(data);
    return EARTOK;
}

int art_save_png(art_ctx * ctx, const char * filename) {
    GLsizei width = ctx->screen_w;
    GLsizei height = ctx->screen_h;
    char *buffer;

#if USE_NATIVE_OSMESA
    glfwGetOSMesaColorBuffer(window, &width, &height, NULL, (void**) &buffer);
#else
    buffer = (char*)calloc (4, width * height);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
#endif

    // Write image Y-flipped because OpenGL
    int ret = stbi_write_png(filename,
                width, height, 4,
                buffer + (width * 4 * (height - 1)),
                -width * 4);

#if USE_NATIVE_OSMESA
        // nothing to do
#else
        free(buffer);
#endif
    if (ret == 0) {
        return EARTIOFAILURE;
    }
    return EARTOK;
}

static void _error_callback(int error, const char* description) {
    std::cerr << "Error " << error << " : " << description << std::endl;
}
*/