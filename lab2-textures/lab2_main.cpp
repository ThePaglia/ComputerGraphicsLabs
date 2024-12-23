#include <GL/glew.h>

// STB_IMAGE for loading images of many filetypes
#include <stb_image.h>

#include <cstdlib>

#include <labhelper.h>

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// Various globals
///////////////////////////////////////////////////////////////////////////////

// The window we'll be rendering to
SDL_Window* g_window = nullptr;

int mag = 1;
int mini = 5;
float anisotropy = 16.0f;
float camera_pan = 0.f;
bool showUI = false;

///////////////////////////////////////////////////////////////////////////////
// Shader programs
///////////////////////////////////////////////////////////////////////////////

// The shaderProgram holds the vertexShader and fragmentShader
GLuint shaderProgram;

///////////////////////////////////////////////////////////////////////////////
// Scene objects
///////////////////////////////////////////////////////////////////////////////

// The vertexArrayObject here will hold the pointers to
// the vertex data (in positionBuffer) and color data per vertex (in colorBuffer)
GLuint positionBuffer, colorBuffer, indexBuffer, texCoordBuffer, vertexArrayObject;

GLuint positionBufferExpl, indexBufferExpl, texCoordBufferExpl, vertexArrayObjectExpl;

GLuint texture, textureExpl;

///////////////////////////////////////////////////////////////////////////////
/// This function is called once at the start of the program and never again
///////////////////////////////////////////////////////////////////////////////
void initialize()
{
	ENSURE_INITIALIZE_ONLY_ONCE();

	///////////////////////////////////////////////////////////////////////////
	// Create the vertex array object
	///////////////////////////////////////////////////////////////////////////
	// Create a handle for the vertex array object
	glGenVertexArrays(1, &vertexArrayObject);
	// Set it as current, i.e., related calls will affect this object
	glBindVertexArray(vertexArrayObject);

	///////////////////////////////////////////////////////////////////////////
	// Create the positions buffer object
	///////////////////////////////////////////////////////////////////////////
	const float positions[] = {
		// X      Y       Z
		-10.0f, 0.0f, -10.0f,  // v0
		-10.0f, 0.0f, -330.0f, // v1
		10.0f,  0.0f, -330.0f, // v2
		10.0f,  0.0f, -10.0f   // v3
	};
	// Create a handle for the vertex position buffer
	glGenBuffers(1, &positionBuffer);
	// Set the newly created buffer as the current one
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	// Send the vetex position data to the current buffer
	glBufferData(GL_ARRAY_BUFFER, labhelper::array_length(positions) * sizeof(float), positions,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false /*normalized*/, 0 /*stride*/, 0 /*offset*/);
	// Enable the attribute
	glEnableVertexAttribArray(0);

	///////////////////////////////////////////////////////////////////////////
	// Task 1 : Create the texture coordinates.
	//			Create the texture coordinates' buffer object.
	//			Set up the attrib pointer.
	//			Enable the vertex attrib array.
	///////////////////////////////////////////////////////////////////////////
	const float texCoords[] = {
		0.0f, 0.0f, // (u,v) for v0
		0.0f, 15.0f, // (u,v) for v1
		1.0f, 15.0f, // (u,v) for v2
		1.0f, 0.0f  // (u,v) for v3
	};
	// Create a handle for the vertex position buffer
	glGenBuffers(1, &texCoordBuffer);
	// Set the newly created buffer as the current one
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	// Send the vetex position data to the current buffer
	glBufferData(GL_ARRAY_BUFFER, labhelper::array_length(texCoords) * sizeof(float), texCoords,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, false /*normalized*/, 0 /*stride*/, 0 /*offset*/);
	// Enable the attribute
	glEnableVertexAttribArray(1);

	///////////////////////////////////////////////////////////////////////////
	// Create the element array buffer object
	///////////////////////////////////////////////////////////////////////////
	const int indices[] = {
		// Counter clockwise order for front face
		0, 3, 1, // Triangle 1
		1, 3, 2  // Triangle 2
	};
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, labhelper::array_length(indices) * sizeof(int), indices,
		GL_STATIC_DRAW);

	// Task 7
	glGenVertexArrays(1, &vertexArrayObjectExpl);
	glBindVertexArray(vertexArrayObjectExpl);

	const float positionsExpl[] = {
		0.0f, 0.0f,-20.0f,  // v0
		0.0f, 10.0f, -20.0f, // v1
		10.0f, 10.0f, -20.0f, // v2
		10.0f, 0.0f, -20.0f  // v3
	};

	glGenBuffers(1, &positionBufferExpl);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferExpl);
	glBufferData(GL_ARRAY_BUFFER, labhelper::array_length(positionsExpl) * sizeof(float), positionsExpl,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	const float texCoordsExpl[] = {
		0.0f, 0.0f, // (u,v) for v0
		0.0f, 1.0f, // (u,v) for v1
		1.0f, 1.0f, // (u,v) for v2
		1.0f, 0.0f  // (u,v) for v3
	};

	glGenBuffers(1, &texCoordBufferExpl);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferExpl);
	glBufferData(GL_ARRAY_BUFFER, labhelper::array_length(texCoordsExpl) * sizeof(float), texCoordsExpl,
		GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);

	const int indicesExpl[] = {
		// Counter clockwise order for front face
		0, 3, 1, // Triangle 1
		1, 3, 2  // Triangle 2
	};

	glGenBuffers(1, &indexBufferExpl);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferExpl);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, labhelper::array_length(indicesExpl) * sizeof(int), indicesExpl,
		GL_STATIC_DRAW);

	//************************************
	//			Load Texture
	//************************************
	// Task 2
	int w, h, comp;
	unsigned char* image = stbi_load("../scenes/textures/asphalt.jpg", &w, &h, &comp, STBI_rgb_alpha);

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D); // Optimizes the texture for minification
	// Sets the type of filtering to be used on magnifying and
	// minifying the active texture. These are the nicest available options.

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_LINEAR and GL_LINEAR_MIPMAP_LINEAR are the nicest options
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f); // Used to blur the texture

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // use this to clamp the texture to the edge
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // use this to repeat the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

	stbi_image_free(image); // can use free(image);

	// Task 7
	glGenTextures(1, &textureExpl);
	glBindTexture(GL_TEXTURE_2D, textureExpl);

	unsigned char* imageExpl = stbi_load("../scenes/textures/explosion.png", &w, &h, &comp, STBI_rgb_alpha);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageExpl);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

	stbi_image_free(imageExpl);

	// The loadShaderProgram and linkShaderProgam functions are defined in glutil.cpp and
	// do exactly what we did in lab1 but are hidden for convenience
	shaderProgram = labhelper::loadShaderProgram("../lab2-textures/simple.vert",
		"../lab2-textures/simple.frag");
}

///////////////////////////////////////////////////////////////////////////////
/// This function will be called once per frame, so the code to set up
/// the scene for rendering should go here
///////////////////////////////////////////////////////////////////////////////
void display(void)
{
	// The viewport determines how many pixels we are rasterizing to
	int w, h;
	SDL_GetWindowSize(g_window, &w, &h);
	// Set viewport
	glViewport(0, 0, w, h);

	// Set clear color
	glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
	// Clears the color buffer and the z-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// We disable backface culling for this tutorial, otherwise care must be taken with the winding order
	// of the vertices. It is however a lot faster to enable culling when drawing large scenes.
	glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE); // Works because I ordered the vertices in counter clockwise order
	// Disable depth testing
	glDisable(GL_DEPTH_TEST);
	// Set the shader program to use for this draw call
	glUseProgram(shaderProgram);

	// Set up a projection matrix
	float fovy = radians(45.0f);
	float aspectRatio = float(w) / float(h);
	float nearPlane = 0.01f;
	float farPlane = 400.0f;
	mat4 projectionMatrix = perspective(fovy, aspectRatio, nearPlane, farPlane);
	// Send it to the vertex shader
	int loc = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(loc, 1, false, &projectionMatrix[0].x);

	loc = glGetUniformLocation(shaderProgram, "cameraPosition");
	glUniform3f(loc, camera_pan, 10, 0);

	// Task 3.1
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(vertexArrayObject);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Task 7
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, textureExpl);

	glBindVertexArray(vertexArrayObjectExpl);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisable(GL_BLEND);

	glUseProgram(0); // "unsets" the current shader program. Not really necessary.
}

///////////////////////////////////////////////////////////////////////////////
/// This function is to hold the general GUI logic
///////////////////////////////////////////////////////////////////////////////
void gui()
{
	// ----------------- Set variables --------------------------
	ImGui::PushID("mag");
	ImGui::Text("Magnification");
	ImGui::RadioButton("GL_NEAREST", &mag, 0);
	ImGui::RadioButton("GL_LINEAR", &mag, 1);
	ImGui::PopID();

	ImGui::PushID("mini");
	ImGui::Text("Minification");
	ImGui::RadioButton("GL_NEAREST", &mini, 0);
	ImGui::RadioButton("GL_LINEAR", &mini, 1);
	ImGui::RadioButton("GL_NEAREST_MIPMAP_NEAREST", &mini, 2);
	ImGui::RadioButton("GL_NEAREST_MIPMAP_LINEAR", &mini, 3);
	ImGui::RadioButton("GL_LINEAR_MIPMAP_NEAREST", &mini, 4);
	ImGui::RadioButton("GL_LINEAR_MIPMAP_LINEAR", &mini, 5);
	ImGui::PopID();

	ImGui::SliderFloat("Anisotropic filtering", &anisotropy, 1.0, 16.0, "Number of samples: %.0f");
	ImGui::Dummy({ 0, 20 });
	ImGui::SliderFloat("Camera Panning", &camera_pan, -1.0, 1.0);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
		ImGui::GetIO().Framerate);
	// ----------------------------------------------------------
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	switch (mag)
	{
	case 0:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case 1:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	default:
		break;
	}

	switch (mini)
	{
	case 0:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	case 1:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case 2:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		break;
	case 3:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		break;
	case 4:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		break;
	case 5:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		break;
	default:
		break;
	}
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
}

int main(int argc, char* argv[])
{
	g_window = labhelper::init_window_SDL("OpenGL Lab 2");

	initialize();

	// render-loop
	bool stopRendering = false;
	while (!stopRendering)
	{
		// Inform imgui of new frame
		ImGui_ImplSdlGL3_NewFrame(g_window);

		// check events (keyboard among other)
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			// Allow ImGui to capture events.
			ImGui_ImplSdlGL3_ProcessEvent(&event);

			if (event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
			{
				stopRendering = true;
			}
			else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_g)
			{
				showUI = !showUI;
			}
			else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_PRINTSCREEN)
			{
				labhelper::saveScreenshot();
			}
		}

		// render to window
		display();

		// Render overlay GUI.
		if (showUI)
		{
			gui();
		}

		// Render the GUI.
		ImGui::Render();

		// Swap front and back buffer. This frame will now been displayed.
		SDL_GL_SwapWindow(g_window);
	}

	// Shut down everything. This includes the window and all other subsystems.
	labhelper::shutDown(g_window);
	return 0;
}