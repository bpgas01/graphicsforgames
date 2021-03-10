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
}

void Mesh::Draw()
{
}
