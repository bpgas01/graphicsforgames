#pragma once
#include <glm/glm.hpp>
class Camera
{

public:

	Camera();
	Camera(int a_camera, glm::vec3 a_position) : m_camera(a_camera), m_position(a_position) 
	{
		m_phi = 0;
		m_theta = 0;
		lastMouseX = 0;
		lastMouseY = 0;
	};
	~Camera() {};

	void Update(float a_deltaTime);
	
	int GetCamera() const { return m_camera; }
	glm::vec3 GetPosition() const { return m_position; }
	bool IsOrthographic() { return m_isOrthographic; }
	
	void SetCamera(int a_camera) { m_camera = a_camera; }
	void SetPosition( const glm::vec3 a_position) { m_position = a_position; }
	void SetOrthographic(bool state) { m_isOrthographic = state; }
	void SetStationary( const bool a_state) { m_isStationary = a_state; }
	void SetRotation(const glm::vec2 rotation)
	{
		m_rotation = rotation;
		m_theta = m_rotation.x;
		m_phi = m_rotation.y;
	}
	glm::mat4 MakeTransform();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix(float width, float height);
	glm::mat4 GetOrthographic(float width, float height);

private:
	bool m_isStationary = false;
	int m_camera = 0;
	float m_theta;
	float m_phi; 
	int zero = 0;
	glm::vec3 m_position;
	glm::vec2 m_rotation;
	
	float lastMouseX, 
		  lastMouseY;

	bool m_isOrthographic = false;
};

