#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.hpp"

static void genMipMap();

GLuint LoadBMP(const char *path)
{
    printf("loading texture %s\n", path);

    // Data read from the header of the BMP file
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    unsigned int width, height;

    // Open the file
    FILE *file = fopen(path, "rb");
    if (!file) {
        printf("file not found: %s\n", path);
        exit(-1);
    }

    if (fread(header, 1, 54, file) != 54) {
        fprintf(stderr, "Unexpected EOF\n");
        fclose(file);
        exit(-1);
    }
    if (header[0] != 'B' || header[1] != 'M') {
        fprintf(stderr, "Bad magic\n");
        fclose(file);
        exit(-1);
    }
    if (*(int *)(header + 0x1E) != 0 || *(int *)(header + 0x1C) != 24) {
        fprintf(stderr, "Not a 24-bit BMP\n");
        fclose(file);
        exit(-1);
    }

    dataPos = *(int *)&(header[0x0A]);
    imageSize = *(int *)&(header[0x22]);
    width = *(int *)&(header[0x12]);
    height = *(int *)&(header[0x16]);

    if (imageSize == 0)
        imageSize = width * height * 3;
    if (dataPos == 0)
        dataPos = 54;

    uint8_t *data = new uint8_t[imageSize];
    fread(data, 1, width * height * 3, file);
    fclose(file);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
                 GL_UNSIGNED_BYTE, data);
    genMipMap();

    delete[] data;
    return textureID;
}

GLuint LoadTGA(const char *path)
{
    printf("loading texture %s\n", path);

    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "file not found: %s\n", path);
        exit(-1);
    }

    size_t offset = 4 * sizeof(char) + 4 * sizeof(short int);
    fseek(file, offset, SEEK_SET);

    uint16_t width, height;
    uint8_t bitsPerPixel, imageDescriptor;

    fread(&width, sizeof(short), 1, file);
    fread(&height, sizeof(short), 1, file);
    fread(&bitsPerPixel, sizeof(char), 1, file);
    fread(&imageDescriptor, sizeof(char), 1, file);

    int texFormat;
    if (bitsPerPixel == 24) {
        texFormat = GL_RGB;
    } else if (bitsPerPixel == 32) {
        texFormat = GL_RGBA;
    } else {
        fprintf(stderr, "Only supports 24- or 32-bit TGA, now: %d\n", bitsPerPixel);
        exit(-1);
    }

    int bytesBerPixel = bitsPerPixel / 8;

    char *buf = new char[width * height * bitsPerPixel / 8];
    int pixels = width * height;

    for (int i = 0; i < pixels; i++) {
        // BGRA -> RGBA
        buf[i * 4 + 2] = fgetc(file);
        buf[i * 4 + 1] = fgetc(file);
        buf[i * 4 + 0] = fgetc(file);
        if (texFormat == GL_RGBA) {
            buf[i * 4 + 3] = fgetc(file);
        }
    }

    fclose(file);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, texFormat, width, height, 0, texFormat,
                 GL_UNSIGNED_BYTE, buf);
    genMipMap();

    delete[] buf;
    return textureID;
}

static void genMipMap()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}
