#include "RenderingGeometry.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "..\bootstrap\Application.h"

#include <gl_core_4_4.h>
using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

RenderingGeometry::RenderingGeometry() 
{

}

RenderingGeometry::~RenderingGeometry() 
{

}

bool RenderingGeometry::startup() 
{
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(64000, 64000, 64000, 64000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);


	setupShader();
	generateGrid(10, 10);
	return true;
}

void RenderingGeometry::shutdown() 
{

	Gizmos::destroy();
}

void RenderingGeometry::update(float deltaTime) 
{

	// query time since application started
	float time = getTime();

	// rotate camera
	m_viewMatrix = glm::lookAt(vec3(glm::sin(time) * 10, 10, glm::cos(time) * 10),
		vec3(0), vec3(0, 1, 0));

	// wipe the gizmos clean for this frame
	Gizmos::clear();


	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));



	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void RenderingGeometry::draw()
{

	// wipe the screen to the background colour
	clearScreen();

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
		getWindowWidth() / (float)getWindowHeight(),
		0.1f, 1000.f);

	glm::mat4 projView = m_projectionMatrix * m_viewMatrix;

	glUseProgram(m_programID);
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(projView));

	glBindVertexArray(m_VAO);						// VAO with VBO/IBO?

	unsigned int indexCount = m_numberOfIndices;	// IBO



	// DRAW triangle mesh
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}
/////////////////////////////////		SHADER			//////////////////////////////////////
void RenderingGeometry::setupShader()
{
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 position; \
							layout(location=1) in vec4 colour; \
							out vec4 vColour; \
							uniform mat4 projectionViewWorldMatrix; \
							void main() {vColour = colour; gl_Position = projectionViewWorldMatrix * position; }";

	const char* fsSource = "#version 410\n \
							in vec4 vColour; \
							out vec4 fragColor; \
							void main() {fragColor = vColour; }";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;

	}
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}


// DATA: GRID
void RenderingGeometry::generateGrid(unsigned int rows, unsigned int cols)
{
	/////////////////////////////////		VBO			//////////////////////////////////////

	// VBO Step 1. VBO Data creation - Create a grid of vertex points

	// 1. Allocate memory to hold vertex data
	Vertex* aoVertices = new Vertex[rows * cols];
	// 2. Populate memory with vertex data

	// ROWS
	for (unsigned int r = 0; r < rows; ++r)
	{
		// COLUMNS
		for (unsigned int c = 0; c < cols; ++c)
		{
			Vertex& vert = aoVertices[r * cols + c];
			vert.position = glm::vec4((float)c, 0, (float)r, 1.0f);
			vert.colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}
	}
// VBO Step 2. CREATE & BIND BUFFERS - Map data to GPU
	// a. Create Buffer: Size 1
	glGenBuffers(1, &m_VBO);
	// b. Bind the VBO to VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	// c. Pass in the Data variables from VBO to VAO
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
	// d. Assign Data to memory index 0 or 1.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

/////////////////////////////////		IBO Part 1			//////////////////////////////////////

	// IBO Step 1: Create Index loop
	// Create Index Array
	m_numberOfIndices = (rows - 1) * (cols - 1) * 6;
	unsigned int* auiIndices = new unsigned int[m_numberOfIndices];
	unsigned int index = 0;
	// Generate the index positions of each triangle that forms a quad in the VBO grid.
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			// DRAW QUAD, comprised of two triangles:
			// Tri 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);

			// Tri 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}
/////////////////////////////////		IBO	Part 2			//////////////////////////////////////
	// IBO Step 2: CREATE & BIND BUFFER
	// a. Create Buffer: Size 1
	glGenBuffers(1, &m_IBO);
	// b. Bind the IBO to VAO 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	// c. Pass in the Index from IBO to VAO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
	// d. Assign Data to memory index 0 or 1.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

/////////////////////////////////			VAO				//////////////////////////////////////

// VAO Step 1: CREATE & BIND
	// a. Create Array: Size 1
	glGenVertexArrays(1, &m_VAO);
	// b. Bind the VAO to VAO ?
	glBindVertexArray(m_VAO);
	// c. BIND & FILL VBO & IBO to VAO
	// Bind 1: VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	// Bind 2: IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	
	// d. Assign Data to memory index 0 and 1.
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// e. VAO Attributes: 
	// VBO Attributes: Index 0, Size 4, Type F, normalized, stride, pointer
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	// IBO Attributes: Index 1, Size 4, Type F, normalized, stride, pointer
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));

	glBindVertexArray(0);

	// 4. Delete allocated memory
	// Delete VBO array
	delete[] aoVertices;
	// Delete IBO array
	delete[] auiIndices;
}


