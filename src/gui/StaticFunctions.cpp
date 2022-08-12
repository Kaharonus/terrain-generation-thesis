//
// Created by tomas on 05.04.22.
//
#include <StaticFunctions.h>
#include <cxxabi.h>
#include <memory>
#include <png.h>
#include <stdexcept>

float lerp(float a, float b, float f) {
    return a + f * (b - a);
}

std::string demangle(const char* name) {

    int status = -4; // some arbitrary value to eliminate the compiler warning

    std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name, NULL, NULL, &status),std::free};

    return (status == 0) ? res.get() : name;
}

//code edited by me, originally from https://gist.github.com/niw/5963798
std::vector<unsigned char> readPng(const std::string &path) {
    FILE *fp = fopen(path.data(), "rb");
    if (fp == nullptr) {
        return {};
    }
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);
    png_init_io(png, fp);
    png_read_info(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);
    if (bit_depth == 16)
        png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    if (bit_depth == 16)
        png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);
    png_read_update_info(png, info);
    png_bytep *row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * png_get_image_height(png, info));
    for (int y = 0; y < png_get_image_height(png, info); y++) {
        row_pointers[y] = (png_byte *) malloc(png_get_rowbytes(png, info));
    }
    png_read_image(png, row_pointers);
    std::vector<unsigned char> data;
    for (int y = 0; y < png_get_image_height(png, info); y++) {
        data.insert(data.end(), row_pointers[y], row_pointers[y] + png_get_rowbytes(png, info));
    }
    for (int y = 0; y < png_get_image_height(png, info); y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);
    png_destroy_read_struct(&png, &info, nullptr);
    fclose(fp);
    return data;
}


std::string parseSeverity(GLenum severity) {
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            return "high";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "medium";
        case GL_DEBUG_SEVERITY_LOW:
            return "low";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "notification";
        default:
            return "unknown";
    }
}

std::string parseType(GLenum type) {
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            return "error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "deprecated behavior";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "undefined behavior";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "portability";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "performance";
        case GL_DEBUG_TYPE_MARKER:
            return "marker";
        case GL_DEBUG_TYPE_PUSH_GROUP:
            return "push group";
        case GL_DEBUG_TYPE_POP_GROUP:
            return "pop group";
        case GL_DEBUG_TYPE_OTHER:
            return "other";
        default:
            return "unknown";
    }
}

void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam){
    auto severityString = parseSeverity(severity);
    fprintf(stderr, "GL CALLBACK: type = %s, severity = %s, message = %s\n", parseType(type).data(), parseSeverity(severity).data(), message);

}