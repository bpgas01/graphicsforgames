#include "Application3D.h"

#include <iostream>

#include "Gizmos.h"
#include "Input.h"
#include "Mesh.h"

#include "Scene.h"
#include "Instance.h"


#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "imgui.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Application3D::Application3D()
{
}

Application3D::~Application3D()
{
}

bool ColorPicker(const char* label, float col[3])
{
    static const float HUE_PICKER_WIDTH = 20.0f;
    static const float CROSSHAIR_SIZE = 7.0f;
    static const ImVec2 SV_PICKER_SIZE = ImVec2(200, 200);

    ImColor color(col[0], col[1], col[2]);
    bool value_changed = false;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 picker_pos = ImGui::GetCursorScreenPos();

    ImColor colors[] = { ImColor(255, 0, 0),
        ImColor(255, 255, 0),
        ImColor(0, 255, 0),
        ImColor(0, 255, 255),
        ImColor(0, 0, 255),
        ImColor(255, 0, 255),
        ImColor(255, 0, 0) };

    for (int i = 0; i < 6; ++i)
    {
        draw_list->AddRectFilledMultiColor(
            ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 10, picker_pos.y + i * (SV_PICKER_SIZE.y / 6)),
            ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 10 + HUE_PICKER_WIDTH,
                picker_pos.y + (i + 1) * (SV_PICKER_SIZE.y / 6)),
            colors[i],
            colors[i],
            colors[i + 1],
            colors[i + 1]);
    }

    float hue, saturation, value;
    ImGui::ColorConvertRGBtoHSV(
        color.Value.x, color.Value.y, color.Value.z, hue, saturation, value);

    draw_list->AddLine(
        ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 8, picker_pos.y + hue * SV_PICKER_SIZE.y),
        ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 12 + HUE_PICKER_WIDTH, picker_pos.y + hue * SV_PICKER_SIZE.y),
        ImColor(255, 255, 255));

    {
        const int step = 5;
        ImVec2 pos = ImVec2(0, 0);

        ImVec4 c00(1, 1, 1, 1);
        ImVec4 c10(1, 1, 1, 1);
        ImVec4 c01(1, 1, 1, 1);
        ImVec4 c11(1, 1, 1, 1);
        for (int y = 0; y < step; y++)
        {
            for (int x = 0; x < step; x++)
            {
                float s0 = (float)x / (float)step;
                float s1 = (float)(x + 1) / (float)step;
                float v0 = 1.0 - (float)(y) / (float)step;
                float v1 = 1.0 - (float)(y + 1) / (float)step;

                ImGui::ColorConvertHSVtoRGB(hue, s0, v0, c00.x, c00.y, c00.z);
                ImGui::ColorConvertHSVtoRGB(hue, s1, v0, c10.x, c10.y, c10.z);
                ImGui::ColorConvertHSVtoRGB(hue, s0, v1, c01.x, c01.y, c01.z);
                ImGui::ColorConvertHSVtoRGB(hue, s1, v1, c11.x, c11.y, c11.z);

                draw_list->AddRectFilledMultiColor(
                    ImVec2(picker_pos.x + pos.x, picker_pos.y + pos.y),
                    ImVec2(picker_pos.x + pos.x + SV_PICKER_SIZE.x / step, picker_pos.y + pos.y + SV_PICKER_SIZE.y / step),
                    ImGui::ColorConvertFloat4ToU32(c00),
                    ImGui::ColorConvertFloat4ToU32(c10),
                    ImGui::ColorConvertFloat4ToU32(c11),
                    ImGui::ColorConvertFloat4ToU32(c01));

                pos.x += SV_PICKER_SIZE.x / step;
            }
            pos.x = 0;
            pos.y += SV_PICKER_SIZE.y / step;
        }
    }

    float x = saturation * SV_PICKER_SIZE.x;
    float y = (1 - value) * SV_PICKER_SIZE.y;
    ImVec2 p(picker_pos.x + x, picker_pos.y + y);
    draw_list->AddLine(ImVec2(p.x - CROSSHAIR_SIZE, p.y), ImVec2(p.x - 2, p.y), ImColor(255, 255, 255));
    draw_list->AddLine(ImVec2(p.x + CROSSHAIR_SIZE, p.y), ImVec2(p.x + 2, p.y), ImColor(255, 255, 255));
    draw_list->AddLine(ImVec2(p.x, p.y + CROSSHAIR_SIZE), ImVec2(p.x, p.y + 2), ImColor(255, 255, 255));
    draw_list->AddLine(ImVec2(p.x, p.y - CROSSHAIR_SIZE), ImVec2(p.x, p.y - 2), ImColor(255, 255, 255));

    ImGui::InvisibleButton("saturation_value_selector", SV_PICKER_SIZE);

    if (ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0])
    {
        ImVec2 mouse_pos_in_canvas = ImVec2(
            ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

        /**/ if (mouse_pos_in_canvas.x < 0) mouse_pos_in_canvas.x = 0;
        else if (mouse_pos_in_canvas.x >= SV_PICKER_SIZE.x - 1) mouse_pos_in_canvas.x = SV_PICKER_SIZE.x - 1;

        /**/ if (mouse_pos_in_canvas.y < 0) mouse_pos_in_canvas.y = 0;
        else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

        value = 1 - (mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1));
        saturation = mouse_pos_in_canvas.x / (SV_PICKER_SIZE.x - 1);
        value_changed = true;
    }

    ImGui::SetCursorScreenPos(ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 10, picker_pos.y));
    ImGui::InvisibleButton("hue_selector", ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));

    if ((ImGui::IsItemHovered() || ImGui::IsItemActive()) && ImGui::GetIO().MouseDown[0])
    {
        ImVec2 mouse_pos_in_canvas = ImVec2(
            ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

        /* Previous horizontal bar will represent hue=1 (bottom) as hue=0 (top). Since both colors are red, we clamp at (-2, above edge) to avoid visual continuities */
        /**/ if (mouse_pos_in_canvas.y < 0) mouse_pos_in_canvas.y = 0;
        else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 2) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 2;

        hue = mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1);
        value_changed = true;
    }

    color = ImColor::HSV(hue > 0 ? hue : 1e-6, saturation > 0 ? saturation : 1e-6, value > 0 ? value : 1e-6);
    col[0] = color.Value.x;
    col[1] = color.Value.y;
    col[2] = color.Value.z;
    return value_changed | ImGui::ColorEdit3(label, col);
}

bool Application3D::startup()
{
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

    // To do
    // 1. create particle system

    Light light;
    light.m_color = { 1, 1,1 };
    light.m_direction = { 1,-1,1 };
   

    m_iter = 0;
	return LoadShaperAndMeshLogic(light);
}

void Application3D::shutdown()
{
	Gizmos::destroy();
    //delete m_scene;
}


void HorizontalSpace(int amount)
{
    for (int x = 0; x < amount; x++)
    {
        ImGui::Spacing();
        ImGui::SameLine();
    }
}


glm::mat4 RotateY(float degree)
{
    float rads = glm::radians(degree);

    return glm::mat4(
        1, 0, 0, 0,
        0, cos(rads), -sin(rads), 0,
        0, sin(rads), cos(rads), 0,
        0, 0, 0, 0
        );

}



void Application3D::DebugUI(float dt)
{
    // Static Variables
  
    static float ambientBrightness;
    static float lightColor[3];
   
    ImGui::Begin("Inspector View");
    ImGui::SetNextWindowPos({ 0,0 });  
    if (ImGui::CollapsingHeader("LIGHTING"))
    {
        ImGui::Text("Color Picker"); ColorPicker(" ", lightColor);
        ImGui::Text("Light Direction"); ImGui::SliderFloat3(" ", &m_scene->GetLight().m_direction[0], -1, 1.f, "");
        m_scene->GetLight().m_color = { lightColor[0],lightColor[1],lightColor[2] };
       
    }    
    ImGui::Spacing();   
    if (ImGui::CollapsingHeader("OBJECTS"))
    {  	
        ImGui::Text("Select Object");
       
        for (int x = 0; x < m_objects.size(); x++)
        {
            if (ImGui::CollapsingHeader(m_objects[x]->name.c_str()))
            {
                ImGui::BeginChild("a");
                ImGui::DragFloat3("Position", &m_objects[x]->position[0], 0.01, -10, 10);
                ImGui::DragFloat3("Scale", &m_objects[x]->scale[0], 0.01, 0, 10);
                // ImGui::DragFloat3("Rotation", &m_objects[x]->rotation[0], 1, -1000, 1000);
                ImGui::EndChild();
            }
            m_objects[x]->transform = glm::translate(glm::mat4(1.0f), m_objects[x]->position);
            m_objects[x]->transform = glm::scale(m_objects[x]->transform, m_objects[x]->scale);

        }
    }
    ImGui::End();
}



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
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}

    // rotate light around using application time as the delta time
    m_scene->GetLight().m_direction = glm::normalize(glm::vec3(glm::cos(time * 2), glm::sin(time * 2), 0));
	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));
    for(auto* object : m_scene->getList())
    {
        object->DecomposeMatrix();
        rotate(object->m_transform, time, vec3(0,1,0));
    }
    
    m_camera->Update(deltaTime);
    // Check for Function key logic to change cameras
    SwitchCameraLogic();
	

	// quit if we press escape

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application3D::draw()
{
	// wipe the screen to the background colour
	clearScreen();

    glm::mat4 projectionMatrix = m_camera->GetProjectionMatrix((float)getWindowWidth(), (float)getWindowHeight());
    glm::mat4 viewMatrix = m_camera->GetViewMatrix();

    for (auto i : m_scene->GetPointLights()) 
    {
        glm::vec4 temp(i.m_color.r, i.m_color.g, i.m_color.b, 1);
        Gizmos::addSphere(i.m_direction, 0.1f, 10, 10, temp);
    }
	for (auto i : m_scene->GetCameras())
	{
		
		Gizmos::addSphere(i->GetPosition(), 0.03f, 10,10, glm::vec4(1,1,1,1));
		
	}

    // Add particle Effects here
    // bind particle shaders to temp matrix and draw

    Light sunLight = m_scene->GetLight();
    glm::vec4 sunColor(sunLight.m_color.r, sunLight.m_color.g, sunLight.m_color.b, 1);
    Gizmos::addLine({ 0,0,0 }, sunLight.m_direction * 4.0f, sunColor);
    
	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
		getWindowWidth() / (float)getWindowHeight(),
		0.1f, 1000.f);

	//DrawShaderAndMeshes(projectionMatrix, viewMatrix);
    m_scene->Draw();
	// draw 3D gizmos
	Gizmos::draw(projectionMatrix * viewMatrix);

	
}

void Application3D::SwitchCameraLogic()
{
    aie::Input* input = aie::Input::getInstance();

    if (input->isKeyDown(aie::INPUT_KEY_F1))
    {
        m_scene->ChangeCamera(0); // default moving camera
    }
    else if (input->isKeyDown(aie::INPUT_KEY_F2))
    {
        m_scene->ChangeCamera(1); // Stationary front
    }
    else if (input->isKeyDown(aie::INPUT_KEY_F3))
    {
        m_scene->ChangeCamera(2); // Stationary top
    }
    else if (input->isKeyDown(aie::INPUT_KEY_F4))
    {
        m_scene->ChangeCamera(3); // stationary side
    }
    else if (input->isKeyDown(aie::INPUT_KEY_F5))
    {
        m_scene->ChangeCamera(4); // othographic diagonal
    }

    m_camera = m_scene->getCamera(); // Set camera to current


}

bool Application3D::LoadShaperAndMeshLogic(Light a_light)
{

#pragma region Shadersss


    // load particle shaders


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
    m_dragon->position = { -1,1,1 };
 
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

    m_scene->AddCamera(m_camera);

    // Add gameObjects to new instances
    m_scene->AddInstances(new Instance(m_dragon->position, glm::vec3(0, 0, 0), m_dragon->scale, &m_dragon->mesh, &m_phongShader));
    m_scene->AddInstances(new Instance(m_futureGun->position, glm::vec3(0, 0, 0), m_futureGun->scale, &m_futureGun->mesh, &m_normalMapShader));

    for (int i = 0; i < 10; i++)
    {
        if (i % 2 == 0)
        {
            m_scene->AddInstances(new Instance(glm::vec3(i * 2, 0, 0), glm::vec3(0, 90, 0),
                glm::vec3(1), &m_spear->mesh, &m_normalMapShader));
        }
        else
        {
            m_scene->AddInstances(new Instance(glm::vec3(i * 2, 0, 0), glm::vec3(0, 0, 0),
                glm::vec3(1), &m_spear->mesh, &m_normalMapShader));
        }
    }

#pragma endregion

    m_scene->GetPointLights().push_back(Light(vec3(5, 3, 5), vec3(1, 0, 0), 100)); // red
    m_scene->GetPointLights().push_back(Light(vec3(-5, 3, -5), vec3(0, 1, 0), 50)); // green

    m_scene->GetPointLights().push_back(Light(vec3(0, 5, 0), vec3(1, 1, 1), 30)); // dynamic

    return true; // All loads correctly
}

