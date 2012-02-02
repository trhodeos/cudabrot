#include "cudabrot.h"

#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

#include <iostream>

extern void launch_cudabrot_kernel(unsigned char* a, unsigned int width, unsigned int height);

Cudabrot::Cudabrot(unsigned int image_width, unsigned int image_height) :
    image_width_(image_width),
    image_height_(image_height),
    texture_id_(0),
    pbo_(0)
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
    launch_cudabrot_kernel(dptr, image_width_, image_height_);
    /*    int i = image_width_ / 2;
          int j = image_height_ / 2;
          for (i = 0; i < image_width_; i++) {
          for(j = 0; j < image_height_; j++) {
          if (h_ptr[4*(image_height_ * i + j) + 0] &&
          h_ptr[4*(image_height_ * i + j) + 1] &&
          h_ptr[4*(image_height_ * i + j) + 2] &&
          h_ptr[4*(image_height_ * i + j) + 3]) {
          std::cout << "Value of (" << i << ", " << j << "):\n" <<
          (unsigned int) h_ptr[4*(image_height_ * i + j) + 0] << " " <<
          (unsigned int) h_ptr[4*(image_height_ * i + j) + 1] << " " <<
          (unsigned int) h_ptr[4*(image_height_ * i + j) + 2] << " " <<
          (unsigned int) h_ptr[4*(image_height_ * i + j) + 3] << std::endl;
          }
          }
          }*/
    cudaGLUnmapBufferObject(pbo_);
  }
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
