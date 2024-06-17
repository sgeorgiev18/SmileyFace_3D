// SDL_OGL1.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

//#define GLEW_STATIC
#include <GL\glew.h>

#include <iostream>
#include <thread>

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <gl\GLU.h>
#define STB_IMAGE_IMPLEMENTATION //if not defined the function implementations are not included
#include "stb_image.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

bool init();
bool initGL();
void render();
bool LoadTexture(const char* filename, GLuint& texID);
GLuint CreateCube(float, GLuint&, GLuint&);
void DrawCube(GLuint id);
void close();

// Global variables
SDL_Window* gWindow = NULL;
SDL_GLContext gContext;
Shader shader;
GLuint gVAO, gVBO, gEBO;
GLuint texID1;
float cubePosX = 0.0f, cubePosY = 0.0f, cubePosZ = 0.0f;

void HandleKeyUp(const SDL_KeyboardEvent& key);


int main(int argc, char* args[])
{
	init();
	SDL_Event e;
	bool quit = false;

	
	//SDL_GL_SwapWindow(gWindow);
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			switch (e.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
				else {
					HandleKeyUp(e.key);
				}
				break;
			}
			render();
		}	
		
		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	close();
	return 0;
}


void HandleKeyUp(const SDL_KeyboardEvent& key)
{
	//render();
	//SDL_GL_SwapWindow(gWindow);
	switch (key.keysym.sym)
	{
	case SDLK_UP:
		cubePosY += 1.1f;
		break;
	case SDLK_DOWN:
		cubePosY -= 1.1f;
		break;
	case SDLK_LEFT:
		cubePosX -= 1.1f;
		break;
	case SDLK_RIGHT:
		cubePosX += 1.1f;
		break;
	}
}

bool init()
{
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}
				if (!initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
			}
		}
	}
	return success;
}

bool initGL()
{
	bool success = true;
	GLenum error = GL_NO_ERROR;
	glewInit();
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		success = false;
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (!LoadTexture("smiley.jpg", texID1))
	{
		printf("Could not load texture \"smiley.jpg\"");
	}
	shader.Load(".\\shaders\\vertex.vs", ".\\shaders\\fragment.fs");
	shader.use();
	shader.setInt("Texture1", 0);
	gVAO = CreateCube(1.0f, gVBO, gEBO);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	return success;
}

bool LoadTexture(const char* filename, GLuint& texID)
{
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLint width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* img_data = stbi_load(filename, &width, &height, &channels, 0);
	if (img_data)
	{
		GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, img_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("Failed to load texture\n");
	}
	stbi_image_free(img_data);
	return true;
}

void close()
{
	glDeleteProgram(shader.ID);
	glDeleteVertexArrays(1, &gVAO);
	glDeleteBuffers(1, &gVBO);
	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(cubePosX, cubePosY, cubePosZ));

	glm::mat4 view = glm::mat4(2.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);

	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID1);
	DrawCube(gVAO);

	SDL_GL_SwapWindow(gWindow);
}

GLuint CreateCube(float width, GLuint& VBO, GLuint& EBO)
{
	GLfloat vertices[] = {
		// positions          // colors           // texture coords
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,   0.0f, 1.0f
	};

	GLuint indices[] = {
		0, 1, 3, 1, 2, 3, // back face
		4, 5, 7, 5, 6, 7, // front face
		4, 5, 0, 5, 1, 0, // bottom face
		7, 6, 3, 6, 2, 3, // top face
		4, 0, 7, 0, 3, 7, // left face
		5, 1, 6, 1, 2, 6  // right face
	};

	GLuint VAO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

void DrawCube(GLuint VAO)
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


