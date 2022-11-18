#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "VAOManager/VAOManager.h"
#include "inc_opengl.h"
#include <pugixml.hpp>
#include <vector>
#include "System/Components/Camera.h"
#include "GameObject/GameObject.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include "Lighting/Light.h"
#include "Lighting/LightManager.h"
#include "global/GlobalThings.h"
#include <imgui/misc/imgui_stdlib.h>

class SceneEditor
{
public:
	std::vector<GameObject*> list_GameObjects;
	static GameObject* EDITOR_CAMERA;
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	static GameObject* selectedGameObject;
	std::string sceneName;
	cVAOManager* mainVAOManager;
	std::string sceneFileName = "";

	SceneEditor();
	~SceneEditor();

	void GamePlayUpdate(GLFWwindow* window);
	bool gamePlay = false;

	bool LoadPlyFiles(std::string fileName, sModelDrawInfo& modelDrawInfo);
	bool LoadSceneFile(cVAOManager* pVAOManager, GLuint shaderID);
	bool SaveSceneFile();
	void RenderScene(GLuint shaderID);
	void CreateNewGameObject();
	GameObject* CreateNewGameObject(std::string name);
	void DeleteGameObjects(std::vector<GameObject*>);
	void ShutdownRender();
	bool InitSceneRender(GLFWwindow* window);
	void DuplicateGameObject(GameObject* gameobject);
	GameObject* GetGameObjectByName(std::string);
};

