#include "Application3D.h"
#include <iostream>

#include "Gizmos.h"
#include "Input.h"
#include "Mesh.h"
#include "ColorPicker.h"
#include "Scene.h"
#include "Instance.h"
#include "RenderTarget.h"

#include <string>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "imgui.h"

// Create namespace 'short cuts'
using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;
using namespace ImGui;

// Application constructor
Application3D::Application3D()
{
}

Application3D::~Application3D()
{
}

bool Application3D::startup()
{
    // Set grey background
	setBackgroundColour(0.25f, 0.25f, 0.25f);

   
    // initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
		getWindowWidth() / (float)getWindowHeight(),
		0.1f, 1000.f);

    // Create new camera and set it to the default (Camera 0)
    m_camera = new Camera(0, glm::vec3(0, 3, -0.6));

    // Create default dynamic light
    Light light;
    light.m_color = { 1, 1,1 };
    light.m_direction = { 1,-1,1 };

    // Init particle Emitter
    m_emitter = new ParticleEmitter();
    m_emitter->initialise(1000, 50,
        0.1f, 1.0f,
        1.0f, 5.0f,
        0.3f, 0.1f,
        glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1));

    m_iter = 0;
  
    // Load Shaders and meshes
	return LoadShaperAndMeshLogic(light);
}

void Application3D::shutdown()
{
	Gizmos::destroy();
    //delete m_scene;
}

// Main Debug UI for Imgui
void Application3D::DebugUI(float dt)
{
    // Static Variables for particle emitter
#pragma region Emitter Variables


    static float ambientBrightness;
    static float lightColor[3];
    static float start_alpha = 1;
    static float end_alpha = 1;
    static float start_particle_color[3];
    static float end_particle_color[3];
    static float lifespan_min = 10;
    static float lifespan_max = 5;
    static float velocity_min = 1;
    static float velocity_max = 5;
    static float start_size = 0.1;
    static float end_size = 0.01;
    static int ParticleAmount = 1000;
    
#pragma endregion
	// Lighting settings
#pragma region Lighting, Debug , Post Processing
    ImGui::Begin("Settings");
    if (CollapsingHeader("Debug")) {
        if (Button("Enable Gizmos")) { gizmos = true;  }
        if (Button("Disable Gizmos")) { gizmos = false;  }
    }
    if (ImGui::CollapsingHeader("Lighting"))
    {
        Indent();
        ColorPicker lighting("Lighting");
      
        ImGui::Spacing();
        Text("Dynamic Light");
        if (CollapsingHeader("Dynamic Light")) {
            Indent();
            std::string name;
            name = "Direction";
            if (Button("Enable Movement")) {
                move_light = true;
            }
            SameLine();
            if (Button("Disable Movement")) {
                move_light = false;
            }
            SliderFloat(name.c_str(), &m_scene->GetLight().m_direction[0], -20.f, 20.f);


            ImGui::Text("Color Picker"); lighting.Picker(lightColor);
            m_scene->GetLight().m_color = { lightColor[0],lightColor[1],lightColor[2] };
            Unindent();
        }

        ImGui::Text("Point Lights");
        if (CollapsingHeader("Point Light")) {
            int count = 0;

            Indent();
            static float light_intensity = 20.f;
            std::string namea = "Point Light intensity";
            SliderFloat(namea.c_str(), &light_intensity, 0.00, 100.00);

            for (auto i = m_scene->GetPointLights().begin(); i != m_scene->GetPointLights().end(); i++) {
                
                count++;
                std::string name = "Light: " + std::to_string(count);

                if (CollapsingHeader(name.c_str())) {

                    Indent();

                    name += "Pos";
                    SliderFloat3(name.c_str(), &i->m_direction[0], -20.0f, 20.0f);

                    name = "Light: " + std::to_string(count) + " color";
                    ColorPicker point_picker(name.c_str());
                    point_picker.Picker(lightColor);

                   
                    i->SetColor(glm::vec3(lightColor[0],lightColor[1],lightColor[2]), light_intensity);


                    Unindent();

                }
            }
            Unindent();
        }

        Text("Ambient Light");
        if (CollapsingHeader("Ambient")) {
            Indent();
            std::string name;
            name = "color";
            static float amb[3];
            SliderFloat3(name.c_str(), amb, 0, 20);
            m_scene->m_ambientLight = glm::vec3(amb[0], amb[1], amb[2]);
            Unindent();
        }



        Unindent();
    }
    if (CollapsingHeader("Post Processing")) {
        Indent();
        Text("Enable / Disable Post Processing");
        if (Button("Enable Distort")) {
            enable_post = true;
            blur = false;
        }
        SameLine();
        if (Button("Disable Distort")) {
            enable_post = false;
            
        }
        
        if (Button("Enable Blur")) {
            blur = true;
            enable_post = false;
        }
        SameLine();
        if (Button("Disable Blur")) {
            blur = false;
        }
        Unindent();
        
    }
    m_scene->DebugUI(current_camera);
#pragma endregion

#pragma region Emitter
    if (CollapsingHeader("Emitter")) {
        Indent();
        ImGui::Text("EMITTER SETTINGS");
        if (Button("Enable Emitter")) {
            emitter_enable = true;
        }
        SameLine();
        if (Button("Disable Emitter")) {
            emitter_enable = false;
        }

        if (emitter_enable) {
            ImGui::SliderInt("Particle Amount", &ParticleAmount, 0, 25000);
            ImGui::SliderFloat("Lifespan Min", &lifespan_min, 0.1, 5);
            ImGui::SliderFloat("Lifespan Max", &lifespan_max, 0.1, 5);
            ImGui::Spacing();
            ImGui::SliderFloat("Velocity Min", &velocity_min, 1, 5);
            ImGui::SliderFloat("Velocity Max", &velocity_max, 1, 5);
            ImGui::Spacing();
            ImGui::SliderFloat("Start Size", &start_size, 0.00, 2);
            ImGui::SliderFloat("End Size", &end_size, 0.00, 2);


            if (ImGui::CollapsingHeader("Start Color"))
            {
                ImGui::Text("Start Color");
                ImGui::SliderFloat("Alpha", &start_alpha, 0, 1);
                ColorPicker startColor("Start Color");
                startColor.Picker(start_particle_color);
                m_emitter->set_starting_color(glm::vec4(start_particle_color[0], start_particle_color[1], start_particle_color[2], start_alpha));
            }

            if (ImGui::CollapsingHeader("End Color"))
            {
                ColorPicker endColor("End Color");
                ImGui::SliderFloat("Alpha", &end_alpha, 0, 1);
                ImGui::Text("End Color");
                endColor.Picker(end_particle_color);
                m_emitter->set_ending_color(glm::vec4(end_particle_color[0], end_particle_color[1], end_particle_color[2], end_alpha));
            }

            m_emitter->set_lifespan(lifespan_min, lifespan_max);
            m_emitter->set_velocity(velocity_min, velocity_max);
            m_emitter->set_size(start_size, end_size);
            m_emitter->set_amount(ParticleAmount);
        }
        Unindent();
    }
	ImGui::End();
#pragma endregion 
}

// Main update Loop
void Application3D::update(float deltaTime)
{
	// query time since application started
	float time = getTime();
    // Render IMGUI debug
    DebugUI(deltaTime);
   
    // rotate camera
	aie::Input* input = aie::Input::getInstance();

    m_viewMatrix = glm::lookAt(vec3(glm::sin(xPos * time) * 10, 10, glm::cos(xPos * time) * 10),
        vec3(0), vec3(0, 1, 0));

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	// draw a simple grid with gizmos
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			 black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			 black);
	}

    if (move_light) {
        // rotate light around using application time as the delta time
        m_scene->GetLight().m_direction = glm::normalize(glm::vec3(glm::cos(time * 2), glm::sin(time * 2), 0));
    }

	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));
    for(auto* object : m_scene->getList())
    {
        object->DecomposeMatrix();
        rotate(object->m_transform, time, vec3(0,1,0));
    }
    
    m_camera->Update(deltaTime);

    m_emitter->update(deltaTime, m_camera->MakeTransform());
    
    // Check for Function key logic to change cameras
    SwitchCameraLogic();
	

	// quit if we press escape

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

// Main application draw
void Application3D::draw()
{
    if (enable_post || blur) {
        // bind render target
        m_renderTarget.bind();
    }
    // wipe the screen to the background colour
    clearScreen();
    
    // bind transform
    glm::mat4 projectionMatrix = m_camera->GetProjectionMatrix((float)getWindowWidth(), (float)getWindowHeight());
    glm::mat4 viewMatrix = m_camera->GetViewMatrix();
   
    glm::mat4 temp(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );

    // Bind particle system shader
    m_particleShader.bind();
    auto pvm = projectionMatrix * viewMatrix * temp;
    m_particleShader.bindUniform("ProjectionViewModel", pvm);
    if (emitter_enable) {
        m_emitter->draw();
    }
    // Render gizmos for Point Lights
    for (auto i : m_scene->GetPointLights())
    {
        glm::vec4 temp(i.m_color.r, i.m_color.g, i.m_color.b, 1);
        Gizmos::addSphere(i.m_direction, 0.1f, 10, 10, temp);
    }
   
    // Update Projection matrix for screen size changes
    m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
        getWindowWidth() / (float)getWindowHeight(),
        0.1f, 1000.f);

    if (gizmos) {
        // Draw Gizmos
        Gizmos::draw(projectionMatrix * viewMatrix);
    }
    // Draw scenes
    m_scene->Draw();
    
    if (blur) {

        // unbind render target and return to backbuffer
        m_renderTarget.unbind();
        // clear the back buffer
        clearScreen();

        m_postProcessing.bind();
        m_postProcessing.bindUniform("colorTarget", 0);
        m_renderTarget.getTarget(0).bind(0);

        // Draw fullscreen quad
        m_quadFullscreen.Draw();
     
    }
    
    if (enable_post) {

        // unbind render target and return to backbuffer
        m_renderTarget.unbind();
    
        // clear the back buffer
        clearScreen();

        // bind post processor
        m_distShader_POST.bind();
        m_distShader_POST.bindUniform("colorTarget", 0);
        m_renderTarget.getTarget(0).bind(0);

        // Draw fullscreen quad
        m_quadFullscreen.Draw();
 
    }

 
}

void Application3D::SwitchCameraLogic()
{
    m_camera = m_scene->getCamera(); // Set camera to current
}

bool Application3D::LoadShaperAndMeshLogic(Light a_light)
{

#pragma region Shadersss


    // load particle shaders
    m_particleShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/ParticleShader.vert");
    m_particleShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/ParticleShader.frag");
    if (m_particleShader.link() == false) { printf("Shader Erro: %s", m_particleShader.getLastError()); }

 
    // Load Textured shaders
    m_texturedShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/textured.vert");
    m_texturedShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/textured.frag");
    if (m_texturedShader.link() == false) { printf("Shader Error %s", m_texturedShader.getLastError()); }
    if (m_gridTexture.load("./textures/numbered_grid.tga") == false) { printf("Error"); return false; }

    // Load Phong Shader
    m_phongShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/phong.vert");
    m_phongShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/phong.frag");
    if (m_phongShader.link() == false) { printf("Phong Shader failed to load. Error: %s\n", m_phongShader.getLastError()); return false; }

    // Load Normal Map Shader
    m_normalMapShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/normalMap.vert");
    m_normalMapShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/normalMap.frag");
    if (!m_normalMapShader.link()) { printf("Error: %s", m_normalMapShader.getLastError()); return false; }

#pragma endregion

#pragma region Mesh


   


    // ------------ LOAD QUAD ---------------------
    m_quad.mesh.InitialiseQuad();

    m_quad.transform = {
        10,0,0,0,
        0,10,0,0,
        0,0,10,0,
        0,0,0,1
    };
    m_quad.name = "floor";

    // ------------ LOAD SPEAR ------------------------------
    m_spear = new gameObject();
    if (m_spear->mesh.load("./soulspear/soulspear.obj", true, true) == false) { return false; }
    m_spear->name = "Soul Spear";
    m_spear->transform = {
       1,     0,     0,  0,
          0,  1,     0,  0,
          0,     0,  1,  0,
          0,     0,     0,  1
    };
    m_spear->position = { 0,0,0 };
    m_spear->scale = { 1,1,1 };

    m_objects.push_back(m_spear);
  
    // ------------ LOAD DRAGON -----------------------------
    m_dragon = new gameObject();
    if (m_dragon->mesh.load("./stanford/Dragon.obj") == false) { return false; }
    m_dragon->name = "Dragon";
    m_dragon->transform = {
          1,0,0,0,
          0,1,0,0,
          0,0,1,0,
          0,0,0,1
    };
    m_dragon->scale = { 0.5,0.5,0.5 };
    m_dragon->position = { -10,1,1 };
 
    // ------------ LOAD GUN ---------------------------

    m_futureGun = new gameObject();
    if (m_futureGun->mesh.load("./M1_carbine/M1_carbine.obj", true, true) == false) { printf("Failure loading gun"); return false; }
    m_futureGun->name = "Gun";
    m_futureGun->transform = {
          1,0,0,0,
          0,1,0,0,
          0,0,1,0,
          0,0,0,1
    };
    m_futureGun->scale = { 0.2,0.2,0.2};
    m_futureGun->position = { -5,2,5 };
    // ------------------------------------------------

    m_scene = new Scene(m_camera, glm::vec2(getWindowWidth(), getWindowHeight()), a_light, glm::vec3(0.03f));
    
    // Stationary Cameras
    auto* stationary_front = new Camera(1, glm::vec3(0.0f, 2.0f, 15.0f));
    stationary_front->SetStationary(true);
    stationary_front->SetRotation(glm::vec2(-90, 0));
	m_scene->AddCamera(stationary_front);
    
    auto* stationary_right = new Camera(2, glm::vec3(0.0f, 2.0f, -20.0f));
    stationary_right->SetStationary(true);
    stationary_right->SetRotation(glm::vec2(90, 0));
    m_scene->AddCamera(stationary_right);

    auto* stationary_top = new Camera(3, glm::vec3(0.0f, 30.0f, 0.0f));
    stationary_top->SetStationary((true));
    stationary_top->SetRotation(glm::vec2(0, -90));
    m_scene->AddCamera(stationary_top);

    // init render target for post processing
    if (m_renderTarget.initialise(1, getWindowWidth(), getWindowHeight()) == false) {
        printf("Render Target Error\n");
        return false;
    }

    m_quadFullscreen.FullScreenQuad();

    // load post procssing shaders
    m_postProcessing.loadShader(aie::eShaderStage::VERTEX, "./shaders/post.vert");
    m_postProcessing.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/post.frag");
    if (m_postProcessing.link() == false) { printf("Shader Error: %s", m_postProcessing.getLastError()); }

    // load post procssing shaders
    m_distShader_POST.loadShader(aie::eShaderStage::VERTEX, "./shaders/distort.vert");
    m_distShader_POST.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/distort.frag");
    if (m_distShader_POST.link() == false) { printf("Shader Error: %s", m_distShader_POST.getLastError()); }



    m_scene->AddCamera(m_camera);

    // Add gameObjects to new instances
    m_scene->AddInstances(new Instance(m_dragon->position, glm::vec3(0, 0, 0), m_dragon->scale, &m_dragon->mesh, &m_phongShader, "Dragon"));
    m_scene->AddInstances(new Instance(m_futureGun->position, glm::vec3(0, 0, 0), m_futureGun->scale, &m_futureGun->mesh, &m_normalMapShader, "Gun"));
    int count = 0;
    for (int i = 0; i < 10; i++)
    {
        count++;
        if (i % 2 == 0)
        {
            std::string temp = "Soul Spear: " + std::to_string(count);
            m_scene->AddInstances(new Instance(glm::vec3(i * 2, 0, 0), glm::vec3(0, 90, 0),
                glm::vec3(1), &m_spear->mesh, &m_normalMapShader,temp));
        }
        else
        {
            std::string temp = "Soul Spear: " + std::to_string(count);

            m_scene->AddInstances(new Instance(glm::vec3(i * 2, 0, 0), glm::vec3(0, 0, 0),
                glm::vec3(1), &m_spear->mesh, &m_normalMapShader, temp));
        }
    }

#pragma endregion

    m_scene->GetPointLights().push_back(Light(vec3(5, 3, 5), vec3(1, 0, 0), 100)); // red
    m_scene->GetPointLights().push_back(Light(vec3(-5, 3, -5), vec3(0, 1, 0), 50)); // green

    m_scene->GetPointLights().push_back(Light(vec3(0, 5, 0), vec3(1, 1, 1), 30)); // dynamic
    LastCamera();
    return true; // All loads correctly
}

void Application3D::LastCamera()
{
    std::string line;
    std::fstream load("../bin/state.txt");

    // Check in file for last camera state (which camera was rendering)
    if (load.is_open()) {
        while (std::getline(load, line)) {
            if (line == "lastcamera=0") {
                m_scene->ChangeCamera(0);
            }
            if (line == "lastcamera=1") {
                m_scene->ChangeCamera(1);
            }
            if (line == "lastcamera=2") {
                m_scene->ChangeCamera(2);
            }
            if (line == "lastcamera=3") {
                m_scene->ChangeCamera(3);
            }
        }
    }
    
}

