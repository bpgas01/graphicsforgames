#pragma once
#include "glm/glm.hpp"
using namespace glm;

// Struct for each particle
struct Particle
{
	vec3 position;
	vec3 velocity;
	vec4 color;
	float size;
	float lifetime;
	float lifespan;
};

// 
struct ParticleVertex
{
	vec4 Position;
	vec4 Color;
};

class ParticleEmitter
{
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();


	void update(float a_deltaTime, const mat4& a_cameraTransform);


	// Setters for IMGUI debug logic
	void set_starting_color(const glm::vec4 color) { m_startColor = color; }
	void set_ending_color(const glm::vec4 color) { m_endColor = color; }

	void set_lifespan(const float min, const float max) { m_lifespanMin = min; m_lifespanMax = max; }
	void set_velocity(const float min, const float max) { m_velocityMin = min; m_velocityMax = max; }
	void set_size(const float start_size, const float end_size) { m_endSize = end_size; m_startSize = start_size; }
	void set_amount(const float amount) { m_maxParticles = amount; }

	// initilise the emitter 
	void initialise(unsigned int a_maxParticles,
		unsigned int a_emitRate,
		float a_lifetime_min, float a_lifetime_max,
		float a_velocity_min, float a_velocity_max,
		float a_startSize, float a_endSize,
		const vec4& a_startColor, const vec4& a_endColor);
	// emit new particle
	void emit();
	// draw particles
	void draw();
protected:

	Particle* m_particles;
	unsigned int m_firstDead;
	unsigned int m_maxParticles;
	unsigned int m_vao, m_vbo, m_ibo;
	ParticleVertex* m_vertexData;

	vec3 m_position;

	float m_emitTimer;
	float m_emitRate;

	float m_lifespanMin;
	float m_lifespanMax;

	float m_velocityMin;
	float m_velocityMax;

	float m_startSize;
	float m_endSize;

	vec4 m_startColor;
	vec4 m_endColor;


};