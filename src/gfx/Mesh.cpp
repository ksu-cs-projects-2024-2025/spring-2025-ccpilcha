#include "Mesh.hpp"

#include <iostream>
#include "GLHelper.hpp"
#include <glad/gl.h>

/**
 * @brief Loads the mesh in the pipeline.
 * @warning Must be ran from the main thread!
 */
void GFX::Mesh::Load()
{
    shader->use();
	vbo.bind(GL_ARRAY_BUFFER);
	vao.bind();
}

/**
 * @brief Renders the mesh. Must be loaded prior
 * @warning Must be ran from the main thread!
 */
void GFX::Mesh::Render()
{
    if (!loaded)
    {
        std::cerr << "Mesh not loaded!" << std::endl;
        return;
    }
    glCall(glDrawArrays(GL_TRIANGLES, 0, 3));
}