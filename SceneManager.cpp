///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";

	/***********************************************************
	 *  DefineObjectMaterials()
	 *
	 *  This helper function is used for defining material properties for
	 *  objects in the 3D scene. Materials control how objects
	 *  interact with light using the Phong lighting model.
	 ***********************************************************/
	void DefineObjectMaterials(SceneManager* sceneManager, std::vector<SceneManager::OBJECT_MATERIAL>& materials)
	{
		SceneManager::OBJECT_MATERIAL woodMaterial;
		woodMaterial.ambientColor = glm::vec3(0.2f, 0.15f, 0.1f);
		woodMaterial.ambientStrength = 0.3f;
		woodMaterial.diffuseColor = glm::vec3(0.6f, 0.4f, 0.2f);
		woodMaterial.specularColor = glm::vec3(0.3f, 0.3f, 0.3f);
		woodMaterial.shininess = 16.0f;
		woodMaterial.tag = "wood";
		materials.push_back(woodMaterial);

		SceneManager::OBJECT_MATERIAL plasticMaterial;
		plasticMaterial.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
		plasticMaterial.ambientStrength = 0.2f;
		plasticMaterial.diffuseColor = glm::vec3(0.8f, 0.8f, 0.8f);
		plasticMaterial.specularColor = glm::vec3(0.9f, 0.9f, 0.9f);
		plasticMaterial.shininess = 85.0f;
		plasticMaterial.tag = "plastic";
		materials.push_back(plasticMaterial);

		SceneManager::OBJECT_MATERIAL ceramicMaterial;
		ceramicMaterial.ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
		ceramicMaterial.ambientStrength = 0.3f;
		ceramicMaterial.diffuseColor = glm::vec3(0.7f, 0.7f, 0.7f);
		ceramicMaterial.specularColor = glm::vec3(0.8f, 0.8f, 0.8f);
		ceramicMaterial.shininess = 64.0f;
		ceramicMaterial.tag = "ceramic";
		materials.push_back(ceramicMaterial);

		SceneManager::OBJECT_MATERIAL metalMaterial;
		metalMaterial.ambientColor = glm::vec3(0.15f, 0.15f, 0.15f);
		metalMaterial.ambientStrength = 0.2f;
		metalMaterial.diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
		metalMaterial.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
		metalMaterial.shininess = 128.0f;
		metalMaterial.tag = "metal";
		materials.push_back(metalMaterial);

		SceneManager::OBJECT_MATERIAL plantMaterial;
		plantMaterial.ambientColor = glm::vec3(0.1f, 0.2f, 0.1f);
		plantMaterial.ambientStrength = 0.4f;
		plantMaterial.diffuseColor = glm::vec3(0.3f, 0.6f, 0.3f);
		plantMaterial.specularColor = glm::vec3(0.2f, 0.4f, 0.2f);
		plantMaterial.shininess = 16.0f;
		plantMaterial.tag = "plant";
		materials.push_back(plantMaterial);
	}

	/***********************************************************
	 *  SetupSceneLights()
	 *
	 *  This helper function is used for setting up the light sources in
	 *  the 3D scene. It configures multiple light sources with
	 *  different positions and properties to properly illuminate
	 *  the scene using the Phong lighting model.
	 ***********************************************************/
	void SetupSceneLights(ShaderManager* shaderManager)
	{
		// Enable lighting in the shader
		shaderManager->setBoolValue(g_UseLightingName, true);

		// Light 1: Main overhead ceiling light (warm white) - centered above the desk
		// This simulates a typical room ceiling light providing main illumination
		shaderManager->setVec3Value("lightSources[0].position", glm::vec3(0.0f, 18.0f, 2.0f));
		shaderManager->setVec3Value("lightSources[0].ambientColor", glm::vec3(0.35f, 0.32f, 0.28f));  // Warm ambient
		shaderManager->setVec3Value("lightSources[0].diffuseColor", glm::vec3(1.0f, 0.95f, 0.85f));  // Warm white light
		shaderManager->setVec3Value("lightSources[0].specularColor", glm::vec3(0.9f, 0.9f, 0.85f));
		shaderManager->setFloatValue("lightSources[0].focalStrength", 48.0f);
		shaderManager->setFloatValue("lightSources[0].specularIntensity", 0.6f);

		// Light 2: Desk lamp from left side (warmer tone)
		// This simulates a desk lamp providing task lighting
		shaderManager->setVec3Value("lightSources[1].position", glm::vec3(-12.0f, 8.0f, 3.0f));
		shaderManager->setVec3Value("lightSources[1].ambientColor", glm::vec3(0.15f, 0.12f, 0.08f));
		shaderManager->setVec3Value("lightSources[1].diffuseColor", glm::vec3(0.9f, 0.85f, 0.7f));  // Warm desk lamp
		shaderManager->setVec3Value("lightSources[1].specularColor", glm::vec3(0.8f, 0.75f, 0.65f));
		shaderManager->setFloatValue("lightSources[1].focalStrength", 24.0f);
		shaderManager->setFloatValue("lightSources[1].specularIntensity", 0.5f);

		// Light 3: Window light from the right (cool daylight)
		// This simulates natural light coming from a window
		shaderManager->setVec3Value("lightSources[2].position", glm::vec3(20.0f, 12.0f, 5.0f));
		shaderManager->setVec3Value("lightSources[2].ambientColor", glm::vec3(0.12f, 0.15f, 0.18f));
		shaderManager->setVec3Value("lightSources[2].diffuseColor", glm::vec3(0.7f, 0.8f, 0.95f));  // Cool daylight
		shaderManager->setVec3Value("lightSources[2].specularColor", glm::vec3(0.85f, 0.9f, 1.0f));
		shaderManager->setFloatValue("lightSources[2].focalStrength", 20.0f);
		shaderManager->setFloatValue("lightSources[2].specularIntensity", 0.4f);

		// Light 4: Monitor glow (subtle blue light)
		// This simulates the screen glow from the monitor
		shaderManager->setVec3Value("lightSources[3].position", glm::vec3(0.0f, 5.0f, 0.0f));
		shaderManager->setVec3Value("lightSources[3].ambientColor", glm::vec3(0.05f, 0.08f, 0.12f));
		shaderManager->setVec3Value("lightSources[3].diffuseColor", glm::vec3(0.4f, 0.6f, 0.9f));  // Blue monitor glow
		shaderManager->setVec3Value("lightSources[3].specularColor", glm::vec3(0.5f, 0.7f, 1.0f));
		shaderManager->setFloatValue("lightSources[3].focalStrength", 12.0f);
		shaderManager->setFloatValue("lightSources[3].specularIntensity", 0.3f);
	}
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationX * rotationY * rotationZ * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/


/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// Only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene

	// Load all mesh types used in the scene
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadTorusMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadTaperedCylinderMesh();
	m_basicMeshes->LoadSphereMesh();
	m_basicMeshes->LoadConeMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadPrismMesh();

	// Load textures for the scene
	// These textures are used to create detailed appearances on 3D objects
	CreateGLTexture("textures/plantBox.jpg", "plantBox");     // Texture for plant pot
	CreateGLTexture("textures/plantStem.jpg", "plantStem");   // Texture for plant stem
	CreateGLTexture("textures/plantLeaf.png", "plantLeaf");   // Texture for plant leaves
	CreateGLTexture("textures/Wood/Wood069_1K-JPG_Color.jpg", "woodDesk");     // Texture for desk surface
	CreateGLTexture("textures/Wallpaper/Wallpaper001B_1K-JPG_Color.jpg", "wallpaper");  // Texture for walls
	CreateGLTexture("textures/Tiles/Tiles081_1K-JPG_Color.jpg", "floorTiles");  // Texture for floor
	CreateGLTexture("textures/Clay/RoofingTiles015C_1K-JPG_Color.jpg", "clay");  // Texture for ceramic objects

	// Bind all loaded textures to OpenGL texture memory slots
	BindGLTextures();

	// Define materials for objects to control lighting interaction
	// Using helper function from anonymous namespace - no header changes needed
	DefineObjectMaterials(this, m_objectMaterials);

	// Configure lighting for the scene
	// Using helper function from anonymous namespace - no header changes needed
	SetupSceneLights(m_pShaderManager);
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by
 *  transforming and drawing the basic 3D shapes
 *
 *  TEXTURING IMPLEMENTATION:
 *  - Desk uses tiled wood texture (complex technique - Rubric #2)
 *  - Plant object uses 3 different textures on multiple shapes:
 *    * plantBox texture on box (pot)
 *    * plantStem texture on cylinder (stem)
 *    * plantLeaf texture on prisms (leaves)
 *    This demonstrates cohesive multi-texture object (Rubric #3)
 ***********************************************************/
void SceneManager::RenderScene()
{
	// Declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	// FLOOR - Using plane for the floor with tile texture
	scaleXYZ = glm::vec3(50.0f, 1.0f, 50.0f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("ceramic");
	SetShaderTexture("floorTiles");
	SetTextureUVScale(15.0f, 15.0f);

	m_basicMeshes->DrawPlaneMesh();

	/****************************************************************/
	// DESK TOP SURFACE - Main working surface
	scaleXYZ = glm::vec3(16.0f, 0.4f, 8.0f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(0.0f, 3.0f, 0.0f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("wood");
	SetShaderTexture("woodDesk");
	SetTextureUVScale(3.0f, 2.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/
	// DESK LEG - Front Left
	scaleXYZ = glm::vec3(0.6f, 3.0f, 0.6f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(-7.0f, 1.5f, 3.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("wood");
	SetShaderTexture("woodDesk");
	SetTextureUVScale(1.0f, 2.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/
	// DESK LEG - Front Right
	scaleXYZ = glm::vec3(0.6f, 3.0f, 0.6f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(7.0f, 1.5f, 3.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("wood");
	SetShaderTexture("woodDesk");
	SetTextureUVScale(1.0f, 2.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/
	// DESK LEG - Back Left
	scaleXYZ = glm::vec3(0.6f, 3.0f, 0.6f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(-7.0f, 1.5f, -3.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("wood");
	SetShaderTexture("woodDesk");
	SetTextureUVScale(1.0f, 2.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/
	// DESK LEG - Back Right
	scaleXYZ = glm::vec3(0.6f, 3.0f, 0.6f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(7.0f, 1.5f, -3.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("wood");
	SetShaderTexture("woodDesk");
	SetTextureUVScale(1.0f, 2.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// BACK WALL - Using wallpaper texture

	scaleXYZ = glm::vec3(50.0f, 20.0f, 0.3f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(0.0f, 10.0f, -15.0f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("wood");
	SetShaderTexture("wallpaper");
	SetTextureUVScale(10.0f, 8.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// LEFT SIDE WALL - Creating a corner room effect

	scaleXYZ = glm::vec3(0.3f, 20.0f, 30.0f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(-25.0f, 10.0f, 0.0f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("wood");
	SetShaderTexture("wallpaper");
	SetTextureUVScale(8.0f, 8.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// RIGHT SIDE WALL - Completing the room

	scaleXYZ = glm::vec3(0.3f, 20.0f, 30.0f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(25.0f, 10.0f, 0.0f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("wood");
	SetShaderTexture("wallpaper");
	SetTextureUVScale(8.0f, 8.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// PICTURE FRAME ON BACK WALL - Adding decorative element

	// Picture frame - outer border
	scaleXYZ = glm::vec3(5.0f, 3.5f, 0.2f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(-10.0f, 12.0f, -14.7f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("wood");
	SetShaderColor(0.2f, 0.15f, 0.1f, 1.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// PICTURE FRAME - Inner picture area (background)

	scaleXYZ = glm::vec3(4.4f, 2.9f, 0.15f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(-10.0f, 12.0f, -14.55f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("ceramic");
	SetShaderColor(0.95f, 0.92f, 0.88f, 1.0f);  // Light beige background

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/
	// ARTWORK - Mountain silhouette (bottom)
	scaleXYZ = glm::vec3(3.8f, 0.8f, 0.12f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-10.0f, 11.0f, -14.5f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderMaterial("ceramic");
	SetShaderColor(0.25f, 0.35f, 0.45f, 1.0f);  // Dark blue mountains
	m_basicMeshes->DrawBoxMesh();

	// ARTWORK - Sun/Moon circle
	scaleXYZ = glm::vec3(0.6f, 0.6f, 0.6f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-9.0f, 12.8f, -14.5f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderMaterial("ceramic");
	SetShaderColor(0.95f, 0.75f, 0.35f, 1.0f);  // Golden sun
	m_basicMeshes->DrawSphereMesh();

	// ARTWORK - Decorative accent (left)
	scaleXYZ = glm::vec3(0.3f, 1.2f, 0.11f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 15.0f;
	positionXYZ = glm::vec3(-12.0f, 12.0f, -14.48f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderMaterial("ceramic");
	SetShaderColor(0.45f, 0.55f, 0.35f, 1.0f);  // Green accent
	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// CUP OF PENS on left side of desk

	scaleXYZ = glm::vec3(0.4f, 0.7f, 0.4f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(-5.5f, 3.2f, 2.0f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("plastic");
	SetShaderColor(0.3f, 0.3f, 0.35f, 1.0f);  // Dark grey pen holder to distinguish from wall

	m_basicMeshes->DrawCylinderMesh();

	/****************************************************************/

	// Pen 1 - Light blue
	scaleXYZ = glm::vec3(0.05f, 0.6f, 0.05f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-5.3f, 3.9f, 2.1f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderMaterial("plastic");
	SetShaderColor(0.408f, 0.851f, 0.988f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	// Pen 2 - Red
	scaleXYZ = glm::vec3(0.05f, 0.6f, 0.05f);
	positionXYZ = glm::vec3(-5.4f, 3.9f, 1.8f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.953f, 0.274f, 0.274f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	// Pen 3 - Grey
	scaleXYZ = glm::vec3(0.05f, 0.6f, 0.05f);
	positionXYZ = glm::vec3(-5.55f, 3.9f, 2.05f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.612f, 0.569f, 0.564f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	// Pen 4 - Green
	scaleXYZ = glm::vec3(0.05f, 0.6f, 0.05f);
	positionXYZ = glm::vec3(-5.7f, 3.9f, 2.2f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.235f, 0.909f, 0.266f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	// Pen 5 - Green
	scaleXYZ = glm::vec3(0.05f, 0.6f, 0.05f);
	positionXYZ = glm::vec3(-5.2f, 3.9f, 2.2f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.235f, 0.909f, 0.266f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	// Pen 6 - Yellow
	scaleXYZ = glm::vec3(0.05f, 0.6f, 0.05f);
	positionXYZ = glm::vec3(-5.2f, 3.9f, 1.95f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.987f, 0.987f, 0.165f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	// Pen 7 - Dark blue
	scaleXYZ = glm::vec3(0.05f, 0.6f, 0.05f);
	positionXYZ = glm::vec3(-5.6f, 3.9f, 1.85f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.247f, 0.145f, 1.0f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	// Pen 8 - Yellow
	scaleXYZ = glm::vec3(0.05f, 0.6f, 0.05f);
	positionXYZ = glm::vec3(-5.7f, 3.9f, 1.9f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.987f, 0.987f, 0.165f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// COMPUTER MONITOR STAND BASE

	scaleXYZ = glm::vec3(1.5f, 0.15f, 1.0f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(0.0f, 3.2f, -1.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("metal");
	SetShaderColor(0.2f, 0.2f, 0.2f, 1.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/
	// COMPUTER MONITOR STAND NECK

	scaleXYZ = glm::vec3(0.2f, 1.5f, 0.2f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(0.0f, 3.3f, -1.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("metal");
	SetShaderColor(0.2f, 0.2f, 0.2f, 1.0f);

	m_basicMeshes->DrawCylinderMesh();

	/****************************************************************/
	// COMPUTER MONITOR SCREEN

	scaleXYZ = glm::vec3(5.0f, 3.0f, 0.3f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(0.0f, 5.0f, -1.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("plastic");
	SetShaderColor(0.1f, 0.1f, 0.12f, 1.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/
	// COMPUTER MONITOR SCREEN - Active Display Area

	scaleXYZ = glm::vec3(4.6f, 2.6f, 0.25f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(0.0f, 5.0f, -1.4f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("plastic");
	SetShaderColor(0.3f, 0.5f, 0.7f, 1.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// KEYBOARD - Base

	scaleXYZ = glm::vec3(3.5f, 0.15f, 1.5f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(0.0f, 3.2f, 2.0f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("plastic");
	SetShaderColor(0.15f, 0.15f, 0.15f, 1.0f);  // Dark grey keyboard

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// MOUSE

	scaleXYZ = glm::vec3(0.6f, 0.3f, 0.8f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = -20.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(4.5f, 3.2f, 1.8f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("plastic");
	SetShaderColor(0.2f, 0.2f, 0.25f, 1.0f);  // Dark grey mouse

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// SCENTSY POT - right side tapered cylinder

	scaleXYZ = glm::vec3(0.6f, 0.8f, 0.6f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 45.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(-6.0f, 3.2f, -2.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("ceramic");
	SetShaderTexture("clay");
	SetTextureUVScale(1.0f, 1.0f);

	m_basicMeshes->DrawTaperedCylinderMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// SCENTSY POT WAX - torus on top

	scaleXYZ = glm::vec3(0.35f, 0.45f, 0.35f);

	XrotationDegrees = 100.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 90.0f;

	positionXYZ = glm::vec3(-6.0f, 4.0f, -2.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("ceramic");
	SetShaderColor(0.753f, 0.216f, 0.765f, 1.0f);

	m_basicMeshes->DrawTorusMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// PLANT POT

	scaleXYZ = glm::vec3(0.6f, 0.6f, 0.6f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(6.0f, 3.2f, -2.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("ceramic");
	SetShaderTexture("plantBox");
	SetTextureUVScale(1.0f, 1.0f);

	m_basicMeshes->DrawBoxMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// PLANT STEM

	scaleXYZ = glm::vec3(0.06f, 1.5f, 0.06f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(6.0f, 3.4f, -2.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("plant");
	SetShaderTexture("plantStem");
	SetTextureUVScale(1.0f, 1.0f);

	m_basicMeshes->DrawCylinderMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// PLANT LEAF - Bottom

	scaleXYZ = glm::vec3(0.4f, 0.0f, 0.3f);

	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(6.0f, 4.02f, -2.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("plant");
	SetShaderTexture("plantLeaf");
	SetTextureUVScale(1.0f, 1.0f);

	m_basicMeshes->DrawPrismMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// PLANT LEAF - Middle

	scaleXYZ = glm::vec3(0.4f, 0.0f, 0.3f);

	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(6.0f, 4.32f, -2.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("plant");
	SetShaderTexture("plantLeaf");
	SetTextureUVScale(1.0f, 1.0f);

	m_basicMeshes->DrawPrismMesh();

	/****************************************************************/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	// PLANT LEAF - Top

	scaleXYZ = glm::vec3(0.4f, 0.0f, 0.3f);

	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(6.0f, 4.62f, -2.5f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderMaterial("plant");
	SetShaderTexture("plantLeaf");
	SetTextureUVScale(1.0f, 1.0f);

	m_basicMeshes->DrawPrismMesh();

	/****************************************************************/

}
