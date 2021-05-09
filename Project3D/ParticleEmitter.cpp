#include "ParticleEmitter.h"
#include "glm/ext.hpp"
#include <gl_core_4_4.h>
#include <iostream>
ParticleEmitter::ParticleEmitter()
	:m_particles(nullptr),
	m_firstDead(0),
	m_maxParticles(0),
	m_position(0, 0, 0),
	m_vao(0), m_vbo(0), m_ibo(0),
	m_vertexData(nullptr)
{

}

ParticleEmitter::~ParticleEmitter()
{
	delete[] m_particles;
	delete[] m_vertexData;

	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}

void ParticleEmitter::initialise(unsigned a_maxParticles,
	unsigned a_emitRate,
	float a_lifetime_min, float a_lifetime_max,
	float a_velocity_min, float a_velocity_max,
	float a_startSize, float a_endSize,
	const vec4& a_startColor, const vec4& a_endColor)
{
	// set up emit timers
	m_emitTimer = 0;
	m_emitRate = 1.0f / a_emitRate;

	// store all the variables passed in
	m_startColor = a_startColor;
	m_endColor = a_endColor;
	m_startSize = a_startSize;
	m_endSize = a_endSize;
	m_velocityMin = a_velocity_min;
	m_velocityMax = a_velocity_max;
	m_lifespanMin = a_lifetime_min;
	m_lifespanMax = a_lifetime_max;
	m_maxParticles = a_maxParticles;

	// create particle array
	m_particles = new Particle[m_maxParticles];
	m_firstDead = 0;

	/* create the array of vertices for the particles
	 * 4 vertices per particle for a quad.*/
	 // will be filled during update
	m_vertexData = new ParticleVertex[m_maxParticles * 4];

	// Create the index buffer data for the particles
	// 6 indices per quad of 2 triangles
	// fill it now as it never changes
	unsigned int* indexData = new unsigned int[m_maxParticles * 6];
	for (unsigned int i = 0; i < m_maxParticles; ++i)
	{
		indexData[i * 6 + 0] = i * 4 + 0;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;

		indexData[i * 6 + 3] = i * 4 + 0;
		indexData[i * 6 + 4] = i * 4 + 2;
		indexData[i * 6 + 5] = i * 4 + 3;
	}

	// create openGL Buffers
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	//bind and fill the buffers with data
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 * sizeof(ParticleVertex),
		m_vertexData, GL_DYNAMIC_DRAW);

	//bind and fill index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxParticles * 6 * sizeof(unsigned int),
		indexData, GL_STATIC_DRAW);

	//enable the vertex indexs for ParticleVertex
	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // colour
	//Layout the memory buffers
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(ParticleVertex), 0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
		sizeof(ParticleVertex), ((char*)0) + 16);

	//unbind uneeded data 
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] indexData;

}

void ParticleEmitter::emit()
{
	// only emit if there is a dead particle to use
	if (m_firstDead >= m_maxParticles)
		return;

	// resurrect the first dead particle
	Particle& particle = m_particles[m_firstDead++];

	// assign its starting position
	particle.position = m_position;

	particle.lifetime = 0;
	particle.lifespan = (rand() / (float)RAND_MAX) *
		(m_lifespanMax - m_lifespanMin) + m_lifespanMin;

	// set starting size and color
	particle.color = m_startColor;
	particle.size = m_startSize;

	// randomise velocity direction and strength
	float velocity = (rand() / (float)RAND_MAX) *
		(m_velocityMax - m_velocityMin) + m_velocityMin;




	particle.velocity.x = (rand() / (float)RAND_MAX * 2 - 1);
	particle.velocity.y = (rand() / (float)RAND_MAX * 2 - 1);
	particle.velocity.z = (rand() / (float)RAND_MAX * 2 - 1);

	particle.velocity = glm::normalize(particle.velocity) * velocity;

}

void ParticleEmitter::update(float a_deltaTime, const mat4& a_cameraTransform)
{

	m_emitTimer += a_deltaTime;

	while (m_emitTimer > m_emitRate)
	{
		emit();
		m_emitTimer -= m_emitRate;
	}

	unsigned int quad = 0;

	// update particles and turn live particles into billboard quads
	for (unsigned int i = 0; i < m_firstDead; i++)
	{
		// get current particle
		Particle* particle = &m_particles[i];
		particle->lifetime += a_deltaTime;

		if (particle->lifetime >= particle->lifespan)
		{
			// Swap alive with dead particle
			*particle = m_particles[m_firstDead - 1];
			m_firstDead--;
		}
		else
		{
			// Move particle
			particle->position += particle->velocity * a_deltaTime;

			// Size Particle
			particle->size = glm::mix(m_startSize, m_endSize,
				particle->lifetime / particle->lifespan);

			// color particle
			particle->color = glm::mix(m_startColor, m_endColor,
				particle->lifetime / particle->lifespan);


			// create quad and assign size + color
			float halfSize = particle->size * 0.5f;

			m_vertexData[quad * 4 + 0].Position = glm::vec4(halfSize, halfSize, 0, 1);
			m_vertexData[quad * 4 + 0].Color = particle->color;

			m_vertexData[quad * 4 + 1].Position = glm::vec4(-halfSize, halfSize, 0, 1);
			m_vertexData[quad * 4 + 1].Color = particle->color;

			m_vertexData[quad * 4 + 2].Position = glm::vec4(-halfSize, -halfSize, 0, 1);
			m_vertexData[quad * 4 + 2].Color = particle->color;

			m_vertexData[quad * 4 + 3].Position = glm::vec4(halfSize, -halfSize, 0, 1);
			m_vertexData[quad * 4 + 3].Color = particle->color;
			


			// create billboard transform (basis vectors to create a matrix 4 transform)
			// so that the quads assigned to each particle face towareds the camera regardless of 
			// the cameras position.
			vec3 zAxis = glm::normalize(glm::vec3(a_cameraTransform[3]) - particle->position);

			vec3 xAxis = glm::normalize(glm::cross(glm::vec3(a_cameraTransform[1]), zAxis));
			
			vec3 yAxis = glm::cross(zAxis, xAxis);
			
			mat4 billboard(vec4(xAxis, 0),
				vec4(yAxis, 0),
				vec4(zAxis, 0),
				vec4(0, 0, 0, 1));


			// Update quad transform by the billboard
			m_vertexData[quad * 4 + 0].Position = billboard * m_vertexData[quad * 4 + 0].Position + vec4(particle->position, 0);
			m_vertexData[quad * 4 + 1].Position = billboard * m_vertexData[quad * 4 + 1].Position + vec4(particle->position, 0);
			m_vertexData[quad * 4 + 2].Position = billboard * m_vertexData[quad * 4 + 2].Position + vec4(particle->position, 0);
			m_vertexData[quad * 4 + 3].Position = billboard * m_vertexData[quad * 4 + 3].Position + vec4(particle->position, 0);

			++quad;

		}
	}


}

void ParticleEmitter::draw()
{
	//bind the buffers, give only the data we need instead of the whole buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_firstDead * 4 * sizeof(ParticleVertex), m_vertexData);
	int error = glGetError();

	if (error != 0) {
		std::cout << "Before GlDrawElements: " << error << std::endl;
	}


	//bind the vertex array to be drawn then draw the amount of particles
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_firstDead * 6, GL_UNSIGNED_INT, 0);
	error = glGetError();
	if (error != 0) {
		std::cout << "After GlDrawElements: " << error << std::endl;
	}

	
}