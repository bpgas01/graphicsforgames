#pragma once
#include <list>
#include <vector>
#include  <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include "Camera.h"
#include "imgui.h"
#include <fstream>
#include <iostream>
#include <stdio.h>

class Camera;
class Instance;

struct Light
{
	// Default light constructor
	Light()
	{
		m_direction = glm::vec3(1);
		m_color = glm::vec4(1, 1, 1, 1);
	};

	// Overrided light constructor
	Light(glm::vec3 a_pos, glm::vec3 a_color, float a_intensity)
	{
		m_direction = a_pos;
		m_color = a_color * a_intensity;
	};
	// Set color based on vec3 value and intensity
	void SetColor(glm::vec3 a_color , float intensity) {
		m_color = a_color * intensity;
	}

	glm::vec3 m_direction;
	float intensity;
	glm::vec3 m_color; 
	
};

const int MAX_LIGHTS = 4;
class Scene
{

public:

	// depreciated function to check if file is empty
	bool is_empty(std::ifstream& pFile) {
		return pFile.peek() == std::ifstream::traits_type::eof();
	}


	Scene(Camera* camera, glm::vec2 windowSize, Light& light, glm::vec3 ambientLight);
	~Scene();
	
	// Draw all instances present in scene
	void Draw();

	// Add new camera to scene
	void AddCamera(Camera* a_camera);
	// Change camera based on its pre-assigned ID
	void ChangeCamera(int cam);

	// Add new instance to scene
	void AddInstances(Instance* instance);
	// return the list of instances
	std::list<Instance*> GetInstances() { return m_instances; }


	// Get Current camera
	Camera* getCamera()
	{
		return m_camera;
	}

	// Get window width and height
	glm::vec2 GetWindowSize()
	{
		return m_windowSize;
	}
	// Get Main Dynamic light
	Light& GetLight()
	{
		return m_light;
	}

	// Get ambient light
	glm::vec3 GetAmbientLight()
	{
		return m_ambientLight;
	}


	 // Getters for light lists and instance lists
	int GetNumOfLights() { return (int)m_pointLights.size(); }
	
	// Get list of instances
	std::list<Instance*> getList() { return m_instances; }
	
	// Get point light infomation
	glm::vec3* GetPointLightPos() { return &m_pointLightPositions[0]; }
	glm::vec3* GetPointLightColor() { return &m_pointLightColors[0]; }
	std::vector<Light>& GetPointLights() { return m_pointLights; }
	// Get all cameras
	std::vector<Camera*> GetCameras() { return m_cameraList; }

	// Debug UI for camera control + scene objects
	void DebugUI(std::string camera);
	// Save last used camera to file
	void SaveConfig(std::string option, std::string OLD);
	// Ambient light color
	glm::vec3 m_ambientLight;
protected:
	
	Camera* m_camera;
	std::vector<Camera*> m_cameraList;
	glm::vec2 m_windowSize;
	Light m_light;

	std::vector<Light> m_pointLights;
	Light sunlight;
	
	glm::vec3 m_pointLightPositions[MAX_LIGHTS];
	glm::vec3 m_pointLightColors[MAX_LIGHTS];

	std::list<Instance*> m_instances;
	
};

