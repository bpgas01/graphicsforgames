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
	
	return LoadShaperAndMeshLogic();
}

void Application3D::shutdown()
{
	Gizmos::destroy();
}


void Application3D::update(float deltaTime)
{
	// query time since application started
	float time = getTime();


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

	aie::Input* input = aie::Input::getInstance();
    static float al;
    static float al2;
	static float colors[3];
    static float colors2[3];


	ImGui::Begin("PLANE");
	ColorPicker("PLANE", colors);
    ImGui::DragFloat("ALPHA", &al, .1f, 0.f, 100.f);
    ImGui::End();

    ImGui::Begin("BUNNY");
    ColorPicker("BUNNY", colors2);
    ImGui::DragFloat("ALPHA", &al2, .1f, 0.f, 100.f);
    ImGui::End();

    if (al < 0.1f) al = 100;
    if (al2 < 0.1f) al2 = 100;

    bunnyColour = { colors2[0], colors2[1], colors2[2], colors2[3]};
    bunnyColour.a = al2 * 0.01;


	colorthing = { colors[0],colors[1],colors[2],colors[3]};
    colorthing.a = al * 0.01;

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



    //// Define the 6 vertices for the 2 triangles that make the quad
    //Mesh::Vertex vertices[6];
    //vertices[0].position = { -0.5f, 0.0f,  0.5f, 1.0f };
    //vertices[1].position = { 0.5f, 0.0f,  0.5f, 1.0f };
    //vertices[2].position = { -0.5f, 0.0f, -0.5f, 1.0f };



    //vertices[3].position = { -0.5f, 0.0f, -0.5f, 1.0f };
    //vertices[4].position = { 0.5f, 0.0f,  0.5f, 1.0f };
    //vertices[5].position = { 0.5f, 0.0f, -0.5f, 1.0f };

    Mesh::Vertex vertices[4];
    vertices[0].position = { -0.5f,0.f, 0.5f,1.f };
    vertices[1].position = { 0.5f,0.f, 0.5f, 1.f };
    vertices[2].position = { -0.5, 0.f, -0.5f, 1.f };
    vertices[3].position = { 0.5f,0.f, -0.5f, 1.f };

    unsigned int indices[6] = { 0,1,2,2,1,3 };





    //m_quadMesh.InitialiseQuad();
    m_quadMesh.Initialise(4, vertices, 6, indices);



    // We will make the quad 10 units by 10 units 
    m_quadTransform = {
        10,  0,  0,  0,
         0, 10,  0,  0,
         0,  0, 10,  0,
         0,  0,  0,  1
    };



  //  m_quadTransform = glm::rotate(m_quadTransform, glm::radians(90.0f), glm::vec3(1, 0, 0));



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



    if (m_bunnyMesh.load("./stanford/bunny.obj") == false)
    {
        printf("Bunny Mesh Failed!\n");
        return false;
    }



    m_bunnyTransform = {
        0.5f,     0,     0,  0,
           0,  0.5f,     0,  0,
           0,     0,  0.5f,  0,
           0,     0,     0,  1
    };



#pragma endregion

    return true;
}

void Application3D::DrawShaderAndMeshes(glm::mat4 a_projectionMatrix, glm::mat4 a_viewMatrix)
{
    auto pvm = a_projectionMatrix * a_viewMatrix * glm::mat4(0); // PVM = Projection view matrix
#pragma region Quad
    //Bind the shader
    m_simpleShader.bind();
    pvm = a_projectionMatrix * a_viewMatrix * m_quadTransform;
    // Bind the transform of the mesh
    m_simpleShader.bindUniform("ProjectionViewModel", pvm);

    m_simpleShader.bindUniform("randcolor", colorthing);

    m_quadMesh.Draw();
#pragma endregion

#pragma region FlatBunny

    m_bunnyShader.bind();
    pvm = a_projectionMatrix * a_viewMatrix * m_bunnyTransform;
    m_bunnyShader.bindUniform("ProjectionViewModel", pvm);
    
    m_bunnyShader.bindUniform("randcolor", bunnyColour);
    // Draw bunny mesh
    m_bunnyMesh.draw();

#pragma endregion


}







