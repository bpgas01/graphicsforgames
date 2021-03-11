#pragma once

#include "Application.h"
#include "Mesh.h"
#include "OBJMesh.h"
#include "Shader.h"
#include <glm/mat4x4.hpp>
#include <vector>

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

	glm::vec3				pos;
	glm::mat4				m_viewMatrix;
	glm::mat4				m_projectionMatrix;
	glm::mat4				m_planetTransform;

	glm::vec4				colorthing;
	glm::vec4				bunnyColour;
	
	// ====== SHADER ==============
	aie::ShaderProgram		m_simpleShader;
	aie::ShaderProgram	    m_bunnyShader;
	aie::ShaderProgram		m_phongShader;
	// ============================


	struct gameObject
	{
		std::string name;
		aie::OBJMesh mesh;
		glm::mat4 transform;
	};

	struct quadObject
	{
		std::string name;
		Mesh mesh;
		glm::mat4 transform;
	};

	gameObject m_bunny;
	quadObject m_quad;
	gameObject dragon;
	

	// Light Object
	struct Light
	{
		glm::vec3			direction;
		glm::vec3			color;
	};

	Light					m_light;
	glm::vec3				m_ambientLight;
	glm::vec3				diffuseVec;
	float					xPos = 0;
	float					yPos = 0;
	int						m_iter = 0;
	int						m_max;

	std::vector<gameObject> m_objects;


public:

	bool LoadShaperAndMeshLogic();
	void DrawShaderAndMeshes(glm::mat4, glm::mat4);

private:

	void DebugUI();

};