#include <iostream>
#include <math.h>
#include <time.h>

#include "includes.h"

#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "camera.h"
#include "arcball_camera.h"
#include "mesh.h"
#include "drawable_light.h"
#include "texture.h"

#include "stb_image.h"

#include <GLFW/glfw3.h>

bool first_mouse_movement = true;
float delta_time;
float last_change;

int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;

float last_x = SCR_WIDTH / 2;
float last_y = SCR_HEIGHT / 2;

float fov = 45.0f;

float pitch = 0.0f;
float yaw = -90.0f;

Camera_Mode camera_mode = ARCBALL;
Shader_Mode shader_mode = ECEF;

int lod = 0;
int lod_change = 0;
int lod_auto = 1;

ArcballCamera arcball_camera(glm::vec3(3.0, 12.0, 8.0), glm::vec3(0, 0, 0), glm::vec3(0.0, 1.0, 0.0), 90.0f, 0.0f);
Camera free_camera(glm::vec3(45.0, 20.0, 10.0), glm::vec3(1.0, 0.0, 0.0));

void windowResizeHandler(GLFWwindow *window, int width, int height);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);
void processInput(GLFWwindow *window);

int main(int argc, char *argv[])
{
	// Initialize GLFW
	glewExperimental = true; // Needed for core profile
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);			   // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		   // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGl

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "heightmap"
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Heightmap-OpenGL", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetWindowSizeCallback(window, framebuffer_size_callback);
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hides mouse cursor and captures it
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the player_camera than the former one
	glDepthFunc(GL_LESS);

	/**************************
	 * Create all objects
	 **************************/

	Shader shader("shaders/simple.vert", "shaders/simple.frag");

	std::vector<Texture> plank_textures = {
		Texture("data/textures/planks.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE)
	};

	std::vector<Texture> metal_textures = {
		Texture("data/textures/green_metal_rust_diff_1k.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("data/textures/green_metal_rust_spec_1k.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

	std::vector<Texture> plank_textures_spec = {
		Texture("data/textures/planks.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("data/textures/planksSpec.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};


	Mesh torus("data/models/torus.obj", "shaders/simple.vert", "shaders/simple.frag", plank_textures_spec);
	Mesh teapot("data/models/teapot_tri.obj", "shaders/simple.vert", "shaders/simple.frag", plank_textures_spec);
	Mesh wooden_floor("data/models/plane.obj", "shaders/simple.vert", "shaders/simple.frag", plank_textures_spec);
	Mesh wooden_floor_spec("data/models/plane.obj", "shaders/simple.vert", "shaders/simple.frag", metal_textures);

	DrawableLight pointlight("data/models/sphere.obj", "shaders/pointlight.vert", "shaders/pointlight.frag", glm::vec3(7, 4, 5));

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 MVP_matrix = glm::mat4(1.0f);

	int mvp_location = glGetUniformLocation(shader.id, "mvp");

	float rotation = 0.0f;
	double prev_time = glfwGetTime();

	// float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	unsigned int counter = 0;
	// glfwSwapInterval(0); // disables VSync -- unlimited FPS

	float theta = 0.0f;
	float phi = 0.0f;

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
	{

		float currentFrame = glfwGetTime();
		delta_time = currentFrame - lastFrame;
		last_change += delta_time;
		counter++;

		float fps = (1.0 / delta_time) * counter;

		if (delta_time >= 1.0 / 30.0)
		{
			std::string FPS = std::to_string(fps);
			std::string ms = std::to_string((delta_time / counter) * 1000);
			std::string newTitle = "Heightmap-OpenGL - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());
			counter = 0;
			lastFrame = currentFrame;
		}

		theta += delta_time;
		phi += delta_time;

		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.07f, 0.07f, 0.07f, 1.0f);

		view = arcball_camera.getViewMatrix();
		projection = glm::perspective(glm::radians(arcball_camera.zoom), (float)(SCR_WIDTH) / (float)SCR_HEIGHT, 0.01f, 100.0f);

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		/**************************
		 * Draw all objects
		 **************************/
		// torus.shader.Activate();
		// torus.Bind();
		// torus.Draw(&model, &view, &projection, TRIANGLES, false, arcball_camera.pos);
		// torus.Unbind();

		// plank_floor.Draw(plank_floor.shader, free_camera, model, view, projection);


		float radius = 4.0f;
		pointlight.position.x = cos(theta/2) * radius;
		pointlight.position.z = sin(phi/2) * radius;

		MVP_matrix = projection * view * model;
		torus.Draw(arcball_camera, MVP_matrix, model, pointlight);

		glm::mat4 model_teapot = glm::translate(model, glm::vec3(-5.0, 0.0, 0.0));
		MVP_matrix = projection * view * model_teapot;
		teapot.Draw(arcball_camera, MVP_matrix, model_teapot, pointlight);

		glm::mat4 model_light = glm::translate(model, pointlight.position);
		MVP_matrix = projection * view * model_light;
		pointlight.Draw(arcball_camera, MVP_matrix);

		glm::mat4 model_floor_spec = glm::translate(model, glm::vec3(0, -2, 0));
		MVP_matrix = projection * view * model_floor_spec;
		wooden_floor_spec.Draw(arcball_camera, MVP_matrix, model_floor_spec, pointlight);



		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// vao.Delete();
	// vbo.Delete();
	// ebo.Delete();
	// planks.Delete();
	// shader.Delete();

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}

void windowResizeHandler(GLFWwindow *window, int width, int height)
{
	const float aspectRatio = (float)width / height;
	float sx = aspectRatio > 1.0f ? aspectRatio : 1.0f;
	float sy = aspectRatio > 1.0f ? 1.0f : 1.0f / aspectRatio;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-sx, sx, -sy, sy, 1, -1);

	glViewport(0, 0, width, height);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos)
{
	if (first_mouse_movement)
	{
		last_x = x_pos;
		last_y = y_pos;
		first_mouse_movement = false;
	}

	float x_offset = x_pos - last_x;
	float y_offset = last_y - y_pos;

	last_x = x_pos;
	last_y = y_pos;

	free_camera.processMouseMovement(x_offset, y_offset, true);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		arcball_camera.processMouseRotation(-x_offset, y_offset, SCR_WIDTH, SCR_HEIGHT);
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
		arcball_camera.processMouseTilt(x_offset, y_offset);
}

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset)
{
	arcball_camera.processMouseScroll(y_offset);
	free_camera.processMouseScroll(y_offset);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		lod = 1;
		lod_change = 1;
		lod_auto = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		lod = 2;
		lod_change = 1;
		lod_auto = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		lod = 3;
		lod_change = 1;
		lod_auto = 0;
	}

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		lod = 4;
		lod_change = 1;
		lod_auto = 0;
	}

	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		lod = 5;
		lod_change = 1;
		lod_auto = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
	{
		lod = 6;
		lod_change = 1;
		lod_auto = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
	{
		lod = 7;
		lod_change = 1;
		lod_auto = 0;
	}

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
		lod = 4;
		lod_change = 1;
		lod_auto = 1;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		arcball_camera.processMouseRotation(0.0f, 5.0f, SCR_WIDTH, SCR_HEIGHT);
		free_camera.processKeyboard(FORWARD, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		arcball_camera.processMouseRotation(0.0f, -5.0f, SCR_WIDTH, SCR_HEIGHT);
		free_camera.processKeyboard(BACKWARD, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		arcball_camera.processMouseRotation(-5.0f, 0.0f, SCR_WIDTH, SCR_HEIGHT);
		free_camera.processKeyboard(LEFT, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		arcball_camera.processMouseRotation(5.0f, 0.0f, SCR_WIDTH, SCR_HEIGHT);
		free_camera.processKeyboard(RIGHT, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		arcball_camera.processKeyboard(UP, delta_time);
		free_camera.processKeyboard(UP, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		arcball_camera.processKeyboard(DOWN, delta_time);
		free_camera.processKeyboard(DOWN, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		arcball_camera.processMouseRotation(0.0, 10., SCR_WIDTH, SCR_HEIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		arcball_camera.processMouseRotation(0.0, -10, SCR_WIDTH, SCR_HEIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		arcball_camera.processMouseRotation(-10.0, 0.0, SCR_WIDTH, SCR_HEIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		arcball_camera.processMouseRotation(10.0, 0.0, SCR_WIDTH, SCR_HEIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && last_change > 0.5)
	{
		if (camera_mode == ARCBALL)
			camera_mode = FREE;
		else
			camera_mode = ARCBALL;
		last_change = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && last_change > 0.5)
	{
		if (shader_mode == ECEF)
			shader_mode = LLA;
		else
			shader_mode = ECEF;
		last_change = 0;
	}
}