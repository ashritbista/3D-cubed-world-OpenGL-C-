#include <iostream>
#include<vector>
#include<time.h>

// GLEW
#define GLEW_STATIC
#include <GLEW/GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
//Simple OpenGL Image Library
#include <SOIL/soil.h>

//AntTweakBar
#include <ATB/AntTweakBar.h>

//glm library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//other includes
#include "Shader.h"

//keyboard input recognition function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

//do movement when the key is pressed
void do_movement();

//mouse input recognition function
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//mouse scroll callback function
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//mouse button callback function
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

const GLuint WIDTH = 800, HEIGHT = 600;
GLfloat mixValue = 0.2f;
//mouse position in the last frame
GLfloat lastX = 400, lastY = 300, pitch = 0.0f, yaw = -90.0f;
GLfloat fov = 45.0f;

//camera positions
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//key pressed array 
bool keys[1024];

bool firstMouse = true;
bool buttonPressed = false;

//camera velocity
GLfloat deltaTime = 0.0f; //Time between current frame and last frame
GLfloat lastFrame = 0.0f; //time of last frame

//global rotation and scaling variable for anttweak bar
GLfloat rotation = 20.0f;
GLfloat scale = 1.0f;
GLuint numberOfCubes = 1;

//AntTweakBar
TwBar *bar;

int main()
{
	//initiate GLFW
	glfwInit();
	//set all the options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//create a GLFW window object to be used for GLFW functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Learn OpenGL!!", nullptr,
		nullptr);

	//print error if window cannot be created
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//set window as your current workspace context
	glfwMakeContextCurrent(window);

	//set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//set this to true so that GLEW uses modern approach to retrieve
	//function pointers and extensions
	glewExperimental = GL_TRUE;

	//initiate GLEW
	glewInit();

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	//initiate random number generator
	srand(time(NULL));

	//initiate AntTweakBar
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WIDTH, HEIGHT);

	//create an AntTweakBar
	bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar label='Main' ");

	TwAddVarRW(bar, "scale", TW_TYPE_FLOAT, &scale, " label= 'size' min= 0.1 max= 3.0 step= 0.1 help= 'alter cube size' ");
	TwAddVarRW(bar, "rotation", TW_TYPE_FLOAT, &rotation, " label= 'rotation speed' min = 0.0 max= 500.0 step= 1 help= 'alters cubes rotation' ");
	TwAddVarRW(bar, "add/remove ", TW_TYPE_INT32, &numberOfCubes, " label= 'add/remove' min= 1 max= 20 step= 1 ");


	glViewport(0, 0, WIDTH, HEIGHT);

	Shader ourShader("C:\\Build\\Projects\\camera\\Shaders\\shader.vs", "C:\\Build\\Projects\\camera\\Shaders\\shader.frag");


	//vertices for the cube with wood texture and smiley face
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	GLuint indices[] =
	{
		0, 1, 3, //First triangle 
		1, 2, 3  //Second triangle
	};

	//vbo, vao and texture binding for the cube
	GLuint VBO1, VAO1, EBO;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glGenBuffers(1, &EBO);

	//Bind the Vertex Array Object(VAO) first, then bind and set vertex
	//buffers and attribute pointers
	glBindVertexArray(VAO1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Texture Coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);//unbind VAO1

						 //load and create textures
	GLuint texture1, texture2;

	//for texture 1
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); // all 2D operations now affect the texture object

											//set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set the filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//load the image and create a texture
	int width, height;
	unsigned char* image = SOIL_load_image("C:\\Build\\Projects\\camera\\Textures\\container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); //unbind the texture when done

									 //for texture 2
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	//set the texture parameter for the second texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set the filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//load the second image and create a texture
	image = SOIL_load_image("C:\\Build\\Projects\\camera\\Textures\\awesomeface.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	//enable depth testing
	glEnable(GL_DEPTH_TEST);

	//set the mouse capture so mouse doesn't exit the glfw window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	std::vector<glm::vec3> cubePos;

	cubePos.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

	//game loop
	while (!glfwWindowShouldClose(window))
	{

		GLfloat x = 0.0f, y = 0.0f, z = 0.0f;
		// Check if any events have been activiated 
		//(key pressed, mouse moved etc.) and call 
		//corresponding response functions
		glfwPollEvents();

		//move accordingly when a key is pressed
		do_movement();

		//calculate frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//render and clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (numberOfCubes <= 20 && buttonPressed)
		{
			x = rand() % 3;
			y = rand() % 3;
			z = rand() % 3;

			for (int index = 0; index < cubePos.size(); index++)
			{
				if (x == cubePos[index].x || x <= cubePos[index].x - 1.0 || x >= cubePos[index].x + 1.0)
					x -=   1.0;

				if (y == cubePos[index].y || y <= cubePos[index].y - 0.5 || y >= cubePos[index].y + 0.5)
					y -=  1.0;

				if (z == cubePos[index].z || z <= cubePos[index].z - 0.5 || z >= cubePos[index].z + 0.5)
					z += 1.0;
			}

			cubePos.push_back(glm::vec3(x, y, z));
		}

		//model matrix for 3D drawing
		glm::mat4 model;

		//veiw matrix for 3D drawing
		glm::mat4 view;
		//we want to translate the scane in the reverse direction of where we want to move
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

		//using lookAt matrix change the view of camera using glfwGetTime() function
		GLfloat radius = 10.0f;
		GLfloat camX = sin(glfwGetTime()) * radius;
		GLfloat camZ = cos(glfwGetTime()) * radius;

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		//projection matrix for 3D drawing
		GLfloat aspect = (GLfloat)WIDTH / (GLfloat)HEIGHT;
		glm::mat4 projection;
		projection = glm::perspective(fov, aspect, 0.1f, 100.0f);

		//activate shaders
		ourShader.Use();

		//bind textures using texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture2"), 1);
		glUniform1f(glGetUniformLocation(ourShader.Program, "mixValue"), mixValue);

		//transformation for mode, view and projection below

		//location for model matrix in vertex shader
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//location for the view matrix in the vertex shader
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		//location for the view matrix in the vertex shader
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//draw container
		glBindVertexArray(VAO1);
		for (GLuint i = 0; i < numberOfCubes; i++)
		{
			glm::mat4 model;
			model = glm::translate(model, cubePos[i]);
			GLfloat angle;
			angle = glfwGetTime() * rotation;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			model = glm::scale(model, glm::vec3(scale));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		//draw the AntTweakBar
		TwDraw();


		//swap the screen buffers
		glfwSwapBuffers(window);
	}

	//properly de-allocate all the resources once they've been used up
	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
		keys[key] = true;
	
	else if (action == GLFW_RELEASE)
		keys[key] = false;
	// When a user presses the escape key, we set the WindowShouldClose
	//property to true,
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);	
}

//movement functions when key is pressed
void do_movement()
{
	GLfloat cameraSpeed = 5.0f * deltaTime;

	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;

	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;

	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}


bool leftClick;
bool rightClick;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	buttonPressed = leftClick = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
	rightClick = button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS;

	TwEventMouseButtonGLFW(button, action);
}

//mosue callback function
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.05;	// Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	if (leftClick)
	{

	}

	else if (rightClick)
	{
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
	}

	TwEventMousePosGLFW(xpos, ypos);
}

//scroll callback function
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}
