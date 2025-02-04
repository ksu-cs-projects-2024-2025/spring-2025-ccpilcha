#pragma once

#include "GLHelper.hpp"
#include <glad/gl.h>

class VBO
{
	unsigned int vboID;
public:
	VBO() {
		glCall(glGenBuffers(1, &(this->vboID)));
	}
	void bind(GLenum target)
	{
		glCall(glBindBuffer(target, this->vboID));
	}
	~VBO() {
		glCall(glDeleteBuffers(1, &(this->vboID)));
	}
};
