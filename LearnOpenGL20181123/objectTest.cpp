#include "stdafx.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <Shader\shader.h>
#include <stb_image.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <MyClass\camera.h>
#include <MyClass/object/object3D.h>
#include <MyClass/object/object3Dcube.h>
#include <MyClass/object/object3Dcylinder.h>
#include <MyClass/object/object3Dsphere.h>
#include <MyClass/object/Collision.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float mouseSensitivity = 0.1f;
float scrollSensitivity = 0.35f;
float keySensitivity = 0.25f;
Camera camera(glm::vec3(0.0f, 0.0f, 15.0f));	// (eyeX, eyeY, eyeZ)


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float collideDelta = 0.0f;
const float collideCD = 0.02f;
bool collideCDing = false;

// lighting
glm::vec3 lightPos;
float lightR = 4.0f;

int main()
{
	// set up window creation hints
	// ----------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		// Uncomment this if Mac OS X

	// create a window
	// ---------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL20181123", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// initialize glad
	// ---------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// print out version number, must be after glad loading
	std::cout << glGetString(GL_VERSION) << std::endl;


	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	// ------------------------------------
	Shader lightingShader("shaders/test/colorsVS.glsl", "shaders/test/colorsFS.glsl");
	Shader lampShader("shaders/test/lampVS.glsl", "shaders/test/lampFS.glsl");

	// ------------------------------------
	camera.SetPerspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


	// -- Objects --
	// cube
	Object3Dcube cube(glm::vec3(1.0f, 1.0f, 1.0f));
	cube.SetPosition(glm::vec3(4.0f, 0.3f, 0.0f));
	cube.AddTexture("resources/textures/awesomeface.png", ObjectTextureType::Emission);
	cube.SetMass(5.0f);
	cube.SetVelocity(glm::vec3(-1.5f, 0.0f, 0.0f));
	cube.SetOmega(glm::vec3(0.0f, 5.0f, 0.0f));
	// cylin
	Object3Dcylinder cylin = Object3Dcylinder(0.5f, 1.0f, 2.0f, 16, 2);
	cylin.SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
	//cylin.AddTexture("resources/textures/awesomeface.png", ObjectTextureType::Emission);
	cylin.AddTexture("resources/textures/matrix.jpg", ObjectTextureType::Emission);
	// ball
	Object3Dsphere ball = Object3Dsphere(0.5f, 32, 20);
	ball.AddTexture("resources/textures/awesomeface.png", ObjectTextureType::Emission);
	ball.SetPosition(glm::vec3(-5.0f, 0.0f, -2.0f));
	ball.SetVelocity(glm:: vec3(0));
	ball.SetMass(0.0f);
	ball.SetFriction(2.0f);
	ball.SetERestitution(1.0f);
	ball.SetAmbient(glm::vec3(1.0f, 0.5f, 0.31f));
	ball.SetDiffuse(glm::vec3(1.0f, 0.5f, 0.31f));
	ball.SetSpecular(glm::vec3(5.0f, 0.5f, 0.5f));
	ball.SetShininess(32.0f);
	ball.SetOmega(glm::vec3(0.0f, 5.0f, 0.0f));
	// ball2
	Object3Dsphere ball2 = Object3Dsphere(1.0f, 32, 20);
	ball2.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	ball2.SetVelocity(glm::vec3(0.8f, 0, 0));
	ball2.SetMass(1.0f);
	ball2.SetFriction(2.0f);
	//ball2.AddTexture("resources/textures/wood.png", ObjectTextureType::Emission);
	ball2.AddTexture("resources/textures/awesomeface.png", ObjectTextureType::Emission);
	//ball2.SetOmega(glm::vec3(2.0f, 2.0f, 2.0f));
	// ball3
	Object3Dsphere ball3 = Object3Dsphere(0.4f, 32, 20);
	ball3.SetPosition(glm::vec3(0.0f, 2.4f, 1.2f));
	ball3.SetVelocity(glm::vec3(0.0f, -0.5f, -0.25f));
	ball3.SetMass(0.1f);
	ball3.AddTexture("resources/textures/bricks2.jpg", ObjectTextureType::Ambient);

	// point light 1
	lightingShader.use();		// don't forget to do this !!!!!!!!
	lightingShader.setBool("pointLights[0].isExist", true);
	lightingShader.setFloat("pointLights[0].constant", 1.0f);
	lightingShader.setFloat("pointLights[0].linear", 0.09);
	//lightingShader.setFloat("pointLights[0].linear", 0.22);
	lightingShader.setFloat("pointLights[0].quadratic", 0.032);
	//lightingShader.setFloat("pointLights[0].quadratic", 0.0019);

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		//std::cout << currentFrame << std::endl;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		lightPos = glm::vec3(sin(currentFrame) * lightR, 3.0f, cos(currentFrame) * lightR);
		

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate the shader before setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("viewPos", camera.GetPosition());
		lightingShader.setVec3("pointLights[0].position", lightPos);


		// light properties
		glm::vec3 lightColor;
		lightColor.r = sin(currentFrame * 2.0f);
		lightColor.g = sin(currentFrame * 1.2f);
		lightColor.b = sin(currentFrame * 0.7f);
		glm::vec3 ambientColor = lightColor * glm::vec3(0.1);	// low influence
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5);	// middle influence
		//lightingShader.setVec3("light.ambient", ambientColor);
		//lightingShader.setVec3("light.diffuse", diffuseColor);
		//lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("pointLights[0].ambient", ambientColor);
		lightingShader.setVec3("pointLights[0].diffuse", diffuseColor);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);

		// material properties
		//lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
		//lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		//lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
		//lightingShader.setFloat("material.shininess", 32.0f);


		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		//cube.TestDraw(camera, lightingShader, projection, cubeVAO);
		//cube.Draw(camera, lightingShader);
		//lamp.SetPosition(lightPos);
		//lamp.Draw(camera, lampShader);
		cylin.SetPosition(lightPos); 
		//cylin.Draw(camera, lightingShader);
		//cylin2.Draw(camera, lightingShader);


		// #NOTE �������ָ�룬�ں������޸�ball���ٶȶԺ������ball��Ӱ��
		CollideSph2Sph(&ball, &ball2, true);
		CollideSph2Cube(&ball2, &cube, true);

		//CollideSph2Sph(&ball, &ball3, true);
		//CollideSph2Sph(&ball2, &ball3, true);

		ball.UpdatePosition(deltaTime);
		ball.Draw(camera, lightingShader);
		ball2.UpdatePosition(deltaTime);
		ball2.Draw(camera, lightingShader);
		//ball3.UpdatePosition(deltaTime);
		//cube.AddAngularMomentum(glm::vec3(0.0f, 0.001f, 0.0f));
		//cube.SetAngularMomentum(glm::vec3(0.0f, sin(currentFrame), cos(currentFrame)));
		cube.UpdatePosition(deltaTime);
		cube.Draw(camera, lightingShader);
		//ball3.Draw(camera, lightingShader);




		// glfw: swap buffers and poll IO events
		// -------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clear all allocated resources
	// ----------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.RotateDownByDegree(-keySensitivity);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.RotateDownByDegree(keySensitivity);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.RotateRightByDegree(keySensitivity);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.RotateRightByDegree(-keySensitivity);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

	camera.OrientLeftByDegree(-xoffset * mouseSensitivity);
	camera.OrientUpByDegree(yoffset * mouseSensitivity);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.GoForward(scrollSensitivity * yoffset);
	//camera.Fov += scrollSensitivity * yoffset;
}