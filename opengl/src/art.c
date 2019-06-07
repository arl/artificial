#include "art.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define USE_NATIVE_OSMESA 0
#if USE_NATIVE_OSMESA
 #define GLFW_EXPOSE_NATIVE_OSMESA
 #include <GLFW/glfw3native.h>
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <string.h>
#include <math.h>
#include <stdio.h>

struct art_ctx {
    unsigned int screen_w;
    unsigned int screen_h;
    GLFWwindow* window;
};

static void _error_callback(int error, const char* description);

int32_t art_create_ctx(struct art_ctx ** ctx, uint32_t screen_width, uint32_t screen_height) {
    *ctx = (struct art_ctx *) malloc(sizeof(struct art_ctx));
    (*ctx)->screen_w = screen_width;
    (*ctx)->screen_h = screen_height;
    (*ctx)->window = NULL;

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
    (*ctx)->window = glfwCreateWindow((*ctx)->screen_w, (*ctx)->screen_h, "artificial", NULL, NULL);
    if ((*ctx)->window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return EARTGEN;
    }
    glfwMakeContextCurrent((*ctx)->window);

    // load all OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return EARTGEN;
    }

    return EARTOK;
}

int32_t art_destroy_context(struct art_ctx * ctx) {
    glfwDestroyWindow(ctx->window);
    glfwTerminate();
    free(ctx);
    ctx = NULL;
    return EARTOK;
}

int art_set_screen_background(float r, float g , float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    return EARTOK;
}

int art_save_screen_png(struct art_ctx * ctx, const char * filename) {
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
        return EARTIO;
    }
    return EARTOK;
}

static void _error_callback(int error, const char* description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

