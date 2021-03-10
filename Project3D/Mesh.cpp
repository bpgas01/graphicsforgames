#include <gl_core_4_4.h>
#include "Mesh.h"

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

}

void Mesh::InitialiseQuad()
{
	// check that the mesh has not been inited already

	assert(vao == 0);

	// Generate the buffers
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	// Bind the vertex array; a mesh wrapper
	glBindVertexArray(vao);

	// Bind the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Define the 6 vertices for the triangles that make the quad
	Vertex verticies[6];
	verticies[0].position = { -0.5f, 0.f, 0.5f , 1.f};
	verticies[1].position = { 0.5f, 0.f, 0.5f , 1.f };
	verticies[2].position = { -0.5f, 0.f, -0.5f , 1.f };

	verticies[3].position = { -0.5f, 0.f, -0.5f , 1.f };
	verticies[4].position = {  0.5f, 0.f,  0.5f ,  1.f };
	verticies[5].position = {  0.5f, 0.f, -0.5f ,  1.f };

	// Fill the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), verticies, GL_STATIC_DRAW);
	

	// Enable the first element as a position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	// Unbind the buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Quad has two triangles
	triCount = 2;



}

void Mesh::Initialise(unsigned int a_vertexCount, const Vertex* a_vertices, unsigned int a_indexCount, unsigned int* a_indices)
{
    // Check that mesh has not been initialised already
    assert(vao == 0);



    // Generate the buffers 
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);



    // Binds the vertex array; a mesh wrapper
    glBindVertexArray(vao);



    // Bind the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);



    // Fill the vertex buffer
    glBufferData(GL_ARRAY_BUFFER, a_vertexCount * sizeof(Vertex), a_vertices, GL_STATIC_DRAW);

    // Enable the first element as the position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);



    // Bind the indices if there are any
    if (a_indexCount != 0)
    {
        glGenBuffers(1, &ibo);



        // Bind the Vertex Buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);



        // Fill the Vertex Buffer
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_indexCount * sizeof(unsigned int), a_indices, GL_STATIC_DRAW);



        triCount = a_indexCount / 3;
    }
    else
    {
        triCount = a_vertexCount / 3;
    }



    // Unbind the buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void Mesh::Draw()
{

	glBindVertexArray(vao);
	// Check if we are using indicies or just verticies
	if (ibo != 0)
		glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, 3 * triCount);



}
