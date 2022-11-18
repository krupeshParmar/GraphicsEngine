#include "PhysicsSimulation.h"
#include "VAOManager/VAOManager.h"


// Helper Functions

int CalculateHashValue(float x, float y, float z)
{
	int hashValue = 0;

	assert(x + 150 > 0);
	assert(y + 100 > 0);
	assert(z + 150 > 0);

	hashValue += floor(x + 150) / 100 * 1000000;
	hashValue += floor(y + 100) / 100 * 1000;
	hashValue += floor(z + 150) / 100;
	/*int hashValue = 0;

	assert(x + 128 > 0);
	assert(y + 128 > 0);
	assert(z + 128 > 0);

	hashValue += floor(x + 128) / 100 * 100000;
	hashValue += floor(y + 128) / 100 * 100;
	hashValue += floor(z + 128) / 100;*/

	/*int hashValue = 0;

	assert(x + 128 > 0);
	assert(y + 300 > 0);
	assert(z + 128 > 0);

	hashValue += floor(x + 128) / 100 * 10000;
	hashValue += floor(y + 300) / 100 * 100;
	hashValue += floor(z + 128) / 100;*/
	return hashValue;
}

int CalculateHashValue(const Vector3& v)
{
	return CalculateHashValue(v.x, v.y, v.z);
}

PhysicsSimulation::PhysicsSimulation()
{

}

PhysicsSimulation::~PhysicsSimulation()
{
	delete m_PhysicsDebugRenderer;
}

void PhysicsSimulation::Initialize(int ShaderId)
{
	//m_PhysicsDebugRenderer = new PhysicsDebugRenderer();

	PrepareDemo();

	LoadStaticModelToOurAABBEnvironment("island", 
		glm::vec3(0), 1.f);

	const std::map<int, std::vector<Triangle*>> aabb = m_PhysicsSystem.GetAABBStructure();
	std::map<int, std::vector<Triangle*>>::const_iterator aabbIt = aabb.begin();
	GameObject* parentGameObject = new GameObject();
	parentGameObject->transform->Reset();
	parentGameObject->name = "PartialGameObjects";
	mainSceneEditor->list_GameObjects.push_back(parentGameObject);
	for (; aabbIt != aabb.end(); aabbIt++)
	{
		int hashValue = (*aabbIt).first;
		std::vector<Triangle*> triangles = (*aabbIt).second;

		std::vector<glm::vec3> vertices;
		std::vector<int> faces;
		std::vector<unsigned int> indices;

		sModelDrawInfo drawInfo;
		for (int i = 0; i < triangles.size(); i++) {
			Triangle* triangle = triangles[i];
			glm::vec3 A = triangle->A.GetGLM();
			glm::vec3 B = triangle->B.GetGLM();
			glm::vec3 C = triangle->C.GetGLM();
			vertices.push_back(A);
			vertices.push_back(B);
			vertices.push_back(C);
			//indices.push_back(triangle->index);
			faces.push_back(i * 3);				// 0	3	6
			faces.push_back(i * 3 + 1);			// 1	4	7
			faces.push_back(i * 3 + 2);			// 2	5	8
		}

		drawInfo.meshName = "partialMesh" + std::to_string(hashValue);
		drawInfo.numberOfVertices = vertices.size();
		drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];
		for (int i = 0; i < drawInfo.numberOfVertices; i++)
		{
			sVertex vertex;
			vertex.x = vertices[i].x;
			vertex.y = vertices[i].y;
			vertex.z = vertices[i].z;
			if (vertex.y < 1.f)
			{
				vertex.r = 64.f / 255.f;
				vertex.g = 224.f / 255.f;
				vertex.b = 208.f / 255.f;
			}
			else if (vertex.y > 1.f && vertex.y < 50.f)
			{
				vertex.r = 123.f / 255.f;
				vertex.g = 63.f / 255.f;
				vertex.b = 0.f / 255.f;
			}
			else {
				vertex.r = 255.f / 255.f;
				vertex.g = 255.f / 255.f;
				vertex.b = 255.f / 255.f;
			}
			drawInfo.pVertices[i] = vertex;
		}
		//drawInfo.numberOfTriangles = faces.size();
		drawInfo.numberOfIndices = faces.size();
		unsigned int vertex_element_index_index = 0;
		drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

		for (unsigned int triangleIndex = 0; triangleIndex != faces.size(); triangleIndex++)
		{
			drawInfo.pIndices[triangleIndex] = faces[triangleIndex];
		}
		
		/*for (unsigned int triangleIndex = 0; triangleIndex != drawInfo.numberOfIndices; triangleIndex++)
		{
			drawInfo.pIndices[triangleIndex] = faces[triangleIndex];
		}*/

		mainSceneEditor->mainVAOManager->LoadModelIntoVAO("partialMesh" + std::to_string(hashValue),
			drawInfo, ShaderId);
		GameObject* gameobject = new GameObject();
		gameobject->meshObject->meshName = drawInfo.meshName;
		gameobject->meshObject->bDoNotLight = true;
		gameobject->meshObject->bUse_RGBA_color = false;
		gameobject->meshObject->RGBA_color = glm::vec4(1.f,0.4f,1.f,1.f);
		gameobject->meshObject->isWireframe = false;
		gameobject->name = drawInfo.meshName;
		gameobject->transform->Reset();
		gameobject->parent = parentGameObject;
		parentGameObject->children.push_back(gameobject);
		mainSceneEditor->list_GameObjects.push_back(gameobject);
		m_PartialGameObjects.emplace(hashValue, gameobject);
	}
}

void PhysicsSimulation::Update(GLFWwindow* window,float dt)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (!canMoveForward)
			return;
		m_Aircraft->transform->position += glm::vec3(0.f, 0.f, 1.f) * 3.f * dt;
		for (int i = 0; i < m_AircraftBoundingBoxes.size(); i++)
		{
			m_AircraftBoundingBoxes[i]->transform->position 
				+= glm::vec3(0.f, 0.f, 1.f) * 3.f * dt;
		}
	}
}

void PhysicsSimulation::Render()
{
	for (int i = 0; i < m_AircraftBoundingBoxes.size(); i++)
	{
		int hashValue = CalculateHashValue(m_AircraftBoundingBoxes[i]->transform->position);
		std::cout << "HashValue: " << hashValue << std::endl;
		auto resultIt = m_PartialGameObjects.find(hashValue);
		if (resultIt != m_PartialGameObjects.end())
			if (resultIt->second != nullptr)
			{
				canMoveForward = false;
				resultIt->second->meshObject->RGBA_color = glm::vec4(1.f, 1.f, 1.f, 1.f);
				std::cout << "Collision at " << resultIt->second->name << std::endl;
			}
	}
}

void PhysicsSimulation::PrepareDemo()
{
	m_Aircraft = mainSceneEditor->GetGameObjectByName("mig29");
	GameObject* boundingParent = mainSceneEditor->GetGameObjectByName("boundingboxes");
	if (boundingParent != nullptr)
	{
		for (int i = 0; i < boundingParent->children.size(); i++)
		{
			m_AircraftBoundingBoxes.push_back(boundingParent->children[i]);
		}
	}
	//std::vector<GameObject*> boundingBoxes = mainSceneEditor->GetGameObjectByName("BoundingBoxes")->children;
	//for (int i = 0; i < boundingBoxes.size(); i++)
	//{
	//	std::vector<glm::vec3> vertices;
	//	std::vector<int> triangles;

	//	unsigned int unused1, unused2;

	//	sModelDrawInfo drawInfo;
	//	mainSceneEditor->mainVAOManager->FindDrawInfoByModelName("mig29", drawInfo);
	//	Vector3 minPoints = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	//	Vector3 maxPoints = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
	//	for (int i = 0; i < vertices.size(); i++) {
	//		glm::vec3& vertex = vertices[i];

	//		if (minPoints.x > vertex.x)
	//			minPoints.x = vertex.x;
	//		if (minPoints.y > vertex.y)
	//			minPoints.y = vertex.y;
	//		if (minPoints.z > vertex.z)
	//			minPoints.z = vertex.z;

	//		if (maxPoints.x < vertex.x)
	//			maxPoints.x = vertex.x;
	//		if (maxPoints.y < vertex.y)
	//			maxPoints.y = vertex.y;
	//		if (maxPoints.z < vertex.z)
	//			maxPoints.z = vertex.z;
	//	}

	//	// Calculate the point halfway between the minPoints, and maxPoints
	//	Vector3 halfExtents = (maxPoints - minPoints) / 2.f;
	//	Vector3 centerPoint = minPoints + halfExtents;

	//	printf("Minimum Points: (%.2f, %.2f, %.2f)\n",
	//		minPoints.x,
	//		minPoints.y,
	//		minPoints.z);
	//	printf("Maximum Points: (%.2f, %.2f, %.2f)\n",
	//		maxPoints.x,
	//		maxPoints.y,
	//		maxPoints.z);
	//	printf("Half Extents: (%.2f, %.2f, %.2f)\n",
	//		halfExtents.x,
	//		halfExtents.y,
	//		halfExtents.z);
	//	printf("Center Point: (%.2f, %.2f, %.2f)\n",
	//		centerPoint.x,
	//		centerPoint.y,
	//		centerPoint.z);

	//	m_AircraftBoundingBox.centerPoint = centerPoint;
	//	m_AircraftBoundingBox.halfExtents = halfExtents;
	//	m_AircraftBoundingBox.maxPoints = maxPoints;
	//	m_AircraftBoundingBox.minPoints = minPoints;

	//	m_PhysicsDebugRenderer->AddPhysicsObject(m_Aircraft);
	//}
}

void PhysicsSimulation::LoadStaticModelToOurAABBEnvironment(const std::string& modelName, const Vector3& position, float scale)
{
	GameObject* island = mainSceneEditor->GetGameObjectByName(modelName);

	std::vector<sVertex> vertices;
	std::vector<int> triangles;

	unsigned int unused1, unused2;
	printf("Starting to load Large Model...\n");

	sModelDrawInfo drawInfo;
	mainSceneEditor->mainVAOManager->FindDrawInfoByModelName(modelName, drawInfo);
	for(int i = 0; i < drawInfo.numberOfVertices; i++)
		vertices.push_back(drawInfo.pVertices[i]);
	for (int i = 0; i < drawInfo.numberOfVertices; i+=3)
	{
		triangles.push_back(i);
	}

	printf("Done loading large model!\n");
	Vector3 minPoints = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 maxPoints = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
	glm::vec3 pos = position.GetGLM();
	for (int i = 0; i < vertices.size(); i++) {
		glm::vec3 vertex = glm::vec3(vertices[i].x, vertices[i].y, vertices[i].z);
		//vertex *= scale;
		vertex += pos;

		if (minPoints.x > vertex.x)
			minPoints.x = vertex.x;
		if (minPoints.y > vertex.y)
			minPoints.y = vertex.y;
		if (minPoints.z > vertex.z)
			minPoints.z = vertex.z;

		if (maxPoints.x < vertex.x)
			maxPoints.x = vertex.x;
		if (maxPoints.y < vertex.y)
			maxPoints.y = vertex.y;
		if (maxPoints.z < vertex.z)
			maxPoints.z = vertex.z;
	}

	printf("MinPoints: (%.2f, %.2f, %.2f)\nMaxPoints: (%.2f, %.2f, %.2f)\n",
		minPoints.x, minPoints.y, minPoints.z,
		maxPoints.x, maxPoints.y, maxPoints.z);

	/** For rendering purposes only.. **/
	/*m_BigShipGamObject = GDP_CreateGameObject();
	m_BigShipGamObject->Position = pos;
	m_BigShipGamObject->Renderer.ShaderId = 1;
	m_BigShipGamObject->Renderer.MaterialId = g_SphereMaterialId;
	m_BigShipGamObject->Renderer.MeshId = m_ShipModelId;
	m_BigShipGamObject->Scale = glm::vec3(1.0f);
	m_BigShipGamObject->Enabled = true;*/
	/** End for rendering only **/

	for (int i = 0; i < drawInfo.numberOfIndices; i += 3)
	{
		glm::vec3 vertex1 = glm::vec3(drawInfo.pVertices[drawInfo.pIndices[i]].x,
			drawInfo.pVertices[drawInfo.pIndices[i]].y, 
			drawInfo.pVertices[drawInfo.pIndices[i]].z );
		glm::vec3 vertex2 = glm::vec3(drawInfo.pVertices[drawInfo.pIndices[i + 1]].x,
			drawInfo.pVertices[drawInfo.pIndices[i+1]].y, 
			drawInfo.pVertices[drawInfo.pIndices[i + 1]].z );
		glm::vec3 vertex3 = glm::vec3(drawInfo.pVertices[drawInfo.pIndices[i + 2]].x,
			drawInfo.pVertices[drawInfo.pIndices[i + 2]].y,
			drawInfo.pVertices[drawInfo.pIndices[i + 2]].z );
		/*glm::vec3 vertex1 = glm::vec3(vertices[i].x, vertices[i].y, vertices[i].z);
		glm::vec3 vertex2 = glm::vec3(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z);
		glm::vec3 vertex3 = glm::vec3(vertices[i + 2].x, vertices[i + 2].y, vertices[i + 2].z);*/
		Point a = Point(vertex1 + pos);
		Point b = Point(vertex2 + pos);
		Point c = Point(vertex3 + pos);

		int hashA = CalculateHashValue(a);
		int hashB = CalculateHashValue(b);
		int hashC = CalculateHashValue(c);
		if (a.GetGLM() == glm::vec3(0) || b.GetGLM() == glm::vec3(0) || c.GetGLM() == glm::vec3(0))
		{
			int break_me = 0;
		}
		//printf("(%.2f, %.2f, %.2f) -> %d\n", a.x, a.y, a.z, hashA);
		//printf("(%.2f, %.2f, %.2f) -> %d\n", b.x, b.y, b.z, hashB);
		//printf("(%.2f, %.2f, %.2f) -> %d\n", c.x, c.y, c.z, hashC);

		Triangle* newTriangle = new Triangle(a, b, c, drawInfo.pIndices[i]);

		m_PhysicsSystem.AddTriangleToAABBCollisionCheck(hashA, newTriangle);

		if (hashA != hashB)
			m_PhysicsSystem.AddTriangleToAABBCollisionCheck(hashB, newTriangle);

		if (hashC != hashB && hashC != hashA)
			m_PhysicsSystem.AddTriangleToAABBCollisionCheck(hashC, newTriangle);
	}

}


