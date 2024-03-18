
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;

float deltaTime = 0.0f;	float lastFrame = 0.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader ourShader("VertexShader.txt", "FragmentShader.txt");

	float vertices[] = {
	   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	   
	   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	   -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	   
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	};

	unsigned int indices_cube[] = {
		0, 1, 2, 
		2, 3, 0, 

		4, 5, 6, 
		6, 7, 4, 

		8, 9, 10, 
		10, 11, 8, 

		12, 13, 14, 
		14, 15, 12, 

		16, 17, 18, 
		18, 19, 16, 

		20, 21, 22, 
		22, 23, 20  
	};

	unsigned int CubeTrianglesCount = 36;// 6(вершин на каждой грани) * 6(граней)
	
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_cube), indices_cube, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glm::vec3 cubePositions = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 SpherePositions = glm::vec3(1.5f, 0.0f, 0.0f);

	// Цикл для создания треугольников для шара
	const float radius = 0.5;
	const int sectors = 36;
	const int stacks = 18;
	float* vertices_Sphere = new float[sectors * stacks * 3];
	unsigned int* indices = new unsigned int[sectors * (stacks - 1) * 6];
	float* p = vertices_Sphere;
	unsigned int* index = indices;
	for (int i = 0; i < stacks; ++i)
	{
		float phi = glm::pi<float>() * i / (stacks - 1);
		for (int j = 0; j < sectors; ++j)
		{
			float theta = 2 * glm::pi<float>() * j / (sectors - 1);
			p[0] = cos(theta) * sin(phi) * radius;
			p[1] = cos(phi) * radius;
			p[2] = sin(theta) * sin(phi) * radius;
			p += 3;

			if (i < stacks - 1)
			{
				*index++ = i * sectors + j;
				*index++ = (i + 1) * sectors + j;
				*index++ = i * sectors + j + 1;
				*index++ = i * sectors + j + 1;
				*index++ = (i + 1) * sectors + j;
				*index++ = (i + 1) * sectors + j + 1;
			}
		}
	}

	unsigned int VBO_Sp, VAO_Sp, EBO_Sp;
	glGenVertexArrays(1, &VAO_Sp);
	glGenBuffers(1, &VBO_Sp);
	glGenBuffers(1, &EBO_Sp);

	glBindVertexArray(VAO_Sp);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Sp);
	glBufferData(GL_ARRAY_BUFFER, sectors * stacks * 3 * sizeof(float), vertices_Sphere, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Sp);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sectors * (stacks - 1) * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Texture texture_1("Box.jpg", GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE);
	texture_1.texUnit(ourShader, "texture1", 0);
	Texture texture_2("LittleBox.png", GL_TEXTURE_2D, 1, GL_RGB, GL_UNSIGNED_BYTE);
	texture_2.texUnit(ourShader, "texture2", 1);

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	ourShader.setMat4("projection", projection);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		

		ourShader.use();

		// Создаем преобразование камеры
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		texture_1.Bind();
		texture_2.Bind();


		glBindVertexArray(VAO);


		// Вычисляем матрицу модели для каждого объекта
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions);
		ourShader.setMat4("model", model);

		glDrawElements(GL_TRIANGLES, CubeTrianglesCount, GL_UNSIGNED_INT, 0);

		glBindVertexArray(VAO_Sp);

		glm::mat4 model_Sp = glm::mat4(1.0f);
		model_Sp = glm::translate(model_Sp, SpherePositions);
		ourShader.setMat4("model", model_Sp);

		glDrawElements(GL_TRIANGLES, sectors * (stacks - 1) * 6, GL_UNSIGNED_INT, 0);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VAO_Sp);
	glDeleteBuffers(1, &VBO_Sp);
	glDeleteBuffers(1, &EBO_Sp);
	texture_1.Delete();
	texture_2.Delete();


	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}
