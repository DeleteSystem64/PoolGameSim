/******************************************************************************|
| CPSC 4050 PROJECT-- Michael Schwab, 2024.                                    |
| Modified from:                                                               |
|                                                                              |
| CPSC 4050/6050 Computer Garphics Assignment 5, Daljit Singh Dhillon, 2020    |
| Which itself was modified from:                                              |
|                                                                              |
| DPA 8090 Example Code, Eric Patterson, 2017                                  |
|                                                                              |
| This is mostly plain C but uses a few things from C++ so needs C++ compiler. |
| Some OpenGL setup code here and obj_parser, math_funcs, and gl_utils         |
| are from Angton Gerdelan and "Anton's OpenGL 4 Tutorials."                   |
| http://antongerdelan.net/opengl/                                             |
| Email: anton at antongerdelan dot net                                        |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
|******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <GL/glew.h>       // Include GLEW (or new version of GL if on Windows).
#include <GLFW/glfw3.h>    // GLFW helper library.
#include <iostream>
#include <unordered_set>

#include "maths_funcs.h"   // Anton's maths functions.
#include "gl_utils.h"      // Anton's opengl functions and small utilities like logs
#include "obj_parser.h"    // Anton's little Wavefront .obj mesh loader
#include "stb_image.h"     // Sean Barrett's image loader with Anton's load_texture()
#include "Sphere.h"
#include "Plane.h"
#include "CollisionHandler.h"
#include "Control.h"
#include "load_texture.h"

#include "Solver.cpp"  //leapfrogSolve()

using namespace std;
#define _USE_MATH_DEFINES
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444


/************************************************GLOBAL VARIABLES ******************************************* */
// The view and proj matrices below make-up the camera position, orientation, fov, etc.
// The model matrices moves your surface of revolution to the world coordinate system
mat4 view_mat;
mat4 proj_mat;
mat4 model_mat;

//Booleans to control program based on user input
bool mouse_down = false;
bool hit_cueball = false;
bool should_reset = false;
bool alternate_mouse_vec = false;

//These are used to track mouse movement
double mouseX;
double mouseY;
vec3 mouse_vec_start;
vec3 mouse_vec_end;
vec3 mouse_vec;

//For shader uniforms; lighting
GLint phong_exponent = 32;
GLfloat point_light_pos[3] = {0.f,0.f,5.f};
GLfloat dir_light_vec[3] = {1.0,-1.0,-2.0};
GLint use_diff = 1;
GLint use_spec = 1;
GLint use_tex = 1;
GLint use_rough = 0;

//Other user inputs
float friction = 0.5;

// the vector below indicates camera placement. 
//It looks at (0,0,0) with (0,1,0) as the up-vector
vec3 cam_pos (0.0f, 0.0f, 100.0f);


/*********************************************************HELPER FUNCTIONS******************************************** */
// Sets up the default state of shader
// Only called once, before rendering loop starts
void loadUniforms(GLuint shader_programme)
{	
/*---------------------------SET RENDERING DEFAULTS---------------------------*/
	
	// Choose vertex and fragment shaders to use as well as view and proj matrices.
	int model_mat_location = glGetUniformLocation (shader_programme, "model_mat");
	int view_mat_location  = glGetUniformLocation (shader_programme, "view_mat");
	int proj_mat_location  = glGetUniformLocation (shader_programme, "proj_mat");
	
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view_mat.m);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, proj_mat.m);
	glUniformMatrix4fv (model_mat_location, 1, GL_FALSE, model_mat.m);
	
	// WRITE CODE TO LOAD OTHER UNIFORM VARIABLES LIKE FLAGS FOR ENABLING OR DISABLING CERTAIN FUNCTIONALITIES
	
	GLint phong_exponent_location = glGetUniformLocation (shader_programme, "phong_exponent");
	glUniform1i(phong_exponent_location, phong_exponent);

	GLint point_light_pos_location = glGetUniformLocation (shader_programme, "point_light_pos");
	glUniform3fv(point_light_pos_location, 1, point_light_pos);

	GLint dir_light_vec_location = glGetUniformLocation (shader_programme, "dir_light_vec");
	glUniform3fv(dir_light_vec_location, 1, dir_light_vec);

	GLint use_diff_location = glGetUniformLocation (shader_programme, "use_diff");
	glUniform1i(use_diff_location,use_diff);

	GLint use_spec_location = glGetUniformLocation (shader_programme, "use_spec");
	glUniform1i(use_spec_location,use_spec);

	GLint use_tex_location = glGetUniformLocation (shader_programme, "use_tex");
	glUniform1i(use_tex_location,use_tex);

	GLint use_rough_location = glGetUniformLocation (shader_programme, "use_rough");
	glUniform1i(use_rough_location,use_rough);

}

//just loads in position of cursor
void cursor_position_callback(GLFWwindow * window, double xpos, double ypos)
{
	mouseX = xpos;
	mouseY = ypos;
}

//allows the user to hit the cueball with their mouse
//when left mouse button is held down, we begin tracking the mouse vector
//when its released, that's when we give the cueball velocity
void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//mark where the user first clicked mouse
		mouse_down = true;
		mouse_vec_start = vec3(mouseX, mouseY, 0);

	}
	else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		//get the vector between points where user clicked/released
		mouse_down = false;
		mouse_vec = vec3(mouseX, mouseY, 0) - mouse_vec_start;
		//tells us that we need to give cueball velocity later
		hit_cueball = true;
	}
}

//keyboard callback function
void keyboardFunction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//reset game when R is pressed
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
		should_reset = true;
	}
	else if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
	{
		friction += .05;
		cout << "Friction:" << friction << endl;
	}
	else if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
	{
		
		friction -= .05;
		if(friction < -1e-6) friction = 0.00;
		cout << "Friction:" << friction << endl;
	}
	else if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		alternate_mouse_vec = !alternate_mouse_vec;
		cout << "Switching control schemes...  ";
		if(alternate_mouse_vec) cout << "Using PULL controls" << endl;
		else cout << "Using PUSH controls" << endl;
	}
	//close window when esc is pressed
	else if (GLFW_PRESS == glfwGetKey (g_window, GLFW_KEY_ESCAPE)) {
		// Close window when esacape is pressed
		glfwSetWindowShouldClose (g_window, 1);
	}

}

/*******************************************************************************************************************
 * ***********************************------------MAIN----------------**********************************************
 *******************************************************************************************************************/
/** Lots of this code was given as starter code in Assignment 3 */
int main (int argc, char *argv[]) {
	
/*--------------------------------START OPENGL--------------------------------*/
	assert (restart_gl_log ());
	assert (start_gl ());        // Start glfw window with GL context within.

	glfwSetKeyCallback(g_window, keyboardFunction);
	glfwSetCursorPosCallback(g_window, cursor_position_callback);
	glfwSetMouseButtonCallback(g_window, mouse_button_callback);

/*-------------------------------CREATE SHADERS-------------------------------*/
    // The vertex shader program generally acts to transform vertices.
    // The fragment shader is where we'll do the actual "shading."
	GLuint shader_programme = create_programme_from_files (
		argv[1], argv[2]
	);

	glUseProgram (shader_programme);
	

/*-------------------------------SETUP TEXTURES-------------------------------*/
	
	
/*-------------------------------CREATE CAMERA--------------------------------*/

        // This sets up the matrix transformations that act as the camera lens
        // and sensor would to transform vertices from view space.
	float near = 0.1f;   // clipping planes
	float far = 1000.0f; 
	float fovy = 35.0f;  // vertical field of view, horiz calculated for aspect
	float aspect = (float)g_gl_width / (float)g_gl_height;      // aspect ratio
	proj_mat = perspective (fovy, aspect, near, far);

	mat4 T = translate (
		identity_mat4 (), vec3 (-cam_pos.v[0], -cam_pos.v[1], -cam_pos.v[2])
	);
	
	// would usually use inverse camera orientation with position to construct
	// view matrix, but for simplicity since camera axis-aligned, not needed here
	view_mat =  T; 
	
/*---------------------------SET RENDERING DEFAULTS---------------------------*/
	// The model matrix stores the position and orientation transformations for your SoR

	model_mat = identity_mat4();


	// Setup basic GL display attributes.	
	glEnable (GL_DEPTH_TEST);   // enable depth-testing
	glDepthFunc (GL_LESS);      // depth-testing interprets a smaller value as "closer"
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(100.0f); // Set point size (in pixels)
	glEnable (GL_CULL_FACE);    // cull face
	glCullFace (GL_BACK);       // cull back face
	glFrontFace (GL_CCW);       // set counter-clock-wise vertex order to mean the front
	glClearColor (0.1, 0.1, 0.1, 1.0);   // non-black background to help spot mistakes
	glViewport (0, 0, g_gl_width, g_gl_height); // make sure correct aspect ratio

	/**--------------------------------------SETTING UP CONTROL-----------------------------------***/
	/* Control does a lot of the legwork in managing the program.
	 * Right here, the constructor loads in some Sphere and a Pool Table OBJ files
	 * It also creates Sphere and Plane objects used in collision detection
	*/
	Control control;

	//Get all spheres into position
	control.reset();
	//load in the textures for spheres/pool table
	control.loadTextures(shader_programme);

	
/*-------------------------------RENDERING LOOP-------------------------------*/
	while (!glfwWindowShouldClose (g_window)) {
		// update timers
		static double previous_seconds = glfwGetTime ();
		double current_seconds = glfwGetTime ();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;
		_update_fps_counter (g_window);
		
		// clear graphics context
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// setup shader use	
		glUseProgram (shader_programme);

		//if the user inputted to reset game, do that now
		if(should_reset)
		{
			control.reset();
			cout << "Resetting..." << endl;
			should_reset = false;
		}
		//if the user inputted to hit cueball, do that now
		if(hit_cueball)
		{
			hit_cueball = false;
			mouse_vec = mouse_vec/10;
			//BLAH
			if(alternate_mouse_vec)
				control.spheres[0].vel = vec3(mouse_vec.v[0], -mouse_vec.v[1], -mouse_vec.v[2]);
			else
				control.spheres[0].vel = vec3(-mouse_vec.v[0], mouse_vec.v[1], mouse_vec.v[2]);
		}
		/*******************************************ADVANCING SPHERES' POSITION AND VELOCITY *****************************
		 * ***************************************************************************************************************/
		leapfrogSolve(&control, elapsed_seconds, friction);

		
		// load uniform variables for shaders
		loadUniforms(shader_programme);

		//drawing each sphere
		for(int i = 0; i < control.num_spheres; i++)
		{
			if(!control.deactivated[i])
			{
				control.drawSphere(i, model_mat, shader_programme);
			}
		}	
		//drawing table
		control.drawTable(shader_programme);
		
		//drawing mouse vector
		if(mouse_down)
			control.drawMouseVector(mouseX, mouseY, mouse_vec_start, shader_programme, alternate_mouse_vec);
		
		// update other events like input handling 
		glfwPollEvents();

		aspect = (float)g_gl_width / (float)g_gl_height; // aspect ratio
		proj_mat = perspective (fovy, aspect, near, far);
	  
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers (g_window);
	}
	
	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}
