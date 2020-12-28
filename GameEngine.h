#pragma once

// INCLUDERE OPENGL

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SOIL2.h>

// INCLUDERE FUNCTIONALITATI DIN LIBRARIA STANDARD

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <bitset>

// DECLARAREA TUTUROR CLASELOR SI STRUCTURILOR

// CLASA PRINCIPALA
class GameEngine;

// SISTEME
class WindowManager;
class TimeManager;
class UserInputManager;
class EntityManager;
class AssetManager;
class MovementManager;
class CollisionManager;
class AnimationManager;
class ArtificialIntelligenceManager;

// CLASA SINGLETON PLAYER
class Player;

// RENDERER2D
class Renderer2D;

// ENTITATE
class Entity;
// COMPONENTA (CLASA VIRTUALA, TOATE COMPONENTELE CE VOR FI FOLOSITE SE MOSTENESC DIN ACEASTA)
class Component;

// CE TIPURI DE ENTITATI EXISTA
enum class EntityType
{
	character,
	terrain,
	bullet,
	landScape,
};

// IN CE STADIU DE ANIMATIE POT FI ENTITATILE CARE AU ASA CEVA IN PRIMUL RAND
const int MAX_ANIMATIONS_ENTITY = 9;

enum class EntityAnimation
{
	null = -1,
	standing = 0,
	walkingRight = 1,
	walkingLeft = 2,
	jumpingRight = 3,
	jumpingLeft = 4,
	fallingRight = 5,
	fallingLeft = 6,
	dyingFromRight = 7,
	dyingFromLeft = 8,
};

// CE TIPURI DE INTELIGENCE ARTIFICALE EXISTA

enum class ArtificialIntelligence
{
	passive = 0,
	aggresive = 1,
};

namespace AI
{
	const double EPSILON_X_AGGRESIVE = 90.0; // DISTANTA MAXIMA LA CARE VA INCERCA O ENTITATE SA FIE FATA DE JUCATOR PE AXA OX (UNA AGRESIVA PE MOMENT)
	const double EPSILON_Y_AGGRESIVE = 350.0; // ANALOG PE OY
}

// STRUCTURA UNEI TEXTURI
struct Texture;

// COMPONENT SYSTEM

const int MAX_ENTITIES = 2048;
const int MAX_COMPONENTS = 64;

using ComponentTypeID = int;

using ComponentBitset = std::bitset<MAX_COMPONENTS>;
using ComponentList = std::array<Component*, MAX_COMPONENTS>;

inline ComponentTypeID InternalGetComponentTypeID()
{
	static ComponentTypeID lastID = 0;
	return lastID++;
}

template<typename T>
inline ComponentTypeID GetComponentTypeID()
{
	static const ComponentTypeID typeID = InternalGetComponentTypeID();
	return typeID;
}

// CLASELE COMPONENTELOR CE SE MOSTENESC DIN COMPONENTA DE BAZA, VIRTUALA
// 2D
class Position2D;
class Hitbox2D;
class Texture2D;
class TextureBox2D;
class Render2D;
class Speed2D;
class Acceleration2D;
class Gravity2D;
class Animation2D;
class MovementSpeed2D;
class ArtificialIntelligence2D;

// 3D

// MAI JOS, TOATE DECLARILE NU SUNT IN ORDINEA DE MAI SUS, DIN CAUZA CA UNELE DEPIND DE ALTELE

// CLASA WINDOW MANAGER

class WindowManager
{
public:

	static WindowManager* Get()
	{
		if (WindowManager::instance == nullptr)
		{
			WindowManager::instance = new WindowManager();
		}

		return WindowManager::instance;
	}

	void CreateWindow(int windowWidth, int windowHeight, std::string windowTitle)
	{
		this->windowWidth = windowWidth;
		this->windowHeight = windowHeight;
		this->windowTitle = windowTitle;

		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		this->window = glfwCreateWindow(this->windowWidth, this->windowHeight, this->windowTitle.c_str(), NULL, NULL);
		//glfwGetPrimaryMonitor();

		glfwMakeContextCurrent(this->window);

		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glewInit();

		glfwSwapInterval(1);
	}

	void UpdateWindow()
	{
		glfwSwapBuffers(this->window);

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);
	}

	void DestroyWindow()
	{
		glfwSetWindowShouldClose(this->window, true);

		glfwDestroyWindow(this->window);

		this->window = nullptr;
	}

	void SetWindowShouldClose(bool state)
	{
		glfwSetWindowShouldClose(this->window, state);
	}

	void SetClearColor(double red, double green, double blue)
	{
		glClearColor(red, green, blue, 1.0);
	}

	void SetClearColor(double red, double green, double blue, double alpha)
	{
		glClearColor(red, green, blue, alpha);
	}

	bool ShouldWindowClose()
	{
		return glfwWindowShouldClose(this->window);
	}

	GLFWwindow* GetWindow()
	{
		return this->window;
	}

	int GetWindowWidth()
	{
		return this->windowWidth;
	}

	int GetWindowHeight()
	{
		return this->windowHeight;
	}

private:

	static WindowManager* instance;

	int windowWidth = 1024;
	int windowHeight = 768;

	std::string windowTitle = "GameEngine";

	GLFWwindow* window = nullptr;

	WindowManager() {};

	WindowManager(const WindowManager&) = delete;
};

WindowManager* WindowManager::instance = nullptr;

// CLASA TIME MANAGER

class TimeManager
{
public:

	static TimeManager* Get()
	{
		if (TimeManager::instance == nullptr)
		{
			TimeManager::instance = new TimeManager();
		}

		return TimeManager::instance;
	}

	void UpdateDeltaTime()
	{
		this->previousTime = this->currentTime;
		this->currentTime = glfwGetTime();
		this->deltaTime = this->currentTime - this->previousTime;
	}

	double GetDeltaTime()
	{
		return this->deltaTime;
	}

	double GetCurrentTime()
	{
		return this->currentTime;
	}

private:

	double deltaTime = 0.0;

	double currentTime = 0.0;
	double previousTime = 0.0;

	static TimeManager* instance;

	TimeManager()
	{
		glfwSetTime(0.0);
	}

	TimeManager(const TimeManager&) = delete;
};

TimeManager* TimeManager::instance = nullptr;

// CLASA SINGLETON PLAYER

class Player
{
public:

	static Player* Get()
	{
		if (Player::instance == nullptr)
		{
			Player::instance = new Player();
		}

		return Player::instance;
	}

	void SetEntity(Entity* entity)
	{
		this->entity = entity;
	}

	Entity* GetEntity()
	{
		return this->entity;
	}

	void SetCameraShouldFollowPlayer(bool state)
	{
		this->cameraFollowsPlayer = state;
	}

	bool ShouldCameraFollowPlayer()
	{
		return this->cameraFollowsPlayer;
	}

private:

	static Player* instance;

	Entity* entity;

	bool cameraFollowsPlayer;

	Player()
	{
		this->entity = nullptr;

		this->cameraFollowsPlayer = false;
	}

	Player(const Player&) = delete;
};

Player* Player::instance = nullptr;

// CLASA ENTITY MANAGER

class EntityManager
{
public:

	static EntityManager* Get()
	{
		if (EntityManager::instance == nullptr)
		{
			EntityManager::instance = new EntityManager();
		}

		return EntityManager::instance;
	}

	void AddCharacter(Entity* entity)
	{
		this->characters.push_back(entity);
	}

	void AddTerrain(Entity* entity)
	{
		this->terrains.push_back(entity);
	}

	void AddBullet(Entity* entity)
	{
		this->bullets.push_back(entity);
	}

	void AddAnimatedEntity(Entity* entity)
	{
		this->animatedEntities.push_back(entity);
	}

	void AddArtificialIntelligence(Entity* entity)
	{
		this->artificalIntelligence.push_back(entity);
	}

	void RemoveCharacter(Entity* entity) // DOAR ELIMINA DIN MANAGER. ARE OVERHEAD DE MEMORIE DACA NU STERGEM ENTITATEA MANUAL
	{
		for (int i = 0; i < this->characters.size(); i++)
		{
			if (this->characters[i] == entity)
			{
				this->characters[i] = this->characters[this->characters.size() - 1];
				this->characters.pop_back();

				return;
			}
		}
	}

	void RemoveTerrain(Entity* entity) // DOAR ELIMINA DIN MANAGER. ARE OVERHEAD DE MEMORIE DACA NU STERGEM ENTITATEA MANUAL
	{
		for (int i = 0; i < this->terrains.size(); i++)
		{
			if (this->terrains[i] == entity)
			{
				this->terrains[i] = this->terrains[this->terrains.size() - 1];
				this->terrains.pop_back();

				return;
			}
		}
	}

	void RemoveBullet(Entity* entity)
	{
		for (int i = 0; i < this->bullets.size(); i++)
		{
			if (this->bullets[i] == entity)
			{
				this->bullets[i] = this->bullets[this->bullets.size() - 1];
				this->bullets.pop_back();

				return;
			}
		}
	}

	void RemoveAnimatedEntity(Entity* entity)
	{
		for (int i = 0; i < this->animatedEntities.size(); i++)
		{
			if (this->animatedEntities[i] == entity)
			{
				this->animatedEntities[i] = this->animatedEntities[this->animatedEntities.size() - 1];
				this->animatedEntities.pop_back();

				return;
			}
		}
	}

	void RemoveArtificialIntelligence(Entity* entity)
	{
		for (int i = 0; i < this->artificalIntelligence.size(); i++)
		{
			if (this->artificalIntelligence[i] == entity)
			{
				this->artificalIntelligence[i] = this->artificalIntelligence[this->artificalIntelligence.size() - 1];
				this->artificalIntelligence.pop_back();

				return;
			}
		}
	}

	void RemoveCharacter(int index)
	{
		if (0 <= index && index <= this->characters.size())
		{
			this->characters[index] = this->characters[this->characters.size() - 1];
			this->characters.pop_back();
		}
	}

	void RemoveTerrain(int index)
	{
		if (0 <= index && index <= this->terrains.size())
		{
			this->terrains[index] = this->terrains[this->terrains.size() - 1];
			this->terrains.pop_back();
		}
	}

	void RemoveBullet(int index)
	{
		if (0 <= index && index <= this->bullets.size())
		{
			this->bullets[index] = this->bullets[this->bullets.size() - 1];
			this->bullets.pop_back();
		}
	}

	void RemoveAnimatedEntity(int index)
	{
		if (0 <= index && index <= this->animatedEntities.size())
		{
			this->animatedEntities[index] = this->animatedEntities[this->animatedEntities.size() - 1];
			this->animatedEntities.pop_back();
		}
	}

	void RemoveArtificialIntelligence(int index)
	{
		if (0 <= index && index <= this->artificalIntelligence.size())
		{
			this->artificalIntelligence[index] = this->artificalIntelligence[this->artificalIntelligence.size() - 1];
			this->artificalIntelligence.pop_back();
		}
	}

	void RemoveAllEntities() // DOAR ELIMINA DIN MANAGER. ARE OVERHEAD DE MEMORIE DACA NU STERGEM ENTITATEA MANUAL
	{
		this->characters.clear();
		this->terrains.clear();
		this->bullets.clear();

		this->animatedEntities.clear();
	}

	std::vector<Entity*> characters;
	std::vector<Entity*> terrains;
	std::vector<Entity*> bullets;

	std::vector<Entity*> animatedEntities;

	std::vector<Entity*> artificalIntelligence;

private:

	static EntityManager* instance;

	EntityManager() {};

	EntityManager(const EntityManager&) = delete;
};

EntityManager* EntityManager::instance = nullptr;

// STRUCTURA TEXTURA

struct Texture
{
	unsigned int ID = 0;

	int width = 0;
	int height = 0;

	int nrChannels = 0;
};

// CLASA ASSET MANAGER

class AssetManager
{
public:

	static AssetManager* Get()
	{
		if (AssetManager::instance == nullptr)
		{
			AssetManager::instance = new AssetManager();
		}

		return AssetManager::instance;
	}

	void ClearTextureData()
	{
		this->textures.clear();
	}

	void AddTexture(std::string name, std::string address)
	{
		Texture texture;

		glGenTextures(1, &texture.ID);
		glBindTexture(GL_TEXTURE_2D, texture.ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		unsigned char* data = SOIL_load_image(address.c_str(), &texture.width, &texture.height, &texture.nrChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			this->textures.insert({ name, texture });
		}
		else
		{
			std::cout << "ERROR :: ASSET MANAGER :: ADDTEXTURE :: COULD NOT FIND THE TEXTURED NAMED \"" << name << "\" AT THE ADDRESS \"" << address << "\"\n";
		}

		SOIL_free_image_data(data);
	}

	void DeleteTexture(std::string name)
	{
		this->textures.erase(name);
	}

	unsigned int GetTextureID(std::string name)
	{
		if (this->textures.find(name) == this->textures.end())
		{
			std::cout << "ERROR :: ASSET MANAGER :: GETTEXTUREID :: COULD NOT FIND THE TEXTURE NAMED \"" << name << "\". RETURNING ERROR TEXTURE ID INSTEAD...\n";

			return this->errorTexture.ID;
		}

		return this->textures[name].ID;

	}

	unsigned int GetErrorTextureID()
	{
		return this->errorTexture.ID;
	}

private:

	static AssetManager* instance;

	Texture errorTexture;
	std::unordered_map<std::string, Texture> textures;

	std::string errorTextureAddress = "GameEngine/Textures/ErrorTexture/ErrorTexture.png";

	AssetManager()
	{
		glGenTextures(1, &this->errorTexture.ID);
		glBindTexture(GL_TEXTURE_2D, this->errorTexture.ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		unsigned char* data = SOIL_load_image(this->errorTextureAddress.c_str(), &this->errorTexture.width, &this->errorTexture.height, &this->errorTexture.nrChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->errorTexture.width, this->errorTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "ERROR :: ASSET MANAGER :: COULD NOT FIND THE ERROR TEXTURE AT THE ADDRESS \"" << this->errorTextureAddress << "\"\n";
		}

		SOIL_free_image_data(data);
	};

	AssetManager(const AssetManager&) = delete;
};

AssetManager* AssetManager::instance = nullptr;

// CLASA RENDERER2D

class Renderer2D
{
public:

	Renderer2D(std::string vertexShaderAddress, std::string fragmentShaderAddress)
	{

		std::ifstream fileInput;
		std::string line;

		fileInput.open(vertexShaderAddress);

		if (fileInput)
		{
			while (!fileInput.eof())
			{
				getline(fileInput, line);

				this->vertexShaderCode += line;
				this->vertexShaderCode += "\n";
			}

			this->vertexShaderCode += "\0";
		}
		else
		{
			std::cout << "ERROR :: RENDERER2D :: COULD NOT FIND ANY FILE AT THE ADDRESS \"" << vertexShaderAddress << "\" WHICH CONTAINS THE VERTEX SHADER CODE\n";
		}

		fileInput.close();

		fileInput.open(fragmentShaderAddress);

		if (fileInput)
		{
			while (!fileInput.eof())
			{
				getline(fileInput, line);

				this->fragmentShaderCode += line;
				this->fragmentShaderCode += "\n";
			}

			this->fragmentShaderCode += "\0";
		}
		else
		{
			std::cout << "ERROR :: RENDERER2D :: COULD NOT FIND ANY FILE AT THE ADDRESS \"" << fragmentShaderAddress << "\" WHICH CONTAINS THE FRAGMENT SHADER CODE\n";
		}

		fileInput.close();

		const char* vertexCstr = (this->vertexShaderCode).c_str();
		const char* fragmentCstr = (this->fragmentShaderCode).c_str();

		this->vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		this->fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(this->vertexShaderID, 1, &vertexCstr, 0);
		glShaderSource(this->fragmentShaderID, 1, &fragmentCstr, 0);

		glCompileShader(this->vertexShaderID);
		glCompileShader(this->fragmentShaderID);

		this->programID = glCreateProgram();
		glAttachShader(this->programID, this->vertexShaderID);
		glAttachShader(this->programID, this->fragmentShaderID);

		glLinkProgram(this->programID);

		//glValidateProgram(this->programID);

		glUseProgram(this->programID);

		glGenVertexArrays(1, &this->VAOID);
		glGenBuffers(1, &this->VBOID);

		glBindVertexArray(this->VAOID);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBOID);

		glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 4 * sizeof(double), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, 4 * sizeof(double), (void*)(2 * sizeof(double)));
		glEnableVertexAttribArray(1);

		this->ortho = glm::ortho(0.0, 1.0 * WindowManager::Get()->GetWindowWidth(), 0.0, 1.0 * WindowManager::Get()->GetWindowHeight());
		this->orthoPath = glGetUniformLocation(this->programID, "ortho");
		glUniformMatrix4fv(this->orthoPath, 1, GL_FALSE, glm::value_ptr(this->ortho));

		this->texturePath = glGetUniformLocation(this->programID, "texture0");
		glUniform1i(this->texturePath, 0);

		glActiveTexture(GL_TEXTURE0);

		glUseProgram(0);
	}

	~Renderer2D()
	{
		glDeleteBuffers(1, &this->VBOID);
		glDeleteVertexArrays(1, &this->VAOID);

		glDeleteShader(this->vertexShaderID);
		glDeleteShader(this->fragmentShaderID);

		glDeleteProgram(this->programID);
	}

	inline void StartUsing()
	{
		glUseProgram(this->programID);
	}

	inline void StopUsing()
	{
		glUseProgram(0);
	}

	unsigned int GetVAOID()
	{
		return this->VAOID;
	}

	unsigned int GetVBOID()
	{
		return this->VBOID;
	}

private:

	int programID;

	int vertexShaderID;
	int fragmentShaderID;

	std::string vertexShaderCode;
	std::string fragmentShaderCode;

	int orthoPath;
	glm::mat4 ortho;

	int texturePath;

	unsigned int VAOID;
	unsigned int VBOID;
};

// CLASA COMPONENT

class Component
{
public:

	Component() {};
	virtual ~Component() {};

	Entity* entity = nullptr;

private:

};

// CLASA ENTITY

class Entity
{
public:

	Entity(EntityType entityType, bool isAnimated, bool isArtificialIntelligence) : entityType(entityType), isAnimated(isAnimated), isArtificialIntelligence(isArtificialIntelligence)
	{
		for (int i = 0; i < MAX_COMPONENTS; i++)
		{
			this->componentBitset[i] = false;
			this->componentList[i] = nullptr;
		}

		if (this->entityType == EntityType::character)
		{
			EntityManager::Get()->AddCharacter(this);
		}
		else if (this->entityType == EntityType::terrain)
		{
			EntityManager::Get()->AddTerrain(this);
		}
		else if (this->entityType == EntityType::bullet)
		{
			EntityManager::Get()->AddBullet(this);
		}

		if (this->isAnimated)
		{
			EntityManager::Get()->AddAnimatedEntity(this);
		}

		if (this->isArtificialIntelligence)
		{
			EntityManager::Get()->AddArtificialIntelligence(this);
		}
	}

	virtual ~Entity()
	{
		for (int i = 0; i < MAX_COMPONENTS; i++)
		{
			delete this->componentList[i];
			this->componentBitset[i] = false;
		}

		if (entityType == EntityType::character)
		{
			EntityManager::Get()->RemoveCharacter(this);
		}
		else if (entityType == EntityType::terrain)
		{
			EntityManager::Get()->RemoveTerrain(this);
		}
		else if (entityType == EntityType::bullet)
		{
			EntityManager::Get()->RemoveBullet(this);
		}

		if (this->isAnimated)
		{
			EntityManager::Get()->RemoveAnimatedEntity(this);
		}

		if (this->isArtificialIntelligence)
		{
			EntityManager::Get()->RemoveArtificialIntelligence(this);
		}
	}

	template<typename T>
	inline void AddComponent(T* component)
	{
		this->componentBitset[GetComponentTypeID<T>()] = true;
		this->componentList[GetComponentTypeID<T>()] = component;

		component->entity = this;
	}

	template<typename T>
	inline void DeleteComponent()
	{
		if (this->componentBitset[GetComponentTypeID<T>()])
		{
			delete this->componentList[GetComponentTypeID<T>()];

			this->componentBitset[GetComponentTypeID<T>()] = false;
			this->componentList[GetComponentTypeID<T>()] = nullptr;
		}
		else
		{
			std::cout << "WARNING :: ENTITY :: DELETECOMPONENT :: COULD NOT DELETE A COMPONENT WHICH DID NOT EXIST IN THE FIRST PLACE\n";
		}
	}

	template<typename T>
	inline T* GetComponent() const
	{
		if (!this->componentBitset[GetComponentTypeID<T>()])
		{
			std::cout << "WARNING :: ENTITY :: GETCOMPONENT :: UNCREATED COMPONENT WAS DEMANDED. RETURNING NULL POINTER...\n";

			return nullptr;
		}

		return static_cast<T*>(this->componentList[GetComponentTypeID<T>()]);
	}

	template<typename T>
	inline bool HasComponent() const
	{

		return this->componentBitset[GetComponentTypeID<T>()];
	}

	EntityType GetEntityType()
	{
		return this->entityType;
	}

private:

	EntityType entityType;
	bool isAnimated;
	bool isArtificialIntelligence;

	ComponentBitset componentBitset;
	ComponentList componentList;
};

// CLASELE COMPONENTELOR CE SE MOSTENESC DIN COMPONENTA DE BAZA, VIRTUALA
// 2D
class Position2D : public Component
{
public:

	Position2D(double x, double y) : x(x), y(y) {};

	double x;
	double y;

private:

};

class Hitbox2D : public Component
{
public:

	Hitbox2D(double width, double height) : width(width), height(height)
	{
		this->collidedDownward = false;/////////////////////////////////////////////////////////////////////////////////
	}

	double width;
	double height;

	bool collidedDownward;////////////////////////////////////////////////////////////////////////////////////////////////

private:

};

class Texture2D : public Component
{
public:

	Texture2D() {};

	unsigned int currentTextureID = 0;

private:

};

class TextureBox2D : public Component
{
public:

	TextureBox2D(double width, double height) : width(width), height(height) {};

	double width;
	double height;

private:

};

class Render2D : public Component
{
public:

	Render2D() {};

	void Render(Renderer2D* renderer2D)
	{
		if (!this->entity->HasComponent<Position2D>())
		{
			std::cout << "ERROR :: RENDER2DCOMPONENT :: THERE IS NO POSITION2D COMPONENT FOR ENTITY. RETURNING WITHOUT RENDERING...\n";

			return;
		}
		if (!this->entity->HasComponent<TextureBox2D>())
		{
			std::cout << "ERROR :: RENDER2DCOMPONENT :: THERE IS NO TEXTUREBOX2D COMPONENT FOR ENTITY. RETURNING WITHOUT RENDERING...\n";

			return;
		}
		if (!this->entity->HasComponent<Texture2D>())
		{
			std::cout << "ERROR :: RENDER2DCOMPONENT :: THERE IS NO TEXTURE2D COMPONENT FOR ENTITY. RETURNING WITHOUT RENDERING...\n";

			return;
		}

		if (this->entity->GetComponent<Texture2D>()->currentTextureID == 0)
		{
			std::cout << "WARNING :: RENDER2DCOMPONENT :: THERE IS NO TEXTURE TO USE FOR RENDERING. SELECTING THE ERROR TEXTURE INSTEAD AND ATTEMPTING RENDERING...\n";

			entity->GetComponent<Texture2D>()->currentTextureID = AssetManager::Get()->GetErrorTextureID();
		}

		data.clear();

		// TEXTURILE AU ORIGINEA O(0, 0) IN COLTUL DE SUS DIN STANGA

		/*
		6-------3/4
		|        |
		|        |
		|        |
		1/5------2
		*/

		if (!Player::Get()->ShouldCameraFollowPlayer())
		{
			data.push_back(entity->GetComponent<Position2D>()->x - entity->GetComponent<TextureBox2D>()->width / 2.0);
			data.push_back(entity->GetComponent<Position2D>()->y - entity->GetComponent<TextureBox2D>()->height / 2.0);

			data.push_back(0.0);
			data.push_back(1.0);

			data.push_back(entity->GetComponent<Position2D>()->x + entity->GetComponent<TextureBox2D>()->width / 2.0);
			data.push_back(entity->GetComponent<Position2D>()->y - entity->GetComponent<TextureBox2D>()->height / 2.0);

			data.push_back(1.0);
			data.push_back(1.0);

			data.push_back(entity->GetComponent<Position2D>()->x + entity->GetComponent<TextureBox2D>()->width / 2.0);
			data.push_back(entity->GetComponent<Position2D>()->y + entity->GetComponent<TextureBox2D>()->height / 2.0);

			data.push_back(1.0);
			data.push_back(0.0);

			data.push_back(entity->GetComponent<Position2D>()->x + entity->GetComponent<TextureBox2D>()->width / 2.0);
			data.push_back(entity->GetComponent<Position2D>()->y + entity->GetComponent<TextureBox2D>()->height / 2.0);

			data.push_back(1.0);
			data.push_back(0.0);

			data.push_back(entity->GetComponent<Position2D>()->x - entity->GetComponent<TextureBox2D>()->width / 2.0);
			data.push_back(entity->GetComponent<Position2D>()->y - entity->GetComponent<TextureBox2D>()->height / 2.0);

			data.push_back(0.0);
			data.push_back(1.0);

			data.push_back(entity->GetComponent<Position2D>()->x - entity->GetComponent<TextureBox2D>()->width / 2.0);
			data.push_back(entity->GetComponent<Position2D>()->y + entity->GetComponent<TextureBox2D>()->height / 2.0);

			data.push_back(0.0);
			data.push_back(0.0);

			renderer2D->StartUsing();

			glBindVertexArray(renderer2D->GetVAOID());
			glBindBuffer(GL_ARRAY_BUFFER, renderer2D->GetVBOID());

			glBindTexture(GL_TEXTURE_2D, entity->GetComponent<Texture2D>()->currentTextureID);

			glBufferData(GL_ARRAY_BUFFER, sizeof(double) * data.size(), &data[0], GL_DYNAMIC_DRAW);

			glDrawArrays(GL_TRIANGLES, 0, data.size() / 4);
		}
		else if (Player::Get()->GetEntity() != nullptr && Player::Get()->GetEntity()->HasComponent<Position2D>())
		{
			data.push_back(entity->GetComponent<Position2D>()->x - entity->GetComponent<TextureBox2D>()->width / 2.0 - Player::Get()->GetEntity()->GetComponent<Position2D>()->x + WindowManager::Get()->GetWindowWidth() / 2.0);
			data.push_back(entity->GetComponent<Position2D>()->y - entity->GetComponent<TextureBox2D>()->height / 2.0 - Player::Get()->GetEntity()->GetComponent<Position2D>()->y + WindowManager::Get()->GetWindowHeight() / 2.0);

			data.push_back(0.0);
			data.push_back(1.0);

			data.push_back(entity->GetComponent<Position2D>()->x + entity->GetComponent<TextureBox2D>()->width / 2.0 - Player::Get()->GetEntity()->GetComponent<Position2D>()->x + WindowManager::Get()->GetWindowWidth() / 2.0);
			data.push_back(entity->GetComponent<Position2D>()->y - entity->GetComponent<TextureBox2D>()->height / 2.0 - Player::Get()->GetEntity()->GetComponent<Position2D>()->y + WindowManager::Get()->GetWindowHeight() / 2.0);

			data.push_back(1.0);
			data.push_back(1.0);

			data.push_back(entity->GetComponent<Position2D>()->x + entity->GetComponent<TextureBox2D>()->width / 2.0 - Player::Get()->GetEntity()->GetComponent<Position2D>()->x + WindowManager::Get()->GetWindowWidth() / 2.0);
			data.push_back(entity->GetComponent<Position2D>()->y + entity->GetComponent<TextureBox2D>()->height / 2.0 - Player::Get()->GetEntity()->GetComponent<Position2D>()->y + WindowManager::Get()->GetWindowHeight() / 2.0);

			data.push_back(1.0);
			data.push_back(0.0);

			data.push_back(entity->GetComponent<Position2D>()->x + entity->GetComponent<TextureBox2D>()->width / 2.0 - Player::Get()->GetEntity()->GetComponent<Position2D>()->x + WindowManager::Get()->GetWindowWidth() / 2.0);
			data.push_back(entity->GetComponent<Position2D>()->y + entity->GetComponent<TextureBox2D>()->height / 2.0 - Player::Get()->GetEntity()->GetComponent<Position2D>()->y + WindowManager::Get()->GetWindowHeight() / 2.0);

			data.push_back(1.0);
			data.push_back(0.0);

			data.push_back(entity->GetComponent<Position2D>()->x - entity->GetComponent<TextureBox2D>()->width / 2.0 - Player::Get()->GetEntity()->GetComponent<Position2D>()->x + WindowManager::Get()->GetWindowWidth() / 2.0);
			data.push_back(entity->GetComponent<Position2D>()->y - entity->GetComponent<TextureBox2D>()->height / 2.0 - Player::Get()->GetEntity()->GetComponent<Position2D>()->y + WindowManager::Get()->GetWindowHeight() / 2.0);

			data.push_back(0.0);
			data.push_back(1.0);

			data.push_back(entity->GetComponent<Position2D>()->x - entity->GetComponent<TextureBox2D>()->width / 2.0 - Player::Get()->GetEntity()->GetComponent<Position2D>()->x + WindowManager::Get()->GetWindowWidth() / 2.0);
			data.push_back(entity->GetComponent<Position2D>()->y + entity->GetComponent<TextureBox2D>()->height / 2.0 - Player::Get()->GetEntity()->GetComponent<Position2D>()->y + WindowManager::Get()->GetWindowHeight() / 2.0);

			data.push_back(0.0);
			data.push_back(0.0);

			renderer2D->StartUsing();

			glBindVertexArray(renderer2D->GetVAOID());
			glBindBuffer(GL_ARRAY_BUFFER, renderer2D->GetVBOID());

			glBindTexture(GL_TEXTURE_2D, entity->GetComponent<Texture2D>()->currentTextureID);

			glBufferData(GL_ARRAY_BUFFER, sizeof(double) * data.size(), &data[0], GL_DYNAMIC_DRAW);

			glDrawArrays(GL_TRIANGLES, 0, data.size() / 4);
		}
	}

private:

	std::vector<double> data;

};

class Speed2D : public Component
{
public:

	Speed2D(double speedX = 0.0, double speedY = 0.0) : speedX(speedX), speedY(speedY) {};

	double speedX;
	double speedY;

private:

};

class Acceleration2D : public Component
{
public:

	Acceleration2D(double accelerationX = 0.0, double accelerationY = 0.0) : accelerationX(accelerationX), accelerationY(accelerationY) {};

	double accelerationX;
	double accelerationY;

private:

};

class Gravity2D : public Component
{
public:

	Gravity2D(double value = 10.0) : value(value) {};

	double value;

private:

};

class Animation2D : public Component
{
public:

	Animation2D(double frameTime = 0.1) : frameTime(frameTime)
	{
		this->currentEntityAnimation = EntityAnimation::null;

		this->lastEntityAnimation = EntityAnimation::null;

		this->timeWhenCurrentTextureSelected = 0.0;
	}

	std::vector<unsigned int> animations[MAX_ANIMATIONS_ENTITY];

	int animationIndex = 0;

	EntityAnimation currentEntityAnimation;
	EntityAnimation lastEntityAnimation;

	double frameTime;
	double timeWhenCurrentTextureSelected;

private:

};

class MovementSpeed2D : public Component
{
public:

	MovementSpeed2D(double speedX = 0.0, double speedY = 0.0) : speedX(speedX), speedY(speedY)
	{
		this->wentRight = false;
		this->wentLeft = false;
	}

	double speedX;
	double speedY;

	bool wentRight;
	bool wentLeft;

private:

};

class ArtificialIntelligence2D : public Component
{
public:

	ArtificialIntelligence2D(ArtificialIntelligence state) : state(state) {};

	ArtificialIntelligence state;

private:

};

// CLASA USER INPUT MANAGER

class UserInputManager
{
public:

	static UserInputManager* Get()
	{
		if (UserInputManager::instance == nullptr)
		{
			UserInputManager::instance = new UserInputManager();
		}

		return UserInputManager::instance;
	}

	void ListenForInput()
	{
		if (glfwGetKey(WindowManager::Get()->GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			this->gameEngineShouldStop = true;
		}

		if (Player::Get()->GetEntity() == nullptr)
		{
			std::cout << "WARNING :: USERINPUTMANAGER :: LISTENFORINPUT :: PLAYER ENTITY HAS NOT BEEN SET YET\n";

			return;
		}

		if (Player::Get()->GetEntity()->HasComponent<Speed2D>() && Player::Get()->GetEntity()->HasComponent<MovementSpeed2D>())
		{

			if (glfwGetKey(WindowManager::Get()->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
			{
				if (!Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->wentRight)
				{
					Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->wentRight = true;

					Player::Get()->GetEntity()->GetComponent<Speed2D>()->speedX += Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->speedX;
				}
			}
			else if (Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->wentRight)
			{
				Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->wentRight = false;

				Player::Get()->GetEntity()->GetComponent<Speed2D>()->speedX -= Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->speedX;
			}

			if (glfwGetKey(WindowManager::Get()->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
			{
				if (!Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->wentLeft)
				{
					Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->wentLeft = true;

					Player::Get()->GetEntity()->GetComponent<Speed2D>()->speedX -= Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->speedX;
				}
			}
			else if (Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->wentLeft)
			{
				Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->wentLeft = false;

				Player::Get()->GetEntity()->GetComponent<Speed2D>()->speedX += Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->speedX;
			}

			if (glfwGetKey(WindowManager::Get()->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
			{
				if (Player::Get()->GetEntity()->HasComponent<Hitbox2D>())
				{
					if (Player::Get()->GetEntity()->GetComponent<Hitbox2D>()->collidedDownward)
					{
						Player::Get()->GetEntity()->GetComponent<Hitbox2D>()->collidedDownward = false;

						Player::Get()->GetEntity()->GetComponent<Speed2D>()->speedY += Player::Get()->GetEntity()->GetComponent<MovementSpeed2D>()->speedY;
					}
				}
			}
		}
	}

	bool ShouldGameEngineStop()
	{
		return this->gameEngineShouldStop;
	}

private:

	static UserInputManager* instance;

	bool gameEngineShouldStop;

	UserInputManager()
	{
		this->gameEngineShouldStop = false;
	}

	UserInputManager(const UserInputManager&) = delete;
};

UserInputManager* UserInputManager::instance = nullptr;

// CLASA MOVEMENT MANAGER

class MovementManager
{
public:

	static MovementManager* Get()
	{
		if (MovementManager::instance == nullptr)
		{
			MovementManager::instance = new MovementManager();
		}

		return MovementManager::instance;
	}

	void UpdateMovements()
	{
		for (int i = 0; i < EntityManager::Get()->characters.size(); i++)
		{
			if (EntityManager::Get()->characters[i]->HasComponent<Speed2D>())
			{
				if (EntityManager::Get()->characters[i]->HasComponent<Gravity2D>())
				{
					EntityManager::Get()->characters[i]->GetComponent<Speed2D>()->speedY += EntityManager::Get()->characters[i]->GetComponent<Gravity2D>()->value * TimeManager::Get()->GetDeltaTime();
				}
				if (EntityManager::Get()->characters[i]->HasComponent<Acceleration2D>())
				{
					EntityManager::Get()->characters[i]->GetComponent<Speed2D>()->speedX += EntityManager::Get()->characters[i]->GetComponent<Acceleration2D>()->accelerationX * TimeManager::Get()->GetDeltaTime();
					EntityManager::Get()->characters[i]->GetComponent<Speed2D>()->speedY += EntityManager::Get()->characters[i]->GetComponent<Acceleration2D>()->accelerationY * TimeManager::Get()->GetDeltaTime();
				}

				if (EntityManager::Get()->characters[i]->HasComponent<Position2D>())
				{

					EntityManager::Get()->characters[i]->GetComponent<Position2D>()->x += EntityManager::Get()->characters[i]->GetComponent<Speed2D>()->speedX * TimeManager::Get()->GetDeltaTime();
					EntityManager::Get()->characters[i]->GetComponent<Position2D>()->y += EntityManager::Get()->characters[i]->GetComponent<Speed2D>()->speedY * TimeManager::Get()->GetDeltaTime();
				}
			}
		}

		for (int i = 0; i < EntityManager::Get()->terrains.size(); i++)
		{
			if (EntityManager::Get()->terrains[i]->HasComponent<Speed2D>())
			{
				if (EntityManager::Get()->terrains[i]->HasComponent<Position2D>())
				{

					EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->x += EntityManager::Get()->terrains[i]->GetComponent<Speed2D>()->speedX * TimeManager::Get()->GetDeltaTime();
					EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->y += EntityManager::Get()->terrains[i]->GetComponent<Speed2D>()->speedY * TimeManager::Get()->GetDeltaTime();
				}

				if (EntityManager::Get()->terrains[i]->HasComponent<Gravity2D>())
				{
					EntityManager::Get()->terrains[i]->GetComponent<Speed2D>()->speedY += EntityManager::Get()->terrains[i]->GetComponent<Gravity2D>()->value * TimeManager::Get()->GetDeltaTime();
				}
				if (EntityManager::Get()->terrains[i]->HasComponent<Acceleration2D>())
				{
					EntityManager::Get()->terrains[i]->GetComponent<Speed2D>()->speedX += EntityManager::Get()->terrains[i]->GetComponent<Acceleration2D>()->accelerationX * TimeManager::Get()->GetDeltaTime();
					EntityManager::Get()->terrains[i]->GetComponent<Speed2D>()->speedY += EntityManager::Get()->terrains[i]->GetComponent<Acceleration2D>()->accelerationY * TimeManager::Get()->GetDeltaTime();
				}
			}
		}

		for (int i = 0; i < EntityManager::Get()->bullets.size(); i++)
		{
			if (EntityManager::Get()->bullets[i]->HasComponent<Speed2D>())
			{
				if (EntityManager::Get()->bullets[i]->HasComponent<Position2D>())
				{

					EntityManager::Get()->bullets[i]->GetComponent<Position2D>()->x += EntityManager::Get()->bullets[i]->GetComponent<Speed2D>()->speedX * TimeManager::Get()->GetDeltaTime();
					EntityManager::Get()->bullets[i]->GetComponent<Position2D>()->y += EntityManager::Get()->bullets[i]->GetComponent<Speed2D>()->speedY * TimeManager::Get()->GetDeltaTime();
				}

				if (EntityManager::Get()->bullets[i]->HasComponent<Gravity2D>())
				{
					EntityManager::Get()->bullets[i]->GetComponent<Speed2D>()->speedY += EntityManager::Get()->bullets[i]->GetComponent<Gravity2D>()->value * TimeManager::Get()->GetDeltaTime();
				}
				if (EntityManager::Get()->bullets[i]->HasComponent<Acceleration2D>())
				{
					EntityManager::Get()->bullets[i]->GetComponent<Speed2D>()->speedX += EntityManager::Get()->bullets[i]->GetComponent<Acceleration2D>()->accelerationX * TimeManager::Get()->GetDeltaTime();
					EntityManager::Get()->bullets[i]->GetComponent<Speed2D>()->speedY += EntityManager::Get()->bullets[i]->GetComponent<Acceleration2D>()->accelerationY * TimeManager::Get()->GetDeltaTime();
				}
			}
		}
	}

private:

	static MovementManager* instance;

	MovementManager() {};

	MovementManager(const MovementManager&) = delete;

};

MovementManager* MovementManager::instance = nullptr;

// CLASA COLLISION MANAGER

class CollisionManager
{
public:

	static CollisionManager* Get()
	{
		if (CollisionManager::instance == nullptr)
		{
			CollisionManager::instance = new CollisionManager();
		}

		return CollisionManager::instance;
	}

	void UpdateCollisions()
	{
		for (int i = 0; i < EntityManager::Get()->characters.size(); i++)
		{

			if (!EntityManager::Get()->characters[i]->HasComponent<Position2D>() || !EntityManager::Get()->characters[i]->HasComponent<Hitbox2D>()) continue;

			for (int j = 0; j < EntityManager::Get()->bullets.size(); j++)
			{
				if (!EntityManager::Get()->bullets[j]->HasComponent<Position2D>() || !EntityManager::Get()->bullets[j]->HasComponent<Hitbox2D>()) continue;

				if (std::max(EntityManager::Get()->characters[i]->GetComponent<Position2D>()->x - EntityManager::Get()->characters[i]->GetComponent<Hitbox2D>()->width / 2.0, EntityManager::Get()->bullets[j]->GetComponent<Position2D>()->x - EntityManager::Get()->bullets[j]->GetComponent<Hitbox2D>()->width / 2.0) <=
					std::min(EntityManager::Get()->characters[i]->GetComponent<Position2D>()->x + EntityManager::Get()->characters[i]->GetComponent<Hitbox2D>()->width / 2.0, EntityManager::Get()->bullets[j]->GetComponent<Position2D>()->x + EntityManager::Get()->bullets[j]->GetComponent<Hitbox2D>()->width / 2.0) &&
					std::max(EntityManager::Get()->characters[i]->GetComponent<Position2D>()->y - EntityManager::Get()->characters[i]->GetComponent<Hitbox2D>()->height / 2.0, EntityManager::Get()->bullets[j]->GetComponent<Position2D>()->y - EntityManager::Get()->bullets[j]->GetComponent<Hitbox2D>()->height / 2.0) <=
					std::min(EntityManager::Get()->characters[i]->GetComponent<Position2D>()->y + EntityManager::Get()->characters[i]->GetComponent<Hitbox2D>()->height / 2.0, EntityManager::Get()->bullets[j]->GetComponent<Position2D>()->y + EntityManager::Get()->bullets[j]->GetComponent<Hitbox2D>()->height / 2.0))
				{
					EntityManager::Get()->RemoveBullet(j);
					j--;
				}
			}
		}

		for (int i = 0; i < EntityManager::Get()->characters.size(); i++)
		{
			if (EntityManager::Get()->characters[i]->HasComponent<Hitbox2D>())
			{
				EntityManager::Get()->characters[i]->GetComponent<Hitbox2D>()->collidedDownward = false;
			}
		}

		for (int i = 0; i < EntityManager::Get()->terrains.size(); i++)
		{
			if (!EntityManager::Get()->terrains[i]->HasComponent<Position2D>() || !EntityManager::Get()->terrains[i]->HasComponent<Hitbox2D>()) continue;

			for (int j = 0; j < EntityManager::Get()->characters.size(); j++)
			{
				if (!EntityManager::Get()->characters[j]->HasComponent<Position2D>() || !EntityManager::Get()->characters[j]->HasComponent<Hitbox2D>()) continue;

				if (std::max(EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->x - EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->width / 2.0, EntityManager::Get()->characters[j]->GetComponent<Position2D>()->x - EntityManager::Get()->characters[j]->GetComponent<Hitbox2D>()->width / 2.0) <=
					std::min(EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->x + EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->width / 2.0, EntityManager::Get()->characters[j]->GetComponent<Position2D>()->x + EntityManager::Get()->characters[j]->GetComponent<Hitbox2D>()->width / 2.0) &&
					std::max(EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->y - EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->height / 2.0, EntityManager::Get()->characters[j]->GetComponent<Position2D>()->y - EntityManager::Get()->characters[j]->GetComponent<Hitbox2D>()->height / 2.0) <=
					std::min(EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->y + EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->height / 2.0, EntityManager::Get()->characters[j]->GetComponent<Position2D>()->y + EntityManager::Get()->characters[j]->GetComponent<Hitbox2D>()->height / 2.0))
				{
					// COLIZIUNE CU REZOLVARE PE AXA OX
					if (EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->width / 2.0 + EntityManager::Get()->characters[j]->GetComponent<Hitbox2D>()->width / 2.0 - std::abs(EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->x - EntityManager::Get()->characters[j]->GetComponent<Position2D>()->x) < EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->height / 2.0 + EntityManager::Get()->characters[j]->GetComponent<Hitbox2D>()->height / 2.0 - std::abs(EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->y - EntityManager::Get()->characters[j]->GetComponent<Position2D>()->y))
					{
						if (EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->x < EntityManager::Get()->characters[j]->GetComponent<Position2D>()->x)
						{
							EntityManager::Get()->characters[j]->GetComponent<Position2D>()->x += EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->width / 2.0 + EntityManager::Get()->characters[j]->GetComponent<Hitbox2D>()->width / 2.0 - (EntityManager::Get()->characters[j]->GetComponent<Position2D>()->x - EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->x);

							if (EntityManager::Get()->characters[j]->HasComponent<MovementSpeed2D>())
							{
								if (EntityManager::Get()->characters[j]->GetComponent<MovementSpeed2D>()->wentLeft)
								{
									EntityManager::Get()->characters[j]->GetComponent<MovementSpeed2D>()->wentLeft = false;
								}
							}

							if (EntityManager::Get()->characters[j]->HasComponent<Speed2D>())
							{
								if (EntityManager::Get()->characters[j]->GetComponent<Speed2D>()->speedX < 0.0)
								{
									EntityManager::Get()->characters[j]->GetComponent<Speed2D>()->speedX = 0.0;
								}
							}
						}
						else
						{
							EntityManager::Get()->characters[j]->GetComponent<Position2D>()->x -= EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->width / 2.0 + EntityManager::Get()->characters[j]->GetComponent<Hitbox2D>()->width / 2.0 - (EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->x - EntityManager::Get()->characters[j]->GetComponent<Position2D>()->x);

							if (EntityManager::Get()->characters[j]->HasComponent<MovementSpeed2D>())
							{
								if (EntityManager::Get()->characters[j]->GetComponent<MovementSpeed2D>()->wentRight)
								{
									EntityManager::Get()->characters[j]->GetComponent<MovementSpeed2D>()->wentRight = false;
								}
							}

							if (EntityManager::Get()->characters[j]->HasComponent<Speed2D>())
							{
								if (EntityManager::Get()->characters[j]->GetComponent<Speed2D>()->speedX > 0.0)
								{
									EntityManager::Get()->characters[j]->GetComponent<Speed2D>()->speedX = 0.0;
								}
							}
						}
					}
					// PE AXA OY
					else
					{
						if (EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->y < EntityManager::Get()->characters[j]->GetComponent<Position2D>()->y)
						{
							EntityManager::Get()->characters[j]->GetComponent<Position2D>()->y += EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->height / 2.0 + EntityManager::Get()->characters[j]->GetComponent<Hitbox2D>()->height / 2.0 - (EntityManager::Get()->characters[j]->GetComponent<Position2D>()->y - EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->y);

							EntityManager::Get()->characters[j]->GetComponent<Hitbox2D>()->collidedDownward = true;

							if (EntityManager::Get()->characters[j]->HasComponent<Speed2D>())
							{
								if (EntityManager::Get()->characters[j]->GetComponent<Speed2D>()->speedY < 0.0)
								{
									EntityManager::Get()->characters[j]->GetComponent<Speed2D>()->speedY = 0.0;
								}
							}
						}
						else
						{
							EntityManager::Get()->characters[j]->GetComponent<Position2D>()->y -= EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->height / 2.0 + EntityManager::Get()->characters[j]->GetComponent<Hitbox2D>()->height / 2.0 - (EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->y - EntityManager::Get()->characters[j]->GetComponent<Position2D>()->y);

							if (EntityManager::Get()->characters[j]->HasComponent<Speed2D>())
							{
								if (EntityManager::Get()->characters[j]->GetComponent<Speed2D>()->speedY > 0.0)
								{
									EntityManager::Get()->characters[j]->GetComponent<Speed2D>()->speedY = 0.0;
								}
							}
						}
					}

				}
			}

			for (int j = 0; j < EntityManager::Get()->bullets.size(); j++)
			{
				if (!EntityManager::Get()->bullets[j]->HasComponent<Position2D>() || !EntityManager::Get()->bullets[j]->HasComponent<Hitbox2D>()) continue;

				if (std::max(EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->x - EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->width / 2.0, EntityManager::Get()->bullets[j]->GetComponent<Position2D>()->x - EntityManager::Get()->bullets[j]->GetComponent<Hitbox2D>()->width / 2.0) <=
					std::min(EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->x + EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->width / 2.0, EntityManager::Get()->bullets[j]->GetComponent<Position2D>()->x + EntityManager::Get()->bullets[j]->GetComponent<Hitbox2D>()->width / 2.0) &&
					std::max(EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->y - EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->height / 2.0, EntityManager::Get()->bullets[j]->GetComponent<Position2D>()->y - EntityManager::Get()->bullets[j]->GetComponent<Hitbox2D>()->height / 2.0) <=
					std::min(EntityManager::Get()->terrains[i]->GetComponent<Position2D>()->y + EntityManager::Get()->terrains[i]->GetComponent<Hitbox2D>()->height / 2.0, EntityManager::Get()->bullets[j]->GetComponent<Position2D>()->y + EntityManager::Get()->bullets[j]->GetComponent<Hitbox2D>()->height / 2.0))
				{
					EntityManager::Get()->RemoveBullet(j);
					j--;
				}
			}
		}
	}

private:

	static CollisionManager* instance;

	CollisionManager() {};

	CollisionManager(const CollisionManager&) = delete;

};

CollisionManager* CollisionManager::instance = nullptr;

// CLASA ANIMATION MANAGER

class AnimationManager
{
public:

	static AnimationManager* Get()
	{
		if (AnimationManager::instance == nullptr)
		{
			AnimationManager::instance = new AnimationManager();
		}

		return AnimationManager::instance;
	}

	void UpdateAnimations()
	{
		for (int i = 0; i < EntityManager::Get()->animatedEntities.size(); i++)
		{
			if (EntityManager::Get()->animatedEntities[i]->HasComponent<Speed2D>() && EntityManager::Get()->animatedEntities[i]->HasComponent<Texture2D>() && EntityManager::Get()->animatedEntities[i]->HasComponent<Animation2D>())
			{
				EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->lastEntityAnimation = EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->currentEntityAnimation;

				if (EntityManager::Get()->animatedEntities[i]->GetComponent<Speed2D>()->speedY == 0.0)
				{
					if (EntityManager::Get()->animatedEntities[i]->GetComponent<Speed2D>()->speedX == 0.0)
					{
						EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->currentEntityAnimation = EntityAnimation::standing;
					}
					else if (EntityManager::Get()->animatedEntities[i]->GetComponent<Speed2D>()->speedX > 0.0)
					{
						EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->currentEntityAnimation = EntityAnimation::walkingRight;
					}
					else
					{
						EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->currentEntityAnimation = EntityAnimation::walkingLeft;
					}
				}
				else if (EntityManager::Get()->animatedEntities[i]->GetComponent<Speed2D>()->speedY > 0.0)
				{
					if (EntityManager::Get()->animatedEntities[i]->GetComponent<Speed2D>()->speedX >= 0.0)
					{
						EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->currentEntityAnimation = EntityAnimation::jumpingRight;
					}
					else
					{
						EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->currentEntityAnimation = EntityAnimation::jumpingLeft;
					}
				}
				else
				{
					if (EntityManager::Get()->animatedEntities[i]->GetComponent<Speed2D>()->speedX >= 0.0)
					{
						EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->currentEntityAnimation = EntityAnimation::fallingRight;
					}
					else
					{
						EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->currentEntityAnimation = EntityAnimation::fallingLeft;
					}
				}
			}

			if (EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->currentEntityAnimation == EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->lastEntityAnimation)
			{
				if (TimeManager::Get()->GetCurrentTime() - EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->timeWhenCurrentTextureSelected >= EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->frameTime)
				{
					EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->animationIndex++;

					if (EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->animationIndex >= EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->animations[(int)EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->currentEntityAnimation].size())
					{
						EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->animationIndex = 0;
					}

					EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->timeWhenCurrentTextureSelected = TimeManager::Get()->GetCurrentTime();
					EntityManager::Get()->animatedEntities[i]->GetComponent<Texture2D>()->currentTextureID = EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->animations[(int)EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->currentEntityAnimation][EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->animationIndex];
				}
			}
			else
			{
				EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->animationIndex = 0;
				EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->timeWhenCurrentTextureSelected = TimeManager::Get()->GetCurrentTime();
				EntityManager::Get()->animatedEntities[i]->GetComponent<Texture2D>()->currentTextureID = EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->animations[(int)EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->currentEntityAnimation][EntityManager::Get()->animatedEntities[i]->GetComponent<Animation2D>()->animationIndex];

			}
		}
	}

private:

	static AnimationManager* instance;

	AnimationManager() {};

	AnimationManager(const AnimationManager&) = delete;
};

AnimationManager* AnimationManager::instance = nullptr;

// CLASA ARTIFICIAL INTELLIGENCE MANAGER

class ArtificialIntelligenceManager
{
public:

	static ArtificialIntelligenceManager* Get()
	{
		if (ArtificialIntelligenceManager::instance == nullptr)
		{
			ArtificialIntelligenceManager::instance = new ArtificialIntelligenceManager();
		}

		return ArtificialIntelligenceManager::instance;
	}

	void UpdateArtificialIntelligence()
	{
		if (Player::Get()->GetEntity() != nullptr) // DACA EXISTA JUCATORUL
		{
			if (Player::Get()->GetEntity()->HasComponent<Position2D>())
			{
				for (int i = 0; i < EntityManager::Get()->artificalIntelligence.size(); i++)
				{
					if (EntityManager::Get()->artificalIntelligence[i]->HasComponent<Position2D>() && EntityManager::Get()->artificalIntelligence[i]->HasComponent<Speed2D>() && EntityManager::Get()->artificalIntelligence[i]->HasComponent<MovementSpeed2D>() && EntityManager::Get()->artificalIntelligence[i]->HasComponent<ArtificialIntelligence2D>())
					{
						///////////////////////////////////////////////////////////////////////////////////////////////////////////////

						if (EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->wentRight)
						{
							EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->wentRight = false;

							EntityManager::Get()->artificalIntelligence[i]->GetComponent<Speed2D>()->speedX -= EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->speedX;
						}

						if (EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->wentLeft)
						{
							EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->wentLeft = false;

							EntityManager::Get()->artificalIntelligence[i]->GetComponent<Speed2D>()->speedX += EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->speedX;
						}

						// DACA ENTITATEA ESTE AGRESIVA FATA DE JUCATOR
						if (EntityManager::Get()->artificalIntelligence[i]->GetComponent<ArtificialIntelligence2D>()->state == ArtificialIntelligence::aggresive)
						{
							if (Player::Get()->GetEntity()->GetComponent<Position2D>()->x < EntityManager::Get()->artificalIntelligence[i]->GetComponent<Position2D>()->x - AI::EPSILON_X_AGGRESIVE)
							{

								EntityManager::Get()->artificalIntelligence[i]->GetComponent<Speed2D>()->speedX -= EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->speedX;

								EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->wentLeft = true;

								///*
								if (EntityManager::Get()->artificalIntelligence[i]->GetComponent<Hitbox2D>()->collidedDownward)
								{
									EntityManager::Get()->artificalIntelligence[i]->GetComponent<Hitbox2D>()->collidedDownward = false;

									EntityManager::Get()->artificalIntelligence[i]->GetComponent<Speed2D>()->speedY += EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->speedY;
								}
								//*/
							}

							if (Player::Get()->GetEntity()->GetComponent<Position2D>()->x > EntityManager::Get()->artificalIntelligence[i]->GetComponent<Position2D>()->x + AI::EPSILON_X_AGGRESIVE)
							{

								EntityManager::Get()->artificalIntelligence[i]->GetComponent<Speed2D>()->speedX += EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->speedX;

								EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->wentRight = true;

								///*
								if (EntityManager::Get()->artificalIntelligence[i]->GetComponent<Hitbox2D>()->collidedDownward)
								{
									EntityManager::Get()->artificalIntelligence[i]->GetComponent<Hitbox2D>()->collidedDownward = false;

									EntityManager::Get()->artificalIntelligence[i]->GetComponent<Speed2D>()->speedY += EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->speedY;
								}
								//*/
							}

							if (EntityManager::Get()->artificalIntelligence[i]->GetComponent<Hitbox2D>()->collidedDownward && Player::Get()->GetEntity()->GetComponent<Position2D>()->y > EntityManager::Get()->artificalIntelligence[i]->GetComponent<Position2D>()->y + AI::EPSILON_Y_AGGRESIVE)
							{
								EntityManager::Get()->artificalIntelligence[i]->GetComponent<Hitbox2D>()->collidedDownward = false;

								EntityManager::Get()->artificalIntelligence[i]->GetComponent<Speed2D>()->speedY += EntityManager::Get()->artificalIntelligence[i]->GetComponent<MovementSpeed2D>()->speedY;
							}
						}
					}
				}
			}
		}
	}

private:

	static ArtificialIntelligenceManager* instance;

	ArtificialIntelligenceManager() {};

	ArtificialIntelligenceManager(const ArtificialIntelligenceManager&) = delete;
};

ArtificialIntelligenceManager* ArtificialIntelligenceManager::instance = nullptr;

// CLASA GAME ENGINE

class GameEngine
{
public:

	static GameEngine* Get()
	{
		if (GameEngine::instance == nullptr)
		{
			GameEngine::instance = new GameEngine();
		}

		return GameEngine::instance;
	}

	void Start(int windowWidth, int windowHeight, std::string gameTitle)
	{
		WindowManager::Get()->CreateWindow(windowWidth, windowHeight, gameTitle);
		TimeManager::Get();

		//AssetManager::Get();
	}

	void Update()
	{
		WindowManager::Get()->UpdateWindow();
		TimeManager::Get()->UpdateDeltaTime();
		UserInputManager::Get()->ListenForInput();

		MovementManager::Get()->UpdateMovements();
		CollisionManager::Get()->UpdateCollisions();

		AnimationManager::Get()->UpdateAnimations();

		ArtificialIntelligenceManager::Get()->UpdateArtificialIntelligence();

		if (UserInputManager::Get()->ShouldGameEngineStop())
		{
			this->isRunning = false;
		}
	}

	void Stop()
	{
		WindowManager::Get()->SetWindowShouldClose(true);

		WindowManager::Get()->DestroyWindow();
		EntityManager::Get()->RemoveAllEntities();

		this->isRunning = false;
	}

	bool ShouldGameEngineStop()
	{
		return !isRunning;
	}

private:

	static GameEngine* instance;

	bool isRunning;

	GameEngine()
	{
		this->isRunning = true;
	}

	GameEngine(const GameEngine&) = delete;
};

GameEngine* GameEngine::instance = nullptr;