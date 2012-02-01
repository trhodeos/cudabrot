#ifndef OS_GL_H
#define OS_GL_H

#include "config.h"

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

#endif // OS_GL_H
