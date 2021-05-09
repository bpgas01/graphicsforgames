#include "Scene.h"
#include "imgui.h"
#include "Instance.h"
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>

using namespace ImGui; 
Scene::Scene(Camera* camera, glm::vec2 windowSize, Light& light, glm::vec3 ambientLight)
	: m_camera(camera), m_windowSize(windowSize), m_light(light), m_ambientLight(ambientLight)
{
	
}
Scene::~Scene()
{
	for (auto i = m_instances.begin(); i != m_instances.end(); i++)
	{
		delete(*i);
	}
}

// Add new canera to scene
void Scene::AddCamera(Camera* a_camera) { m_cameraList.push_back(a_camera); }

// Change camera based on int value
void Scene::ChangeCamera(int Cam) 
{
	for (auto *i : m_cameraList)
	{
		if (i->GetCamera() == Cam)
		{
			m_camera = i;
		}
	}
}

// add new object instance to scene
void Scene::AddInstances(Instance* instance)
{
	m_instances.push_back(instance);
}

void Scene::Draw()
{

	// Get all lights
	for (int i = 0; i < MAX_LIGHTS && i <m_pointLights.size(); i++)
	{
		m_pointLightPositions[i] = m_pointLights[i].m_direction;
		m_pointLightColors[i] = m_pointLights[i].m_color;
	}

	// Drawing Each Instance
	for (auto i = m_instances.begin(); i != m_instances.end(); i++)
	{
		Instance* instance = *i;
		instance->Draw(this);
	}
}

// Debug UI for objects and cameras
void Scene::DebugUI(std::string camera)
{

    if (CollapsingHeader("Objects")) {
        Indent();
        int count = 0;
        for (auto i : m_instances) {
            count++;
            std::string name = "Object: " + i->name;
            if (CollapsingHeader(name.c_str())) {

                glm::mat4 transform = i->GetTransform();
                glm::vec3 scale;
                glm::quat rotation;
                glm::vec3 translation;
                glm::vec3 skew;
                glm::vec4 perspective;
                glm::decompose(i->GetTransform() , scale, rotation, translation, skew, perspective);
               
                // Position slider
                std::string transform_name = std::to_string(count) + " position";
                SliderFloat3(transform_name.c_str(), &translation[0], -20, 20);
                // Rotation Slider
                transform_name = std::to_string(count) + " rotation";
                SliderFloat3(transform_name.c_str(), &i->m_rotation[0], -90, 90);
                // Scale Slider
                transform_name = std::to_string(count) + " scale";
                SliderFloat3(transform_name.c_str(), &scale[0], 0.1, 5);

                i->m_transform = i->MakeTransform(translation, i->m_rotation, scale);

            }
        }
        Unindent();
    }
    
    if (CollapsingHeader("Cameras")) {
        Indent();
        Text("Use Buttons to switch view");
        if (Button("Roam Camera")) {
            ChangeCamera(0);
            SaveConfig("lastcamera=0", camera); 
        }
        SameLine();
        if (Button("Static Left")) {
            ChangeCamera(1);
            SaveConfig("lastcamera=1", camera);

        }
        SameLine();
        if (Button("Static Right")) {
            ChangeCamera(2);
            SaveConfig("lastcamera=2", camera);

        }
        SameLine();
        if (Button("Static Top")) {
            ChangeCamera(3);
            SaveConfig("lastcamera=3", camera);

        }
        Unindent();
    }
}


// Save last used camera to text file
void Scene::SaveConfig(std::string option, std::string OLD) {


    std::ifstream filein("../bin/state.txt");
    std::ofstream fileout("../bin/state.txt");


    if (is_empty(filein)) {
        fileout << option + "\n";
    }

}