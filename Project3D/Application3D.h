#pragma once
// Duncan sykes openGL application
#include "Application.h"
#include "Mesh.h"
#include "OBJMesh.h"
#include "imgui.h"
#include "ColorPicker.h"
#include "RenderTarget.h"
#include "ParticleEmitter.h"
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include <fstream>
#include <iostream>
#include <glm/mat4x4.hpp>
#include <vector>
#include <string>
class Application3D : public aie::Application
{
public:

	Application3D();
	virtual ~Application3D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	// Used to use this to change camera logic
	void SwitchCameraLogic();

	float z = 0.03;
	float b = 2;
	float x = 0.01;
	float val = 2;
	
protected:

	// main camera
	Camera* m_camera;
	
	aie::Texture m_gridTexture;

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
	aie::ShaderProgram		m_texturedShader;
	aie::ShaderProgram		m_normalMapShader;
	
	aie::ShaderProgram		m_particleShader;

	aie::ShaderProgram		m_postProcessing;
	aie::ShaderProgram      m_distShader_POST;
	aie::ShaderProgram	    m_blurShader_POST;
	
	// ============================

	aie::RenderTarget      m_renderTarget;
	Mesh				   m_quadFullscreen;

	float				   m_distortBias;

	// ===== PARTICLES ============
	ParticleEmitter* m_emitter;
	

	// gameobject struct to store all needed object info
	struct gameObject
	{
		std::string			name;
		aie::OBJMesh		mesh;
		glm::mat4			transform;
		glm::vec3			position;
		glm::vec3			scale;
		glm::vec3			rotation;
		
	};

	// basic quad gameobject
	struct quadObject
	{
		std::string name;
		Mesh mesh;
		glm::mat4 transform;
	};

	// Checks if file is empty (no longer used)
	bool is_empty(std::ifstream& pFile) {
		return pFile.peek() == std::ifstream::traits_type::eof();
	}

	// Scene object
	Scene* m_scene;
	
	// Each gameobject
	gameObject*				m_bunny;
	gameObject*				m_dragon;
	gameObject*				m_spear;
	gameObject*				m_spear2;
	gameObject*				m_futureGun; // custom mesh / object
	quadObject				m_quad;
	

	bool					emitter_enable = true; // toggle state of particle emitter
	bool					blur = false; // post processing blur toggle
	bool					gizmos = true; // if gizmos are enabled or not

	bool				    enable_post = false; // enable distortion post processing
	bool					move_light = true; // if dyncamic light is using deltatime to move
	
	glm::vec3				diffuseVec;
	float					xPos = 0;
	float					yPos = 0;
	int						m_iter = 0;
	int						m_max;

	// list of gameobjects
	std::vector<gameObject*> m_objects;
	//
	std::string current_camera = " "; // name of current camera - for saving state

public:
	
	// Load shaders and meshes
	bool LoadShaperAndMeshLogic(Light a_light);

	// Load the last camera that was used
	void LastCamera();
	

private:
	// Custom IMGUI UI
	void DebugUI(float dt);

};