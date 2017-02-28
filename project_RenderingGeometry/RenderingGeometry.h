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
	
	// CAMERA:	
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	// Vertex and Index Array Buffers
	unsigned int m_VAO;
	unsigned int m_VBO;	// Storages Vertex Data from GPU. 
	unsigned int m_IBO;

	unsigned int m_numberOfIndices; // not used anywhere, couldnt keep up with tut
	
	// Shader program
	unsigned int m_programID;
};

/*
1. VBO: Storage of Vertex Data from GPU.
		1. CREATE and BIND the VBO.
		2. Inform OpenGL of data size, and type.
		3. Map vertex data to VBO.
		4. DELETE array

2. IBO:	Drawing Instructions to OpenGL: Order to draw the Vertex Points
		1. CREATE Index array
		2. Index VBO data coordinates
		3. Generate and fill OpenGL Index Buffer
		4. DELETE array

3. VAO:	Describes the vertex attributes
		1. 
*/