

#ifndef __LOAD_TEX__H
#define __LOAD_TEX__H

#include <stdarg.h> // used by log functions to have variable number of args
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include "stb_image.h" 

// this function loads a .jpg or .png file into a previously activated texture map
bool load_texture (const char* file_name, GLuint* tex);


#endif