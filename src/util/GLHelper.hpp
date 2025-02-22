
#ifndef GLHELPER_H
#define GLHELPER_H

#include <iostream>
#include <glad/gl.h>

// This macro function shall encompass any OpenGL function called in order to track errors.
#define glCall(x) GL_clearError(); \
	x; \
	GL_logCall(#x, __FILE__, __LINE__);

/**
 * @brief Removes any confounding errors from previous calls
 * 
 * glGetError() returns one error at a time off of a queue
 */
static void GL_clearError()
{
	while (glGetError() != GL_NO_ERROR);
}

/**
 * @brief Prints any errors found from a previous call to an OpenGL function
 * 
 * @param function The function called
 * @param file The file of the called function
 * @param line The line of the called function within the file
 */
static void GL_logCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError()) {
		std::cerr << "\n[OpenGL error] " << error << "\n" << file << "\n" << function << "\n" << line << "\n";
		exit(-1);
	}
}

#endif