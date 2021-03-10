#pragma once

#include "Application.h"
#include "Mesh.h"
#include "OBJMesh.h"
#include "Shader.h"
#include <glm/mat4x4.hpp>

class Application3D : public aie::Application
{
public:

	Application3D();
	virtual ~Application3D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	float z = 0.03;
	float b = 2;
	float x = 0.01;
	float val = 2;
	
protected:
	glm::vec3 pos;
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_planetTransform;

	glm::vec4 colorthing;
	glm::vec4 bunnyColour;
	
	// ====== SHADER =======
	aie::ShaderProgram		m_simpleShader;
	aie::ShaderProgram	    m_bunnyShader;
	// ============================

	// Basic Plane
	Mesh					m_quadMesh;
	glm::mat4				m_quadTransform;

	// Bunny Obj
	aie::OBJMesh m_bunnyMesh;
    glm::mat4  m_bunnyTransform;

public:

	bool LoadShaperAndMeshLogic();
	void DrawShaderAndMeshes(glm::mat4, glm::mat4);



};