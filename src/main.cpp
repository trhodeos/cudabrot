#include "config.h"

#include <iostream>

/* gl stuff */
#include <GL/glew.h>
#ifdef APPLE
#include <OpenGL/GL.h>
#include <OpenGL/GLU.h>
#include <GLUT/GLUT.h>
#else // APPLE
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/GLUT.h>
#endif // APPLE

#include <cuda.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime.h>

void display_callback() {}
void keyboard_callback(unsigned char key, int x, int y) {}
void mouse_callback(int button, int state, int x, int y) {}
void motion_callback(int x, int y) {}

bool init_gl(int argc, char* argv[]);

// TEMPORARY GLOBAL VARIABLES
const unsigned int window_width = 640;
const unsigned int window_height = 480;

int main(int argc, char* argv[]) {

  /* initialize OpenGL */
  if (!init_gl(argc, argv)) {
    std::cout << "Couldn't initialize GL!" << std::endl;
    return -1;
  }

  /* init CUDA */

  /* setup device memory */

  /* start opengl loop */
  glutMainLoop();

  /* cleanup */
  cudaThreadExit();
  return 0;
}

bool init_gl(int argc, char* argv[]) {
  /* set up glut parameters */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(window_width, window_height);
  glutCreateWindow("Cudabrot");
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
  glViewport(0, 0, window_width, window_height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glDisable(GL_DEPTH_TEST); // no depth test for 2d stuffz

  /* set up view matrix */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);

  return true;
}
