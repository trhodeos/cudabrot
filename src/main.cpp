#include "config.h"

// these don't like to be included last
#include "os_gl.h"

#include <iostream>

#include <cuda.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime.h>

#include "cudabrot.h"

void display_callback();
void keyboard_callback(unsigned char key, int x, int y);
void mouse_callback(int button, int state, int x, int y);
void motion_callback(int x, int y);

bool init_gl(int argc, char* argv[]);

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

Cudabrot* cudabrot_;

int main(int argc, char* argv[]) {

  /* initialize OpenGL */
  if (!init_gl(argc, argv)) {
    std::cout << "Couldn't initialize GL!" << std::endl;
    return -1;
  }

  cudabrot_ = new Cudabrot(WINDOW_WIDTH, WINDOW_HEIGHT);

  /* start opengl loop */
  glutMainLoop();

  /* cleanup */
  cudaThreadExit();
  return 0;
}

bool init_gl(int argc, char* argv[]) {
  /* argv up glut parameters */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

  /* name window */
  glutCreateWindow("Cudabrot");

  /* set up callbacks */
  glutDisplayFunc(display_callback);
  glutKeyboardFunc(keyboard_callback);
  glutMouseFunc(mouse_callback);
  glutMotionFunc(motion_callback);

  /* check for opengl extensions */
  glewInit();
  if (!glewIsSupported("GL_VERSION_2_0")) {
    /* no good :\ */
    return false;
  }

  /* set up viewport */
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glDisable(GL_DEPTH_TEST); // no depth test for 2d stuffz

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  /* set up ortho projection matrix for only 2d images */
  glOrtho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);

  /* set up view matrix */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  return true;
}

void display_callback() {
  cudabrot_->Run();
  cudabrot_->Draw();
}

void keyboard_callback(unsigned char key, int x, int y) {}
void mouse_callback(int button, int state, int x, int y) {
  if (state == GLUT_DOWN) {
    if (button == GLUT_LEFT_BUTTON) {
      cudabrot_->Zoom(x, y);
    }
    if (button == GLUT_RIGHT_BUTTON) {
      cudabrot_->UnZoom(x, y);
    }
  }
}
void motion_callback(int x, int y) {}

