#pragma once

#include "RenderingGeometry.h"
#include <glm/glm.hpp>
#include "..\bootstrap\Application.h"

// VERTEX BUFFER
struct Vertex
{
	glm::vec4 position;
	glm::vec4 colour;
};

class RenderingGeometry : public aie::Application {
private:


public:

	RenderingGeometry();
	virtual ~RenderingGeometry();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:
	// Setup shader ()
	void setupShader();

	// DATA for VB:
	void generateGrid(unsigned int rows, unsigned int cols);
	
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	// Vertex and Index Array Buffers
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_numberOfIndices; // not used anywhere, couldnt keep up with tut
	// Shader
	unsigned int m_programID;
};