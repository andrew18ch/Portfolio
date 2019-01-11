
#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")
//load libraries
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/glm/gtx/transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//create function
void errorCallbackGLFW(int error, const char* description);
void hintsGLFW();
void endProgram();
void render(GLfloat currentTime);
void update(GLfloat currentTime);
void setupRender();
void startup();
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void debugGL();
static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam);
string readShader(string name);
void checkErrorShader(GLuint shader);
void readObj(string name, struct modelObject *obj);
void readTexture(string name, GLuint texture);



// VARIABLES
GLFWwindow*		window;
int				windowWidth = 1024;
int				windowHeight = 768;
bool			running = true;
glm::mat4		proj_matrix;
glm::vec3		modelAngle = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3       modelTranslate = glm::vec3(3.0f, 0.0f, 0.0f);
glm::vec3		modelDisp = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3		cameraPosition = glm::vec3(-18.0f, 2.37f, 0.29f);
glm::vec3		cameraFront = glm::vec3(0.0f, 30.0f, -1.0f);
glm::vec3		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float           aspect = (float)windowWidth / (float)windowHeight;
float			fovy = 45.0f;
bool			keyStatus[1024];
bool            anim = false;
bool            otherside = false;
float           trigger_anim = 0.0;
GLfloat			deltaTime = 0.0f;
GLfloat			lastTime = 0.0f;

GLuint			framebuffer;
GLuint			framebufferTexture;
GLuint			depthbuffer;
GLuint			displayVao;
GLuint			displayBuffer[2];
std::vector < glm::vec2 > displayVertices;
std::vector < glm::vec2 > displayUvs;
GLuint			displayProgram;

// FPS camera variables
GLfloat			yaw = 00.0f;	// init pointing to inside
GLfloat			pitch = 0.0f;	// start centered
GLfloat			lastX = (GLfloat)windowWidth / 2.0f;	// start middle screen
GLfloat			lastY = (GLfloat)windowHeight / 2.0f;	// start middle screen
bool			firstMouse = true;

// OBJ Variables
struct modelObject {
	std::vector < glm::vec3 > out_vertices;
	std::vector < glm::vec2 > out_uvs;
	std::vector < glm::vec3 > out_normals;
	GLuint*		texture;
	GLuint      program;
	GLuint      vao;
	GLuint      buffer[2];
	GLint       mv_location;
	GLint       proj_location;
	GLint		tex_location;

	// extra variables 
	GLuint		matColor_location;
	GLuint		lightColor_location;

} floorModel, lightModel, headModel, bedModel, picModel, dollModel;

//vectors
glm::vec3		*modelRotations;

// Light
bool			movingLight = true;
glm::vec3		lightDisp = glm::vec3(-1.0f, -1.0f, 0.0f);
glm::vec3		ia = glm::vec3(0.0f, 0.0f, 0.8f);
GLfloat			ka = 0.05;
glm::vec3		id = glm::vec3(0.93f, 0.75f, 0.32f);
GLfloat			kd = 1.0;
glm::vec3		is = glm::vec3(1.00f, 1.00f, 1.0f);
GLfloat			ks = 0.01;

int main()
{
	if (!glfwInit()) {							// Checking for GLFW
		cout << "Could not initialise GLFW...";
		return 0;
	}

	glfwSetErrorCallback(errorCallbackGLFW);	// Setup a function to catch and display all GLFW errors.

	hintsGLFW();								// Setup glfw with various hints.		

												// Start a window using GLFW
	string title = "My OpenGL Application";


	// Window
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
	if (!window) {								// Window or OpenGL context creation failed
		cout << "Could not initialise GLFW...";
		endProgram();
		return 0;
	}

	glfwMakeContextCurrent(window);				// making the OpenGL context current

												// Start GLEW (note: always initialise GLEW after creating your window context.)
	glewExperimental = GL_TRUE;					// hack: catching them all - forcing newest debug callback (glDebugMessageCallback)
	GLenum errGLEW = glewInit();
	if (GLEW_OK != errGLEW) {					// Problems starting GLEW?
		cout << "Could not initialise GLEW...";
		endProgram();
		return 0;
	}

	debugGL();									// Setup callback to catch openGL errors.	

												// Setup all the message loop callbacks.
	glfwSetWindowSizeCallback(window, onResizeCallback);		// Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);					// Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback);	// Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);		// Set callback for mouse move
	
																//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor. Fullscreen
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS.

	setupRender();								// setup some render variables.
	startup();									// Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	do {										// run until the window is closed
		GLfloat currentTime = (GLfloat)glfwGetTime();		// retrieve timelapse
		deltaTime = currentTime - lastTime;		// Calculate delta time
		lastTime = currentTime;					// Save for next frame calculations.
		//check time to check trigger_anim variable in order to activate the animation
		if (currentTime - trigger_anim >= 2.0) {
			anim = true;
			trigger_anim = currentTime;
			
		}
		else
		{
			anim = false;
			
		}

		glfwPollEvents();						// poll callbacks
		update(currentTime);					// update (physics, animation, structures, etc)
		render(currentTime);					// call render function.

		glfwSwapBuffers(window);				// swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);	// exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram();			// Close and clean everything up...

	cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}

void errorCallbackGLFW(int error, const char* description) {
	cout << "Error GLFW: " << description << "\n";
}

void hintsGLFW() {
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);			// Create context in debug mode - for debug message callback
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void endProgram() {
	glfwMakeContextCurrent(window);		// destroys window handler
	glfwTerminate();	// destroys all windows and releases resources.

						// tidy heap memory
	//delete textures and vector
	delete[] floorModel.texture;
	delete[] lightModel.texture;
	delete[] dollModel.texture;
	delete[] headModel.texture;
	delete[] bedModel.texture;
	delete[] picModel.texture;
	delete[] modelRotations;

}

void setupRender() {
	glfwSwapInterval(1);	// Ony render when synced (V SYNC)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_STEREO, GL_FALSE);
}

void startup() {

	//initialize models shader
	string vs_text = readShader("vs_model.glsl"); static const char* vs_source = vs_text.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	checkErrorShader(vs);

	string fs_text = readShader("fs_model.glsl"); static const char* fs_source = fs_text.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	checkErrorShader(fs);

	// --------------Floor Model---------------------
	floorModel.program = glCreateProgram();

	//attach shaders to model
	glAttachShader(floorModel.program, vs);

	
	glAttachShader(floorModel.program, fs);

	glLinkProgram(floorModel.program);
	glUseProgram(floorModel.program);
	//read object
	readObj("floor.obj", &floorModel);

	glGenVertexArrays(1, &floorModel.vao);			// Create Vertex Array Object
	glBindVertexArray(floorModel.vao);				// Bind VertexArrayObject

	glGenBuffers(3, floorModel.buffer);			// Create new buffers (Vertices, Texture Coordinates, Normals)
	glBindBuffer(GL_ARRAY_BUFFER, floorModel.buffer[0]);	// Bind Buffer Vertex
	glBufferStorage(GL_ARRAY_BUFFER, floorModel.out_vertices.size() * sizeof(glm::vec3), &floorModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, floorModel.buffer[1]);	// Bind Buffer UV
	glBufferStorage(GL_ARRAY_BUFFER, floorModel.out_uvs.size() * sizeof(glm::vec2), &floorModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, floorModel.buffer[2]);	// Bind Buffer Normals
	glBufferStorage(GL_ARRAY_BUFFER, floorModel.out_normals.size() * sizeof(glm::vec3), &floorModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);		// Vertices									
	glBindVertexBuffer(0, floorModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);			// UV									
	glBindVertexBuffer(1, floorModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 0);			// Normals									
	glBindVertexBuffer(2, floorModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(2, 2);
	glEnableVertexAttribArray(2);

	glBindVertexArray(floorModel.vao);				// Bind VertexArrayObject

	floorModel.mv_location = glGetUniformLocation(floorModel.program, "mv_matrix");
	floorModel.proj_location = glGetUniformLocation(floorModel.program, "proj_matrix");
	floorModel.tex_location = glGetUniformLocation(floorModel.program, "tex");
	floorModel.lightColor_location = glGetUniformLocation(floorModel.program, "ia");
	floorModel.lightColor_location = glGetUniformLocation(floorModel.program, "ka");



	//--------------Light Model--------------------------
	lightModel.program = glCreateProgram();
	//create and read shaders for light and attach them to object
	string vs_textLight = readShader("vs_light.glsl"); static const char* vs_sourceLight = vs_textLight.c_str();
	GLuint vsLight = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsLight, 1, &vs_sourceLight, NULL);
	glCompileShader(vsLight);
	checkErrorShader(vsLight);
	glAttachShader(lightModel.program, vsLight);

	string fs_textLight = readShader("fs_light.glsl"); static const char* fs_sourceLight = fs_textLight.c_str();
	GLuint fsLight = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsLight, 1, &fs_sourceLight, NULL);
	glCompileShader(fsLight);
	checkErrorShader(fsLight);
	glAttachShader(lightModel.program, fsLight);

	glLinkProgram(lightModel.program);
	//read object
	readObj("sphere.obj", &lightModel);

	glGenVertexArrays(1, &lightModel.vao);			// Create Vertex Array Object
	glBindVertexArray(lightModel.vao);				// Bind VertexArrayObject

	glGenBuffers(3, lightModel.buffer);			// Create new buffers (Vertices, Texture Coordinates, Normals
	glBindBuffer(GL_ARRAY_BUFFER, lightModel.buffer[0]);	// Bind Buffer Vertex
	glBufferStorage(GL_ARRAY_BUFFER, lightModel.out_vertices.size() * sizeof(glm::vec3), &lightModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, lightModel.buffer[1]);	// Bind Buffer UV
	glBufferStorage(GL_ARRAY_BUFFER, lightModel.out_uvs.size() * sizeof(glm::vec2), &lightModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, lightModel.buffer[2]);	// Bind Buffer Normals
	glBufferStorage(GL_ARRAY_BUFFER, lightModel.out_normals.size() * sizeof(glm::vec3), &lightModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);		// Vertices									
	glBindVertexBuffer(0, lightModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);			// UV									
	glBindVertexBuffer(1, lightModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 0);			// Normals									
	glBindVertexBuffer(2, lightModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(2, 2);
	glEnableVertexAttribArray(2);

	lightModel.mv_location = glGetUniformLocation(lightModel.program, "mv_matrix");
	lightModel.proj_location = glGetUniformLocation(lightModel.program, "proj_matrix");
	lightModel.tex_location = glGetUniformLocation(lightModel.program, "tex");

	// --------------Head Model---------------------
	headModel.program = glCreateProgram();

	glAttachShader(headModel.program, vs);

	glAttachShader(headModel.program, fs);

	glLinkProgram(headModel.program);
	glUseProgram(headModel.program);

	readObj("untitled.obj", &headModel);

	glGenVertexArrays(1, &headModel.vao);			// Create Vertex Array Object
	glBindVertexArray(headModel.vao);				// Bind VertexArrayObject

	glGenBuffers(3, headModel.buffer);			// Create new buffers (Vertices, Texture Coordinates, Normals)
	glBindBuffer(GL_ARRAY_BUFFER, headModel.buffer[0]);	// Bind Buffer Vertex
	glBufferStorage(GL_ARRAY_BUFFER, headModel.out_vertices.size() * sizeof(glm::vec3), &headModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, headModel.buffer[1]);	// Bind Buffer UV
	glBufferStorage(GL_ARRAY_BUFFER, headModel.out_uvs.size() * sizeof(glm::vec2), &headModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, headModel.buffer[2]);	// Bind Buffer Normals
	glBufferStorage(GL_ARRAY_BUFFER, headModel.out_normals.size() * sizeof(glm::vec3), &headModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);		// Vertices									
	glBindVertexBuffer(0, headModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);			// UV									
	glBindVertexBuffer(1, headModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 0);			// Normals									
	glBindVertexBuffer(2, headModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(2, 2);
	glEnableVertexAttribArray(2);

	glBindVertexArray(headModel.vao);				// Bind VertexArrayObject

	headModel.mv_location = glGetUniformLocation(headModel.program, "mv_matrix");
	headModel.proj_location = glGetUniformLocation(headModel.program, "proj_matrix");
	headModel.tex_location = glGetUniformLocation(headModel.program, "tex");
	headModel.lightColor_location = glGetUniformLocation(headModel.program, "ia");
	headModel.lightColor_location = glGetUniformLocation(headModel.program, "ka");

	// --------------doll Model---------------------
	dollModel.program = glCreateProgram();

	glAttachShader(dollModel.program, vs);

	glAttachShader(dollModel.program, fs);

	glLinkProgram(dollModel.program);
	glUseProgram(dollModel.program);

	readObj("doll.obj", &dollModel);

	glGenVertexArrays(1, &dollModel.vao);			// Create Vertex Array Object
	glBindVertexArray(dollModel.vao);				// Bind VertexArrayObject

	glGenBuffers(3, dollModel.buffer);			// Create new buffers (Vertices, Texture Coordinates, Normals)
	glBindBuffer(GL_ARRAY_BUFFER, dollModel.buffer[0]);	// Bind Buffer Vertex
	glBufferStorage(GL_ARRAY_BUFFER, dollModel.out_vertices.size() * sizeof(glm::vec3), &dollModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, dollModel.buffer[1]);	// Bind Buffer UV
	glBufferStorage(GL_ARRAY_BUFFER, dollModel.out_uvs.size() * sizeof(glm::vec2), &dollModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, dollModel.buffer[2]);	// Bind Buffer Normals
	glBufferStorage(GL_ARRAY_BUFFER, dollModel.out_normals.size() * sizeof(glm::vec3), &dollModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);		// Vertices									
	glBindVertexBuffer(0, dollModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);			// UV									
	glBindVertexBuffer(1, dollModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 0);			// Normals									
	glBindVertexBuffer(2, dollModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(2, 2);
	glEnableVertexAttribArray(2);

	glBindVertexArray(dollModel.vao);				// Bind VertexArrayObject

	dollModel.mv_location = glGetUniformLocation(dollModel.program, "mv_matrix");
	dollModel.proj_location = glGetUniformLocation(dollModel.program, "proj_matrix");
	dollModel.tex_location = glGetUniformLocation(dollModel.program, "tex");
	dollModel.lightColor_location = glGetUniformLocation(dollModel.program, "ia");
	dollModel.lightColor_location = glGetUniformLocation(dollModel.program, "ka");

	// --------------bed Model---------------------
	bedModel.program = glCreateProgram();

	glAttachShader(bedModel.program, vs);

	glAttachShader(bedModel.program, fs);

	glLinkProgram(bedModel.program);
	glUseProgram(bedModel.program);

	readObj("be21.obj", &bedModel);

	glGenVertexArrays(1, &bedModel.vao);			// Create Vertex Array Object
	glBindVertexArray(bedModel.vao);				// Bind VertexArrayObject

	glGenBuffers(3, bedModel.buffer);			// Create new buffers (Vertices, Texture Coordinates, Normals)
	glBindBuffer(GL_ARRAY_BUFFER, bedModel.buffer[0]);	// Bind Buffer Vertex
	glBufferStorage(GL_ARRAY_BUFFER, bedModel.out_vertices.size() * sizeof(glm::vec3), &bedModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, bedModel.buffer[1]);	// Bind Buffer UV
	glBufferStorage(GL_ARRAY_BUFFER, bedModel.out_uvs.size() * sizeof(glm::vec2), &bedModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, bedModel.buffer[2]);	// Bind Buffer Normals
	glBufferStorage(GL_ARRAY_BUFFER, bedModel.out_normals.size() * sizeof(glm::vec3), &bedModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);		// Vertices									
	glBindVertexBuffer(0, bedModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);			// UV									
	glBindVertexBuffer(1, bedModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 0);			// Normals									
	glBindVertexBuffer(2, bedModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(2, 2);
	glEnableVertexAttribArray(2);

	glBindVertexArray(bedModel.vao);				// Bind VertexArrayObject

	bedModel.mv_location = glGetUniformLocation(bedModel.program, "mv_matrix");
	bedModel.proj_location = glGetUniformLocation(bedModel.program, "proj_matrix");
	bedModel.tex_location = glGetUniformLocation(bedModel.program, "tex");
	bedModel.lightColor_location = glGetUniformLocation(bedModel.program, "ia");
	bedModel.lightColor_location = glGetUniformLocation(bedModel.program, "ka");

	// --------------pic Model---------------------
	picModel.program = glCreateProgram();


	glAttachShader(picModel.program, vs);

	glAttachShader(picModel.program, fs);

	glLinkProgram(picModel.program);
	glUseProgram(picModel.program);

	readObj("pic3.obj", &picModel);

	glGenVertexArrays(1, &picModel.vao);			// Create Vertex Array Object
	glBindVertexArray(picModel.vao);				// Bind VertexArrayObject

	glGenBuffers(3, picModel.buffer);			// Create new buffers (Vertices, Texture Coordinates, Normals)
	glBindBuffer(GL_ARRAY_BUFFER, picModel.buffer[0]);	// Bind Buffer Vertex
	glBufferStorage(GL_ARRAY_BUFFER, picModel.out_vertices.size() * sizeof(glm::vec3), &picModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, picModel.buffer[1]);	// Bind Buffer UV
	glBufferStorage(GL_ARRAY_BUFFER, picModel.out_uvs.size() * sizeof(glm::vec2), &picModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, picModel.buffer[2]);	// Bind Buffer Normals
	glBufferStorage(GL_ARRAY_BUFFER, picModel.out_normals.size() * sizeof(glm::vec3), &picModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);		// Vertices									
	glBindVertexBuffer(0, picModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);			// UV									
	glBindVertexBuffer(1, picModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 0);			// Normals									
	glBindVertexBuffer(2, picModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(2, 2);
	glEnableVertexAttribArray(2);

	glBindVertexArray(picModel.vao);				// Bind VertexArrayObject

	picModel.mv_location = glGetUniformLocation(picModel.program, "mv_matrix");
	picModel.proj_location = glGetUniformLocation(picModel.program, "proj_matrix");
	picModel.tex_location = glGetUniformLocation(picModel.program, "tex");
	picModel.lightColor_location = glGetUniformLocation(picModel.program, "ia");
	picModel.lightColor_location = glGetUniformLocation(picModel.program, "ka");
	//initialize modelRotation vector
	modelRotations = new glm::vec3[1];
	modelRotations[0] = glm::vec3(30.0f, 10.0f, 0.0f);

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);

	// ----------Start Framebuffer Object---------------
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Create a texture for the framebuffer
	glGenTextures(1, &framebufferTexture);

	// Bind this texture so we can modify it.
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);

	//  Start the texture and give it a size but no data- of course if you resize you need to change your texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// filtering needed 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Depth buffer texture	- Need to attach depth too otherwise depth testing will not be performed.
	glGenRenderbuffers(1, &depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);


	// Create a quad to display our framebuffer
	displayVertices.push_back(glm::vec2(-1.0f, 1.0f));
	displayVertices.push_back(glm::vec2(-1.0f, -1.0f));
	displayVertices.push_back(glm::vec2(1.0f, -1.0f));
	displayVertices.push_back(glm::vec2(-1.0f, 1.0f));
	displayVertices.push_back(glm::vec2(1.0f, -1.0f));
	displayVertices.push_back(glm::vec2(1.0f, 1.0f));

	displayUvs.push_back(glm::vec2(0.0f, 1.0f));
	displayUvs.push_back(glm::vec2(0.0f, 0.0f));
	displayUvs.push_back(glm::vec2(1.0f, 0.0f));
	displayUvs.push_back(glm::vec2(0.0f, 1.0f));
	displayUvs.push_back(glm::vec2(1.0f, 0.0f));
	displayUvs.push_back(glm::vec2(1.0f, 1.0f));

	glGenBuffers(2, displayBuffer);		// Create a new buffer
	glBindBuffer(GL_ARRAY_BUFFER, displayBuffer[0]);	// Bind Buffer
	glBufferStorage(GL_ARRAY_BUFFER, displayVertices.size() * sizeof(glm::vec2), &displayVertices[0], GL_DYNAMIC_STORAGE_BIT); // Store the vertices

	glBindBuffer(GL_ARRAY_BUFFER, displayBuffer[1]);	// Bind Buffer												// Store the texture coordinates
	glBufferStorage(GL_ARRAY_BUFFER, displayUvs.size() * sizeof(glm::vec2), &displayUvs[0], GL_DYNAMIC_STORAGE_BIT);

	glGenVertexArrays(1, &displayVao);		// Create Vertex Array Object
	glBindVertexArray(displayVao);		// Bind VertexArrayObject

	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);		// Vertices									
	glBindVertexBuffer(0, displayBuffer[0], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);			// UV									
	glBindVertexBuffer(1, displayBuffer[1], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	//load shaders for framebuffer displays
	displayProgram = glCreateProgram();

	string dvs_text = readShader("vs_display.glsl"); 
	const char* dvs_source = dvs_text.c_str();
	GLuint dvs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(dvs, 1, &dvs_source, NULL);
	glCompileShader(dvs);
	checkErrorShader(dvs);
	glAttachShader(displayProgram, dvs);

	string dfs_text = readShader("fs_display.glsl"); const char* dfs_source = dfs_text.c_str();
	GLuint dfs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(dfs, 1, &dfs_source, NULL);
	glCompileShader(dfs);
	checkErrorShader(dfs);
	glAttachShader(displayProgram, dfs);

	glLinkProgram(displayProgram);
	glUseProgram(displayProgram);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// Disable rendering framebuffer to texture - aka render normally.

}

void update(GLfloat currentTime) {

	// calculate movement
	GLfloat cameraSpeed = 1.0f * deltaTime;
	if (keyStatus[GLFW_KEY_W]) cameraPosition += cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_S]) cameraPosition -= cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_A]) cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keyStatus[GLFW_KEY_D]) cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keyStatus[GLFW_KEY_LEFT])			modelAngle.y += 0.05f;
	if (keyStatus[GLFW_KEY_RIGHT])			modelAngle.y -= 0.05f;
	//check if anim is true and move the object on x axis 
	if (anim) {
		if (otherside) {
			modelTranslate.x+= 3.0f;
			otherside = false;
		}
		else
		{
			modelTranslate.x -= 3.0f;
			otherside = true;
		}
		
	}

	
}

void render(GLfloat currentTime) {

	//==============First Pass====================
	//---Render framebuffer to texture
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	glViewport(0, 0, windowWidth, windowHeight);

	// Clear colour buffer
	glm::vec4 backgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f); glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

	// Enable Depth testing
	glEnable(GL_DEPTH_TEST);

	// Clear Deep buffer
	static const GLfloat one = 1.0f; glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//initialize view matrix

	glm::mat4 viewMatrix = glm::lookAt(cameraPosition,					// eye
		cameraPosition + cameraFront,	// centre
		cameraUp);						// up

	// ----------draw floor model------------
	glUseProgram(floorModel.program);
	glBindVertexArray(floorModel.vao);
	glUniformMatrix4fv(floorModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

	glUniform4f(glGetUniformLocation(floorModel.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(floorModel.program, "ia"), ia.r, ia.g, ia.b, 1.0);
	glUniform1f(glGetUniformLocation(floorModel.program, "ka"), ka);
	glUniform4f(glGetUniformLocation(floorModel.program, "id"), id.r, id.g, id.b, 1.0);
	glUniform1f(glGetUniformLocation(floorModel.program, "kd"), 1.0f);
	glUniform4f(glGetUniformLocation(floorModel.program, "is"), is.r, is.g, is.b, 1.0);
	glUniform1f(glGetUniformLocation(floorModel.program, "ks"), 1.0f);
	glUniform1f(glGetUniformLocation(floorModel.program, "shininess"), 32.0f);

	glUniform1f(glGetUniformLocation(floorModel.program, "lightConstant"), 1.0f);
	glUniform1f(glGetUniformLocation(floorModel.program, "lightLinear"), 0.022f);
	glUniform1f(glGetUniformLocation(floorModel.program, "lightQuadratic"), 0.0019f);

	glUniform4f(glGetUniformLocation(floorModel.program, "lightPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(floorModel.program, "lightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
	glUniform1f(glGetUniformLocation(floorModel.program, "lightSpotCutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(floorModel.program, "lightSpotOuterCutOff"), glm::cos(glm::radians(15.0f)));

	// Bind textures and samplers 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorModel.texture[0]);
	glUniform1i(floorModel.tex_location, 0);
	   //set initial object's position
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		//scale object
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(floorModel.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(floorModel.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, floorModel.out_vertices.size());


	// ----------draw head model------------
	glUseProgram(headModel.program);
	glBindVertexArray(headModel.vao);
	glUniformMatrix4fv(headModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

	glUniform4f(glGetUniformLocation(headModel.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(headModel.program, "ia"), ia.r, ia.g, ia.b, 1.0);
	glUniform1f(glGetUniformLocation(headModel.program, "ka"), ka);
	glUniform4f(glGetUniformLocation(headModel.program, "id"), id.r, id.g, id.b, 1.0);
	glUniform1f(glGetUniformLocation(headModel.program, "kd"), 1.0f);
	glUniform4f(glGetUniformLocation(headModel.program, "is"), is.r, is.g, is.b, 1.0);
	glUniform1f(glGetUniformLocation(headModel.program, "ks"), 1.0f);
	glUniform1f(glGetUniformLocation(headModel.program, "shininess"), 32.0f);

	glUniform1f(glGetUniformLocation(headModel.program, "lightConstant"), 1.0f);
	glUniform1f(glGetUniformLocation(headModel.program, "lightLinear"), 0.022f);
	glUniform1f(glGetUniformLocation(headModel.program, "lightQuadratic"), 0.0019f);

	glUniform4f(glGetUniformLocation(headModel.program, "lightPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(headModel.program, "lightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
	glUniform1f(glGetUniformLocation(headModel.program, "lightSpotCutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(headModel.program, "lightSpotOuterCutOff"), glm::cos(glm::radians(15.0f)));

	// Bind textures and samplers
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, headModel.texture[0]);
	glUniform1i(headModel.tex_location, 0);
	    //initialize position,rotation and scale for object
		glm::mat4 modelMatrix1 = glm::translate(glm::mat4(1.0f),glm::vec3(1.88f, 0.61f, 0.52f));
		modelMatrix1 = glm::rotate(modelMatrix1,-30.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix1 = glm::scale(modelMatrix1, glm::vec3(0.5f, 0.5f, 0.5f));

		glm::mat4 mv_matrix1 = viewMatrix * modelMatrix1;

		glUniformMatrix4fv(glGetUniformLocation(headModel.program, "model_matrix"), 1, GL_FALSE, &modelMatrix1[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(headModel.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, headModel.out_vertices.size());
	

	// ----------draw doll model------------
	glUseProgram(dollModel.program);
	glBindVertexArray(dollModel.vao);
	glUniformMatrix4fv(dollModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

	glUniform4f(glGetUniformLocation(dollModel.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(dollModel.program, "ia"), ia.r, ia.g, ia.b, 1.0);
	glUniform1f(glGetUniformLocation(dollModel.program, "ka"), ka);
	glUniform4f(glGetUniformLocation(dollModel.program, "id"), id.r, id.g, id.b, 1.0);
	glUniform1f(glGetUniformLocation(dollModel.program, "kd"), 1.0f);
	glUniform4f(glGetUniformLocation(dollModel.program, "is"), is.r, is.g, is.b, 1.0);
	glUniform1f(glGetUniformLocation(dollModel.program, "ks"), 1.0f);
	glUniform1f(glGetUniformLocation(dollModel.program, "shininess"), 32.0f);

	glUniform1f(glGetUniformLocation(dollModel.program, "lightConstant"), 1.0f);
	glUniform1f(glGetUniformLocation(dollModel.program, "lightLinear"), 0.022f);
	glUniform1f(glGetUniformLocation(dollModel.program, "lightQuadratic"), 0.0019f);

	glUniform4f(glGetUniformLocation(dollModel.program, "lightPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(dollModel.program, "lightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
	glUniform1f(glGetUniformLocation(dollModel.program, "lightSpotCutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(dollModel.program, "lightSpotOuterCutOff"), glm::cos(glm::radians(15.0f)));

	// Bind textures and samplers 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dollModel.texture[0]);
	glUniform1i(dollModel.tex_location, 0);
	
		glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(8.58f, 1.177f, -0.61f));
		modelMatrix2 = glm::translate(modelMatrix2, modelTranslate);//change position of the object regarding the time
		modelMatrix2 = glm::rotate(modelMatrix2, -30.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(1.50f, 1.50f, 1.50f));

		glm::mat4 mv_matrix2 = viewMatrix * modelMatrix2;

		glUniformMatrix4fv(glGetUniformLocation(dollModel.program, "model_matrix"), 1, GL_FALSE, &modelMatrix2[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(dollModel.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, dollModel.out_vertices.size());
	
		// ----------draw bed model------------
		glUseProgram(bedModel.program);
		glBindVertexArray(bedModel.vao);
		glUniformMatrix4fv(bedModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

		glUniform4f(glGetUniformLocation(bedModel.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
		glUniform4f(glGetUniformLocation(bedModel.program, "ia"), ia.r, ia.g, ia.b, 1.0);
		glUniform1f(glGetUniformLocation(bedModel.program, "ka"), ka);
		glUniform4f(glGetUniformLocation(bedModel.program, "id"), id.r, id.g, id.b, 1.0);
		glUniform1f(glGetUniformLocation(bedModel.program, "kd"), 1.0f);
		glUniform4f(glGetUniformLocation(bedModel.program, "is"), is.r, is.g, is.b, 1.0);
		glUniform1f(glGetUniformLocation(bedModel.program, "ks"), 1.0f);
		glUniform1f(glGetUniformLocation(bedModel.program, "shininess"), 32.0f);

		glUniform1f(glGetUniformLocation(bedModel.program, "lightConstant"), 1.0f);
		glUniform1f(glGetUniformLocation(bedModel.program, "lightLinear"), 0.022f);
		glUniform1f(glGetUniformLocation(bedModel.program, "lightQuadratic"), 0.0019f);

		glUniform4f(glGetUniformLocation(bedModel.program, "lightPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
		glUniform4f(glGetUniformLocation(bedModel.program, "lightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
		glUniform1f(glGetUniformLocation(bedModel.program, "lightSpotCutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(bedModel.program, "lightSpotOuterCutOff"), glm::cos(glm::radians(15.0f)));

		// Bind textures and samplers
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bedModel.texture[0]);
		glUniform1i(bedModel.tex_location, 0);

		glm::mat4 modelMatrix3 = glm::translate(glm::mat4(1.0f), glm::vec3(-3.65f, 0.62f, -6.88f));
		modelMatrix3 = glm::scale(modelMatrix3, glm::vec3(2.0f, 2.0f, 2.0f));

		glm::mat4 mv_matrix3 = viewMatrix * modelMatrix3;

		glUniformMatrix4fv(glGetUniformLocation(bedModel.program, "model_matrix"), 1, GL_FALSE, &modelMatrix3[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(bedModel.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, bedModel.out_vertices.size());

		// ----------draw pic model------------
		glUseProgram(picModel.program);
		glBindVertexArray(picModel.vao);
		glUniformMatrix4fv(picModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

		glUniform4f(glGetUniformLocation(picModel.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
		glUniform4f(glGetUniformLocation(picModel.program, "ia"), ia.r, ia.g, ia.b, 1.0);
		glUniform1f(glGetUniformLocation(picModel.program, "ka"), ka);
		glUniform4f(glGetUniformLocation(picModel.program, "id"), id.r, id.g, id.b, 1.0);
		glUniform1f(glGetUniformLocation(picModel.program, "kd"), 1.0f);
		glUniform4f(glGetUniformLocation(picModel.program, "is"), is.r, is.g, is.b, 1.0);
		glUniform1f(glGetUniformLocation(picModel.program, "ks"), 1.0f);
		glUniform1f(glGetUniformLocation(picModel.program, "shininess"), 32.0f);

		glUniform1f(glGetUniformLocation(picModel.program, "lightConstant"), 1.0f);
		glUniform1f(glGetUniformLocation(picModel.program, "lightLinear"), 0.022f);
		glUniform1f(glGetUniformLocation(picModel.program, "lightQuadratic"), 0.0019f);

		glUniform4f(glGetUniformLocation(picModel.program, "lightPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
		glUniform4f(glGetUniformLocation(picModel.program, "lightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
		glUniform1f(glGetUniformLocation(picModel.program, "lightSpotCutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(picModel.program, "lightSpotOuterCutOff"), glm::cos(glm::radians(15.0f)));

		// Bind textures and samplers 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, picModel.texture[0]);
		glUniform1i(picModel.tex_location, 0);


		glm::mat4 modelMatrix4 = glm::translate(glm::mat4(1.0f), glm::vec3(9.14f, 5.78f, 0.37f));
		//rotate the object regarding the input of the user by the left and right keys
		modelMatrix4 = glm::rotate(modelMatrix4, 15.00f, glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix4 = glm::rotate(modelMatrix4, modelAngle.x + modelRotations[0].x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix4 = glm::rotate(modelMatrix4, modelAngle.y + modelRotations[0].y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix4 = glm::scale(modelMatrix4, glm::vec3(3.0f, 3.0f, 3.0f));

		glm::mat4 mv_matrix4 = viewMatrix * modelMatrix4;

		glUniformMatrix4fv(glGetUniformLocation(picModel.program, "model_matrix"), 1, GL_FALSE, &modelMatrix4[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(picModel.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, picModel.out_vertices.size());

		//==============Second Pass===================
		glBindFramebuffer(GL_FRAMEBUFFER, 0);	// Disable rendering framebuffer to texture - aka render normally.

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST); //not needed as we are just displaying a single quad

		glUseProgram(displayProgram);
		glUniform1f(glGetUniformLocation(displayProgram, "width"), windowWidth);
		glUniform1f(glGetUniformLocation(displayProgram, "height"), windowHeight);
		glBindVertexArray(displayVao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebufferTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

}

void onResizeCallback(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;

	aspect = (float)w / (float)h;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) keyStatus[key] = true;
	else if (action == GLFW_RELEASE) keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);

	if (firstMouse) {
		lastX = (GLfloat)mouseX; lastY = (GLfloat)mouseY; firstMouse = false;
	}

	GLfloat xoffset = mouseX - lastX;
	GLfloat yoffset = lastY - mouseY; // Reversed
	lastX = (GLfloat)mouseX; lastY = (GLfloat)mouseY;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity; yoffset *= sensitivity;

	yaw += xoffset; pitch += yoffset;

	// check for pitch out of bounds otherwise screen gets flipped
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraFront = glm::normalize(front);

}

void debugGL() {
	//Output some debugging information
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;

	// Enable Opengl Debug
	//glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam) {

	cout << "---------------------opengl-callback------------" << endl;
	cout << "Message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << " --- ";

	cout << "id: " << id << " --- ";
	cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		cout << "NOTIFICATION";
	}
	cout << endl;
	cout << "-----------------------------------------" << endl;
}

string readShader(string name) {
	string vs_text;
	std::ifstream vs_file(name);

	string vs_line;
	if (vs_file.is_open()) {

		while (getline(vs_file, vs_line)) {
			vs_text += vs_line;
			vs_text += '\n';
		}
		vs_file.close();
	}
	return vs_text;
}

void  checkErrorShader(GLuint shader) {
	// Get log lenght
	GLint maxLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	// Init a string for it
	std::vector<GLchar> errorLog(maxLength);

	if (maxLength > 1) {
		// Get the log file
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		cout << "--------------Shader compilation error-------------\n";
		cout << errorLog.data();
	}

}

void readObj(string name, struct modelObject *obj) {
	cout << "Loading model " << name << "\n";

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< string > materials, textures;
	std::vector< glm::vec3 > obj_vertices;
	std::vector< glm::vec2 > obj_uvs;
	std::vector< glm::vec3 > obj_normals;

	std::ifstream dataFile(name);

	string rawData;		// store the raw data.
	int countLines = 0;
	if (dataFile.is_open()) {
		string dataLineRaw;
		while (getline(dataFile, dataLineRaw)) {
			rawData += dataLineRaw;
			rawData += "\n";
			countLines++;
		}
		dataFile.close();
	}

	cout << "Finished reading model file " << name << "\n";

	istringstream rawDataStream(rawData);
	string dataLine;
	int linesDone = 0;
	while (std::getline(rawDataStream, dataLine)) {
		if (dataLine.find("v ") != string::npos) {	// does this line have a vector?
			glm::vec3 vertex;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_vertices.push_back(vertex);
		}
		else if (dataLine.find("vt ") != string::npos) {	// does this line have a uv coordinates?
			glm::vec2 uv;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			uv.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			uv.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_uvs.push_back(uv);
		}
		else if (dataLine.find("vn ") != string::npos) { // does this line have a normal coordinates?
			glm::vec3 normal;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			normal.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			normal.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			normal.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_normals.push_back(normal);
		}
		else if (dataLine.find("f ") != string::npos) { // does this line defines a triangle face?
			string parts[3];

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			parts[0] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			parts[1] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			parts[2] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			for (int i = 0; i < 3; i++) {		// for each part

				vertexIndices.push_back(stoul(parts[i].substr(0, parts[i].find("/"))));

				int firstSlash = parts[i].find("/"); int secondSlash = parts[i].find("/", firstSlash + 1);

				if ((firstSlash + 1) != (secondSlash)) {	// there are texture coordinates.
					uvIndices.push_back(stoul(parts[i].substr(firstSlash + 1, secondSlash - firstSlash + 1)));
				}


				normalIndices.push_back(stoul(parts[i].substr(secondSlash + 1)));

			}
		}
		else if (dataLine.find("mtllib ") != string::npos) { // does this object have a material?
			materials.push_back(dataLine.substr(dataLine.find(" ") + 1));
		}

		linesDone++;

		if (linesDone % 50000 == 0) {
			cout << "Parsed " << linesDone << " of " << countLines << " from model...\n";
		}

	}



	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i] - 1]);
		(*obj).out_normals.push_back(obj_normals[normalIndices[i] - 1]);
		(*obj).out_uvs.push_back(obj_uvs[uvIndices[i] - 1]);
	}


	// Load Materials
	for (unsigned int i = 0; i < materials.size(); i++) {

		std::ifstream dataFileMat(materials[i]);

		string rawDataMat;		// store the raw data.
		int countLinesMat = 0;
		if (dataFileMat.is_open()) {
			string dataLineRawMat;
			while (getline(dataFileMat, dataLineRawMat)) {
				rawDataMat += dataLineRawMat;
				rawDataMat += "\n";
			}
			dataFileMat.close();
		}

		istringstream rawDataStreamMat(rawDataMat);
		string dataLineMat;
		while (std::getline(rawDataStreamMat, dataLineMat)) {
			if (dataLineMat.find("map_Kd ") != string::npos) {	// does this line have a texture map?
				textures.push_back(dataLineMat.substr(dataLineMat.find(" ") + 1));
			}
		}
	}

	(*obj).texture = new GLuint[textures.size()];		// Warning possible memory leak here - there is a new here, where is your delete?
	glGenTextures(textures.size(), (*obj).texture);

	for (unsigned int i = 0; i < textures.size(); i++) {
		readTexture(textures[i], (*obj).texture[i]);
	}

	cout << "done";
}

void readTexture(string name, GLuint textureName) {

	// Flip images as OpenGL expects 0.0 coordinates on the y-axis to be at the bottom of the image.
	stbi_set_flip_vertically_on_load(true);

	// Load image Information.
	int iWidth, iHeight, iChannels;
	unsigned char *iData = stbi_load(name.c_str(), &iWidth, &iHeight, &iChannels, 0);

	// Load and create a texture 
	glBindTexture(GL_TEXTURE_2D, textureName); // All upcoming operations now have effect on this texture object

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, iWidth, iHeight);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, iData);


	// This only works for 2D Textures...
	// Set the texture wrapping parameters (next lecture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters (next lecture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Generate mipmaps (next lecture)
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);// Unbind texture when done, so we won't accidentily mess up our texture.


}


