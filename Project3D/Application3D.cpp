#include "Application3D.h"
#include "Gizmos.h"
#include "Input.h"
#include "Mesh.h"
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

    m_light.color = { 1, 1,1 };
    m_ambientLight = { 0.25, 0.25, 0.25 };

    m_iter = 0;
	return LoadShaperAndMeshLogic();
}

void Application3D::shutdown()
{
	Gizmos::destroy();
}

// Render IMGUI Debug windows
void Application3D::DebugUI()
{
    // Static Variables
    static float al;
    static float lightAlpha;
    static float colors[3];
    static float ambientBrightness;
    static float lightColor[3];

   

    
    ImGui::Begin("Inspector View");
    ImGui::SetNextWindowPos({ 0,0 });
    // Lighting Settings
    ImGui::Text("-----------------------");

    ImGui::Text("LIGHTING SETTINGS");
    ImGui::Text("Color Picker"); ColorPicker(" ", lightColor);
    ImGui::Text("Ambient Brightness"); ImGui::SliderFloat(" ", &ambientBrightness, 0, 0.25, "");
    ImGui::Text("Light Direction"); ImGui::SliderFloat3(" ", &m_light.direction[0], -1, 1.f, "");
    m_light.color = { lightColor[0],lightColor[1],lightColor[2] };
    m_ambientLight = { ambientBrightness,ambientBrightness,ambientBrightness };
    // Camera Settings
    ImGui::Spacing();
    ImGui::Text("-----------------------");

    ImGui::Text("CAMERA SETTINGS");
    ImGui::Spacing();

    if (ImGui::Button("Move Left")) xPos = 2;
    ImGui::SameLine();
    if (ImGui::Button("Move Right")) xPos = -2;
    ImGui::SameLine(); 
    if (ImGui::Button("STOP")) xPos = 0;
    
    ImGui::Spacing();
    ImGui::Text("-----------------------");
    ImGui::Text("OBJECT TRANSFROM SETTINGS");
    ImGui::Text("Select Object");
    ImGui::Text(" << Object Name = %s >> ", m_objects[m_iter].name.c_str());
    
    if (ImGui::Button("Prev"))
    {
        m_iter-=1;
        if (m_iter < 0)
            m_iter= 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Next"))
    {
        m_iter+=1;
        if (m_iter >= m_objects.size())
            m_iter -=1;       
    }

    ImGui::End();

}



void Application3D::update(float deltaTime)
{
    m_max = m_objects.size();
	// query time since application started
	float time = getTime();
    // Render IMGUI debug
    DebugUI();
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

	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));


   


	// quit if we press escape

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application3D::draw()
{
	// wipe the screen to the background colour
	clearScreen();

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
		getWindowWidth() / (float)getWindowHeight(),
		0.1f, 1000.f);

	DrawShaderAndMeshes(m_projectionMatrix, m_viewMatrix);

	// draw 3D gizmos
	Gizmos::draw(m_projectionMatrix * m_viewMatrix);

	// draw 2D gizmos using an orthogonal projection matrix (or screen dimensions)
	Gizmos::draw2D((float)getWindowWidth(), (float)getWindowHeight());
}

bool Application3D::LoadShaperAndMeshLogic()
{

    

// Shader loading
#pragma region Quad
    // Load the vertex shader from a file
    m_simpleShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/simple.vert");



    // Load the fragment shader from a file
    m_simpleShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/simple.frag");



    if (!m_simpleShader.link())
    {
        printf("Simple Shader had an error: %s\n", m_simpleShader.getLastError());
        return false;
    }


#pragma endregion
#pragma region Phong Shader

    m_phongShader.loadShader(aie::eShaderStage::VERTEX,
        "./shaders/phong.vert");
    m_phongShader.loadShader(aie::eShaderStage::FRAGMENT,
        "./shaders/phong.frag");

    if (m_phongShader.link() == false)
    {
        printf("Phong Shader failed to load. Error: %s\n", m_phongShader.getLastError());
        return false;
    }
#pragma endregion

#pragma region FlatBunny

    m_bunnyShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/simple.vert");



    // Load the fragment shader from a file
    m_bunnyShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/simple.frag");



    if (!m_bunnyShader.link())
    {
        printf("Bunny Shader had an error: %s\n", m_bunnyShader.getLastError());
        return false;
    }

#pragma endregion


    Mesh::Vertex vertices[4];
    vertices[0].position = { -0.5f,0.f, 0.5f,1.f };
    vertices[1].position = { 0.5f,0.f, 0.5f, 1.f };
    vertices[2].position = { -0.5, 0.f, -0.5f, 1.f };
    vertices[3].position = { 0.5f,0.f, -0.5f, 1.f };

    unsigned int indices[6] = { 0,1,2,2,1,3 };
    //m_quadMesh.InitialiseQuad();
    m_quad.mesh.Initialise(4, vertices, 6, indices);
    // We will make the quad 10 units by 10 units 
    m_quad.transform = {
        10,  0,  0,  0,
         0, 10,  0,  0,
         0,  0, 10,  0,
         0,  0,  0,  1
    };
    m_quad.name = "Simple quad";


    if (m_bunny.mesh.load("./stanford/bunny.obj") == false)
    {
        printf("Bunny Mesh Failed!\n");
        return false;
    }

    m_bunny.name = "Bunny";
    m_bunny.transform = {
        0.5f,     0,     0,  0,
           0,  0.5f,     0,  0,
           0,     0,  0.5f,  0,
           0,     0,     0,  1
    };


  
    if (dragon.mesh.load("./stanford/dragon.obj") == false) { printf("Error loading mesh"); return false; }
    dragon.name = "Dragon";
    dragon.transform = {
        0.5f,     0,     0,  0,
           0,  0.5f,     0,  0,
           0,     0,  0.5f,  0,
           -3,     0,     3,  1
    };

    m_objects.push_back(m_bunny);
    m_objects.push_back(dragon);

    return true;
}

void Application3D::DrawShaderAndMeshes(glm::mat4 a_projectionMatrix, glm::mat4 a_viewMatrix)
{
    auto pvm = a_projectionMatrix * a_viewMatrix * glm::mat4(0); // PVM = Projection view matrix
#pragma region Quad Shader
    //Bind the shader
    m_simpleShader.bind();
    pvm = a_projectionMatrix * a_viewMatrix * m_quad.transform;
    // Bind the transform of the mesh
    m_simpleShader.bindUniform("ProjectionViewModel", pvm);

    m_simpleShader.bindUniform("randcolor", colorthing);
    m_quad.mesh.Draw();
   
#pragma endregion

#pragma region Bunny Shader

    m_bunnyShader.bind();
    pvm = a_projectionMatrix * a_viewMatrix * m_bunny.transform;
    m_bunnyShader.bindUniform("ProjectionViewModel", pvm);
    
    m_bunnyShader.bindUniform("randcolor", bunnyColour);
   // m_bunnyMesh.draw();

#pragma endregion

#pragma region Phong
    // Bind the shader
    m_phongShader.bind();
    
    // Bind the Camera Position
    m_phongShader.bindUniform("CameraPosition", vec3(glm::inverse(a_viewMatrix)[3]));
    // Bind the Light
    m_phongShader.bindUniform("AmbientColor", m_ambientLight);
    m_phongShader.bindUniform("LightColor", m_light.color);
    m_phongShader.bindUniform("LightDirection", m_light.direction);
    

    // Bind the PVM
    pvm = a_projectionMatrix * a_viewMatrix * m_bunny.transform;
    m_phongShader.bindUniform("ProjectionViewModel", pvm);

    // Bind the lighting transforms
    m_phongShader.bindUniform("ModelMatrix", m_bunny.transform);



    m_bunny.mesh.draw();
   
#pragma endregion
    
    // Bind the PVM
    pvm = a_projectionMatrix * a_viewMatrix * dragon.transform;
    m_phongShader.bindUniform("ProjectionViewModel", pvm);

    // Bind the lighting transforms
    m_phongShader.bindUniform("ModelMatrix", dragon.transform);


    dragon.mesh.draw();
    
}


