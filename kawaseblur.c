#include "kawaseblur.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

GLuint textureScene, textureBlur1, textureBlur2;
#define width 640
#define height 480

unsigned char pixels[width * height * 4];
unsigned char blurPixels[width * height * 4];

void initTextures() {
    glGenTextures(1, &textureScene);
    glBindTexture(GL_TEXTURE_2D, textureScene);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &textureBlur1);
    glBindTexture(GL_TEXTURE_2D, textureBlur1);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &textureBlur2);
    glBindTexture(GL_TEXTURE_2D, textureBlur2);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void renderSceneToTexture(GLuint texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
}

void applyKawaseBlur(GLuint sourceTex, GLuint destTex, int offset) {

    glBindTexture(GL_TEXTURE_2D, sourceTex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int count = 0;
            unsigned char r = 0, g = 0, b = 0, a = 0;

            for (int ky = -offset; ky <= offset; ++ky) {
                for (int kx = -offset; kx <= offset; ++kx) {
                    int nx = x + kx;
                    int ny = y + ky;

                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        int index = (ny * width + nx) * 4;
                        r += pixels[index] + 100;
                        g += pixels[index + 1];
                        b += pixels[index + 2];
                        a += pixels[index + 3];
                        ++count;
                    }
                }
            }

            int index = (y * width + x) * 4;
            blurPixels[index] = r / count;
            blurPixels[index + 1] = g / count;
            blurPixels[index + 2] = b / count;
            blurPixels[index + 3] = a / count;
        }
    }

    glBindTexture(GL_TEXTURE_2D, destTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, blurPixels);

}

void displayKawase() {
    /* Performance metrics */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640.0f, 0.0f, 480.0f, 1.0f, -1.0f);

    // Set the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Render the scene to the initial texture
    renderSceneToTexture(textureScene);

    // Apply the first blur pass
    applyKawaseBlur(textureScene, textureBlur1, 1);

    // Apply the second blur pass
    applyKawaseBlur(textureBlur1, textureBlur2, 2);

    // Bind the final texture and render it to the screen
    glBindTexture(GL_TEXTURE_2D, textureBlur2);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f((float)width, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f((float)width, (float)height);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, (float)height);
    glEnd();

    glutSwapBuffers();
}

void initKawase() {
    glEnable(GL_TEXTURE_2D);
    initTextures();
}