/********************************************************************************************
 * -----------------------------********* CONTROL *********---------------------------------*
 * -----------------------------***************************---------------------------------*
 * Control is a class for managing a simulation game of 8-ball pool. It loads OBJ files,
 * calls for calculations in managing the simulation, and draws the scene.
 * 
 * It manages 16 Sphere objects
 *      Each sphere:
 *                -has an index
 *                -has position, velocity, and radius (stored within Sphere Object)
 *                -Is active or deactivated with 'deactivated' bool[]
 *                -Has a corresponding set of points loaded into GPU, taken from OBJ file
 *                   to draw the sphere
 * 
 * It manages 12 planes.
 *      -6 of those planes are walls of pool table
 *      -Other 6 are "pockets"
 *      
 *      -12 Planes are laid out like this:
 *       ____-P-_____
 *     P/            \P 
 *      |            |
 *      |            |
 *     P\____   _____/P
 *           -P-
 *     P's are pockets
 * 
 *      -There's also a table OBJ file which is loaded into GPU and drawn every frame.
 *        -This is purely for visual purposes, it's static and isnt used for any calculations
 * 
 * There's also the mouse vector
 *      -A line that has one point at origin of cueball and another point somewhere else
 *      -Used to show user where they are pointing, when they are about to hit the cueball
 * 
 * 
 *
 */

#ifndef CONTROL__H_
#define CONTROL__H_

#include <string>
#include <iostream>
#include <cmath>

/***************************************************************
| obj_parser, math_funcs, and gl_utils                         |
| are from Angton Gerdelan and "Anton's OpenGL 4 Tutorials."   |
| http://antongerdelan.net/opengl/                             |
| Email: anton at antongerdelan dot net                        |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.|
****************************************************************
 */
#include "load_texture.h"
#include "obj_parser.h"
#include "gl_utils.h"
#include "Sphere.h"
#include "Plane.h"

class Control
{
public:

    /***********************************************************************************************
     *                     ---------------------MEMBERS-------------------------                   *
     ***********************************************************************************************/

    /*-----------------------------SPHERES--------------------------------*/

    //number of spheres (hard coded, probably better to make these arrays dynamic)
    const static int num_spheres = 16;
    /****The Spheres
     * spheres[0] is the cueball
     * spheres[1-15] are the other balls
    */
    Sphere spheres[num_spheres];

    //Determines where the ith sphere is deactivated or not
    bool deactivated[num_spheres] = {false};

    //Gives number of points in ith sphere, for drawing purposes only
    int point_counts[num_spheres];

    //the vao locations for each sphere
    GLuint sphere_vaos[num_spheres];

    //The location of textures for spheres
    //There's 4 textures: cueball, 8ball, striped, solid
    unsigned int tex_locs[4];
   

    /***** ---------------------------TABLE/POCKETS/PLANES ------------------------------**/
    //location of pool table's vao
    GLuint table_vao;

    //Pockets: planes with special collision handling
    const static int num_pockets = 6;
    Plane pockets[num_pockets];
    
    //Walls of pool table: Planes
    const static int num_planes = 6;
    Plane planes[num_planes];
    vec3 cueball_starting_pos = vec3(15,0,0);
    
    //location of table texture
    unsigned int table_tex_loc;
    //number of points in pool table OBJ, for drawing purposes only
    int table_point_count;

    /***** ----------------------------------MOUSE VECTOR------------------------------**/
    //location of mouse vector vao
    //Mouse vector is used to show the user where they are aiming
    GLuint mouse_vec_vao;

    Control();

    /***********************************************************************************************
     *                     ---------------------METHODS------------------------                    *
     ***********************************************************************************************/

    /************************* LOADING METHODS **********************************/
    //loads an OBJ file into GPU, then returns location in vao
    int loadOBJ(char* filename, GLuint& vao);

    //loads buffer for mouse vector, returns location in vao
    void loadMouseVec(GLuint& vao);

    //loads some textures into GPU
    //hardcoded to be 4 textures for spheres: solid, striped, cueball, 8-ball
    //and a texture for the table
    void loadTextures(GLuint shader);

    /************************* DRAWING METHODS **********************************/
    //call to draw ith sphere
    void drawSphere(int sphere_index, mat4& model, GLuint shader);
    void drawTable(GLuint shader);
    //call to draw mouse vector, a line
    void drawMouseVector(float mousex, float mousey, vec3 start, GLuint shader, bool alt_mouse_vec);


    /*****************************GAME METHODS **********************************/
    //set all velocities to zero, activate deactivated balls, reset positions
    void reset();

    //Place spheres like they would be in an 8-ball game.
    //15 spheres start in a pyramid formation
    //Cue ball starts across from them
    void setUp8BallGame();

    //marks the ith sphere as inactive
    void deactivateSphere(int index);
};

#endif