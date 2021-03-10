#pragma once

#include "Application.h"
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

protected:
	glm::vec3 position;
	glm::vec3 pos;
	
	void SolarSystem(float dt);

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	glm::mat4	m_planetTransform;
	
};