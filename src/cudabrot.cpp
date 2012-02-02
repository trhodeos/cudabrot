#include "cudabrot.h"

#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

#include <iostream>

extern void launch_cudabrot_kernel(unsigned char* a, unsigned int width, unsigned int height,
                                   float cx, float cy, float scale);

Cudabrot::Cudabrot(unsigned int image_width, unsigned int image_height) :
    image_width_(image_width),
    image_height_(image_height),
    texture_id_(0),
    pbo_(0),
    cx_(0),
    cy_(0),
    scale_(.02)
{
  cudaGLSetGLDevice(0);

  CreatePBO();
  CreateTexture();

  Run();
}

Cudabrot::~Cudabrot() {
  DeletePBO();
  DeleteTexture();
}

void Cudabrot::Run() {
  unsigned char* dptr = NULL;
  if (pbo_ > 0) {
    cudaGLMapBufferObject((void**)&dptr, pbo_);
    cudaMemset(dptr, 0, image_width_ * image_height_ * 4 * sizeof(unsigned char));
    launch_cudabrot_kernel(dptr, image_width_, image_height_, cx_, cy_, scale_);
    cudaGLUnmapBufferObject(pbo_);
  }
}

void Cudabrot::Zoom(unsigned int new_cx, unsigned int new_cy) {
  cx_ = cx_ + scale_ * (new_cx - image_width_ / 2.0f);
  cy_ = cy_ + scale_ * (new_cy - image_height_ / 2.0f);

  scale_ = scale_ / 2.0f;
}

void Cudabrot::UnZoom(unsigned int new_cx, unsigned int new_cy) {
  cx_ = cx_ + scale_ * (new_cx - image_width_ / 2.0f);
  cy_ = cy_ + scale_ * (new_cy - image_height_ / 2.0f);

  scale_ = scale_ * 2.0f;
}

void Cudabrot::Draw() {
  if (pbo_ > 0 && texture_id_ > 0) {
    // bind our texture and pbo buffers
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width_, image_height_,
                   GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glutSwapBuffers();
    glutPostRedisplay();

  }
}

void Cudabrot::CreateTexture() {
  if (texture_id_ == 0) {
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &texture_id_);

    // Make this the current texture (remember that GL is state-based)
    glBindTexture(GL_TEXTURE_2D, texture_id_);

    // Allocate the texture memory. The last parameter is NULL since we only
    // want to allocate memory, not initialize it
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width_, image_height_, 0,
                 GL_RGBA,GL_UNSIGNED_BYTE, NULL);
    // Must set the filter mode, GL_LINEAR enables interpolation when scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

void Cudabrot::DeleteTexture() {
  if (texture_id_ > 0) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture_id_);
    texture_id_ = 0;
  }
}

void Cudabrot::CreatePBO() {
  if (pbo_ == 0) {
    int num_texels = image_width_ * image_height_;
    int num_values = num_texels * 4;

    // generate a buffer
    glGenBuffers(1, &pbo_);
    // bind that buffer
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
    // allocate data for the buffer.. 4-channel 8-bit image
    glBufferData(GL_PIXEL_UNPACK_BUFFER, num_values * sizeof(GLubyte),
                 NULL, GL_DYNAMIC_DRAW);

    // register buffer with cuda
    cudaGLRegisterBufferObject(pbo_);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

void Cudabrot::DeletePBO() {
  if (pbo_ > 0) {
    cudaGLUnregisterBufferObject(pbo_);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &pbo_);

    pbo_ = 0;
  }
}
