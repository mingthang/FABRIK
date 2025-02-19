#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Salamander/Salamander.h"
#include "IK/FABRIK.h"

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

glm::vec2 mousePos(0.0f, 0.0f);

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Inverse Kinematics", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// Shader
	Shader circleShader("./Sources/Salamander/circle.vs", "./Sources/Salamander/circle.fs");

	// Create Salamander
	Salamander salamander(&circleShader);
	for (int i = 0; i < 17; i++) {
		float y = 0.6f - (i * 0.06f);  
		float width = 1.0f - (i * 0.05f); 
		//std::cout << i << ": " << y << " " << width << std::endl;
		salamander.AddBodyJoint(0.0f, y, width);
	}
	salamander.bodyJoints[0].width = 0.9f;
	salamander.bodyJoints[1].width = 0.5f;
	salamander.AddLeg(1);
	salamander.AddLeg(7);
	salamander.bodyJoints[2].width = 0.7f;

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// render
		glClearColor(0.91f, 0.99f, 0.91f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		salamander.Move(mousePos, deltaTime);
		salamander.Draw();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	float x = (float)xPos / SCR_WIDTH * 2.0f - 1.0f;  // Convert to [-1,1]
	float y = 1.0f - (float)yPos / SCR_HEIGHT * 2.0f; // Convert to [-1,1], invert Y

	mousePos = glm::vec2(x, y);
}
