/**
*	Student name:	Krupesh Ashok Parmar
*	Student number:	1124374
*	Student mail:	k_parmar180076@fanshaweonline.ca
*	Program name:	GAME DEVELOPMENT ADVANCED PROGRAMMING
*	Course name:	INFO-6028 - Graphics - 1
*	
*					Graphics Project 1
*	
*	a) How to build this project:
*		- Select x64 platform
*		- Select Debug or Release configuration
*		- Right click on "GraphicsProject1" from Solution Explorer and hit build
*		- The build is now created for selected platform
*	
*	b) How to run this project:
*		- You can run project only from Visual Studio using key "F5" or "Ctrl + F5"
*	
*	c) User inputs:
*		- Hold right click to interact with camera
*		- While right click is held down use WASD keys to move camera
*		- Hold down shift key to move faster
*/

#include "inc_opengl.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include "global/GlobalThings.h"
#include "ShaderManager/ShaderManager.h"
#include "VAOManager/VAOManager.h"
#include "Lighting/LightHelper.h"

#include "VAOManager/VAOManager.h"
#include "GameObject/GameObject.h"
#include "System/Components/Camera.h"
#include "SceneEditor.h"
#include "PhysicsSimulation.h"

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::vec3(0.0f, 0.0f, 0.0f);

SceneEditor theSceneEditor;
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
float YAW = 90.0f;
float PITCH = 0.0f;
float SENSITIVITY = 0.1f;

float deltaTime = 0.f;
float lastFrame = 0.f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseHoldDown = false;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

// Call back signatures here
void key_callback(GLFWwindow* window,
	int key, int scancode,
	int action, int mods)
{
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		if (theSceneEditor.selectedGameObject != nullptr)
		{
			theSceneEditor.EDITOR_CAMERA->transform->position =
				theSceneEditor.selectedGameObject->transform->position + glm::vec3(-5.f, 2.f, -5.f);
		}
	}
	return;
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void ProcessMouseMovement(float, float);

void ProcessInput(GLFWwindow* );

int main()
{
	std::cout << "Booting up..." << std::endl;
	theSceneEditor = SceneEditor();
	PhysicsSimulation* physicsSimulation = new PhysicsSimulation();
	/*EDITOR_CAMERA->components["Camera"] = new Camera();
	EDITOR_CAMERA->transform->position = glm::vec3(0.f, 0.f, -25.f);*/
	if (!glfwInit())
		exit(EXIT_FAILURE);

	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);*/
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
	glfwSetErrorCallback(error_callback);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Scene view", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	theSceneEditor.InitSceneRender(window);
	
	GLuint vertex_buffer = 0;
	GLuint shaderID = 0;
	GLint vpos_location = 0;
	GLint vcol_location = 0;

	ShaderManager* pShaderManager = new ShaderManager();
	ShaderManager::Shader vertexShader01;
	ShaderManager::Shader fragmentShader01;

	vertexShader01.filename = "assets/shaders/vertexShader01.glsl";
	fragmentShader01.filename = "assets/shaders/fragmentShader01.glsl";

	if (!pShaderManager->createProgramFromFile("Shader_1", vertexShader01, fragmentShader01))
	{
		std::cout << "Couldn\'t compile shaders" << std::endl;
		std::string theLastError = pShaderManager->getLastError();
		std::cout << "Error: " << theLastError << std::endl;
		return -1;
	}
	else
	{
		std::cout << "Shaders successfully compiled :)" << std::endl;
	}

	pShaderManager->useShaderProgram("Shader_1");

	shaderID = pShaderManager->getIDFromFriendlyName("Shader_1");
	glUseProgram(shaderID);

	::g_pTheLightManager = new LightManager();
	LightHelper* pLightHelper = new LightHelper();

	::g_pTheLightManager->LoadLightUniformLocation(shaderID);

	cVAOManager* pVAOManager = new cVAOManager();
	theSceneEditor.sceneFileName = "Scenes/PhysicsProject2.xml";
	if (theSceneEditor.LoadSceneFile(pVAOManager, shaderID))
	{
		std::cout << "Scene successfully loaded!" << std::endl;
		physicsSimulation->mainSceneEditor = &theSceneEditor;
	}
	else std::cout << "Scene failed to load!" << std::endl;

	GLint mvp_location = glGetUniformLocation(shaderID, "MVP");
	GLint mModel_location = glGetUniformLocation(shaderID, "mModel");
	GLint mView_location = glGetUniformLocation(shaderID, "mView");
	GLint mProjection_location = glGetUniformLocation(shaderID, "mProjection");

	GLint mModelInverseTransform_location = glGetUniformLocation(shaderID, "mModelInverseTranspose");
	cameraRight = glm::normalize(glm::cross(cameraFront, WorldUp));
	physicsSimulation->Initialize(shaderID);
	while (!glfwWindowShouldClose(window))
	{
		float currentFfame = glfwGetTime();
		deltaTime = currentFfame - lastFrame;
		lastFrame = currentFfame;
		if(!theSceneEditor.gamePlay) ProcessInput(window);

		::g_pTheLightManager->CopyLightInformationToShader(shaderID);

		float ratio;
		int width, height;

		glm::mat4x4 matModel;
		glm::mat4x4 matProjection;
		glm::mat4x4 matView;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
		
		matView = glm::lookAt(theSceneEditor.EDITOR_CAMERA->transform->position,
			theSceneEditor.EDITOR_CAMERA->transform->position + cameraFront,
			upVector);

		if (theSceneEditor.gamePlay)
			if(physicsSimulation->m_Aircraft != nullptr)
				matView = glm::lookAt(theSceneEditor.EDITOR_CAMERA->transform->position,
					physicsSimulation->m_Aircraft->transform->position,
					upVector);

		GLint eyeLocation_UniLoc = glGetUniformLocation(shaderID, "eyeLocation");
		glUniform4f(eyeLocation_UniLoc, theSceneEditor.EDITOR_CAMERA->transform->position.x,
			theSceneEditor.EDITOR_CAMERA->transform->position.y, theSceneEditor.EDITOR_CAMERA->transform->position.z, 1.0f);

		matProjection = glm::perspective(
			0.6f,
			ratio,
			0.1f,
			10000.0f);

		for (std::vector<GameObject*>::iterator itCurrentGameObject = theSceneEditor.list_GameObjects.begin();
			itCurrentGameObject != theSceneEditor.list_GameObjects.end();
			itCurrentGameObject++)
		{
			//MeshObject* currentMeshObject = *itCurrentMesh;
			GameObject* currentGameObject = *itCurrentGameObject;
			if (!currentGameObject->enabled)
				continue;
			if (currentGameObject->parent != nullptr)
				if (!currentGameObject->parent->enabled)
					continue;
			glCullFace(GL_BACK);

			glEnable(GL_DEPTH_TEST);

			if (currentGameObject->parent == nullptr)
				matModel = glm::mat4x4(1.0f);  // identity matrix
			else matModel = currentGameObject->parent->matModel;

			//// Rotate the object
			glm::mat4 matRoationZ = glm::rotate(glm::mat4(1.0f),
				currentGameObject->transform->rotation.z,                // Angle to rotate
				glm::vec3(0.0f, 0.0f, 1.0f));       // Axis to rotate around

			glm::mat4 matRoationY = glm::rotate(glm::mat4(1.0f),
				currentGameObject->transform->rotation.y,                // Angle to rotate
				glm::vec3(0.0f, 1.0f, 0.0f));       // Axis to rotate around

			glm::mat4 matRoationX = glm::rotate(glm::mat4(1.0f),
				currentGameObject->transform->rotation.x,                // Angle to rotate
				glm::vec3(1.0f, 0.0f, 0.0f));       // Axis to rotate around


			// Move the object 
			glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f),
				currentGameObject->transform->position);


			// Scale the object
			glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
				glm::vec3(currentGameObject->transform->scale));

			// Applying all these transformations to the MODEL 
			// (or "world" matrix)
			matModel = matModel * matTranslation;

			matModel = matModel * matRoationX;
			matModel = matModel * matRoationY;
			matModel = matModel * matRoationZ;

			matModel = matModel * matScale;

			currentGameObject->matModel = matModel;

			glUniformMatrix4fv(mModel_location, 1, GL_FALSE, glm::value_ptr(matModel));
			glUniformMatrix4fv(mView_location, 1, GL_FALSE, glm::value_ptr(matView));
			glUniformMatrix4fv(mProjection_location, 1, GL_FALSE, glm::value_ptr(matProjection));

			// Inverse transpose of a 4x4 matrix removes the right column and lower row
			// Leaving only the rotation (the upper left 3x3 matrix values)
			glm::mat4 mModelInverseTransform = glm::inverse(glm::transpose(matModel));
			glUniformMatrix4fv(mModelInverseTransform_location, 1, GL_FALSE, glm::value_ptr(mModelInverseTransform));

			if (currentGameObject->meshObject->isWireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			GLint RGBA_COLOR_ULocID = glGetUniformLocation(shaderID, "RGBA_Colour");

			glUniform4f(RGBA_COLOR_ULocID,
				currentGameObject->meshObject->RGBA_color.r,
				currentGameObject->meshObject->RGBA_color.g,
				currentGameObject->meshObject->RGBA_color.b,
				currentGameObject->meshObject->RGBA_color.w);

			GLint bUseRGBA_Color_ULocID = glGetUniformLocation(shaderID, "bUseRGBA_Colour");

			if (currentGameObject->meshObject->bUse_RGBA_color)
				glUniform1f(bUseRGBA_Color_ULocID, (GLfloat)GL_TRUE);
			else
				glUniform1f(bUseRGBA_Color_ULocID, (GLfloat)GL_FALSE);

			GLint bDoNotLight_Colour_ULocID = glGetUniformLocation(shaderID, "bDoNotLight");
			if (currentGameObject->meshObject->bDoNotLight)
				glUniform1f(bDoNotLight_Colour_ULocID, (GLfloat)GL_TRUE);
			else
				glUniform1f(bDoNotLight_Colour_ULocID, (GLfloat)GL_FALSE);

			sModelDrawInfo drawingInformation;
			if (pVAOManager->FindDrawInfoByModelName(currentGameObject->meshObject->meshName, drawingInformation))
			{
				glBindVertexArray(drawingInformation.VAO_ID);

				glDrawElements(GL_TRIANGLES,
					drawingInformation.numberOfIndices,
					GL_UNSIGNED_INT,
					(void*) 0);

				glBindVertexArray(0);
			}
		}
		if (theSceneEditor.gamePlay)
		{
			physicsSimulation->Update(window, deltaTime);
			physicsSimulation->Render();
			if(physicsSimulation->m_Aircraft != nullptr)
				theSceneEditor.EDITOR_CAMERA->transform->position =
					physicsSimulation->m_Aircraft->transform->position + glm::vec3(0.f, 2.5f,-4.f);
			theSceneEditor.GamePlayUpdate(window);
		}
		theSceneEditor.RenderScene(shaderID);
		glfwSwapBuffers(window);
		glfwPollEvents();
		std::stringstream ssTitle;
		ssTitle << "Camera (x,y,z): "
			<< theSceneEditor.EDITOR_CAMERA->transform->position.x << ", "
			<< theSceneEditor.EDITOR_CAMERA->transform->position.y << ", "
			<< theSceneEditor.EDITOR_CAMERA->transform->position.z;

		std::string theText = ssTitle.str();

		glfwSetWindowTitle(window, ssTitle.str().c_str());
	}

	delete pShaderManager;
	delete ::g_pTheLightManager;

	theSceneEditor.ShutdownRender();

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		mouseHoldDown = true;
	else mouseHoldDown = false;

	if (mouseHoldDown)
	{
		float cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			cameraSpeed = 10.5f * deltaTime;
		else  cameraSpeed = 2.5f * deltaTime;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			theSceneEditor.EDITOR_CAMERA->transform->position += cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			theSceneEditor.EDITOR_CAMERA->transform->position -= cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			theSceneEditor.EDITOR_CAMERA->transform->position -= cameraRight * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			theSceneEditor.EDITOR_CAMERA->transform->position += cameraRight * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)     // Down
		{
			SceneEditor::EDITOR_CAMERA->transform->position.y -= cameraSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)      // Up
		{
			SceneEditor::EDITOR_CAMERA->transform->position.y += cameraSpeed;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;
	if(mouseHoldDown)
		ProcessMouseMovement(xoffset, yoffset);
}

void ProcessMouseMovement(float xoffset, float yoffset)
{
	xoffset *= SENSITIVITY;
	yoffset *= SENSITIVITY;

	YAW += xoffset;
	PITCH += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (PITCH > 89.9f)
		PITCH = 89.9f;
	if (PITCH < -89.9f)
		PITCH = -89.9f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(YAW)) * cos(glm::radians(PITCH));
	direction.y = sin(glm::radians(PITCH));
	direction.z = sin(glm::radians(YAW)) * cos(glm::radians(PITCH));
	cameraFront = glm::normalize(direction);
	cameraRight = glm::normalize(glm::cross(cameraFront, WorldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

	// update Front, Right and Up Vectors using the updated Euler angles
	
}
