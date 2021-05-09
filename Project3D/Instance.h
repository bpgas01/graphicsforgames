#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <functional>
#include  <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class Camera;
class Scene;

namespace aie
{
	class OBJMesh;
	class ShaderProgram;
}


class Instance
{

public:
	// Contructors + overloads for Instance Classes
	Instance(glm::mat4 a_transform, aie::OBJMesh* a_mesh, aie::ShaderProgram* a_shader);
	Instance(glm::vec3 a_position, glm::vec3 a_eulerAngles, glm::vec3 a_scale, aie::OBJMesh* a_mesh, aie::ShaderProgram* a_shader, std::string name);

	// Draw Scene object
	void Draw(Scene* scene);

	 // Creating transform object from matrix3 data
	 glm::mat4 MakeTransform(glm::vec3 a_position, glm::vec3 a_eulerAngles, glm::vec3 a_scale);

	 
	 void DecomposeMatrix()
	 {
		 glm::decompose(m_transform, scale, rotation, translation, skew, perspective);
		 rotation = glm::conjugate(rotation); 	
	 }

	 // Getters for instance objects
	glm::vec3 GetPosition() const { return translation; }
	glm::vec3 GetScale() const { return scale; }
	glm::quat GetRotation() const { return rotation; }
	glm::vec3 GetSkew() const { return skew; }
	glm::vec4 GetPerspective() const { return perspective; }

	glm::mat4 GetTransform() const { return m_transform; }

	glm::mat4 m_transform;
	glm::vec3 m_rotation;
	std::string name;
protected:



	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	
	aie::OBJMesh* m_mesh;
	aie::ShaderProgram* m_shader;
	
};

