/***************
 * Sphere
 *    --Has radius length, velocity, and position
 *    --And a transform matrix, used to draw the sphere
 */
#ifndef SPHERE__H__
#define SPHERE__H__

#include "maths_funcs.h"

class Sphere
{
public: 
    float radius_length;
    //position, velocity, and acceleration is that of the center of the sphere
    //every other point in sphere is going to have same velocity and acceleration as center
    //every other point in the sphere has a position that can be derived from center's position
    vec3 pos;
    vec3 vel;
    //unused in this project
    vec3 acc;
    mat4 trans_mat;
    //the mass of the entire sphere; arbitrary
    float mass = 1;
    Sphere(float r_length) : trans_mat(identity_mat4()), pos(vec3(0,0,0)), vel(vec3(0,0,0)), acc(vec3(0,0,0)), radius_length(r_length) {};
    Sphere() : trans_mat(identity_mat4()), pos(vec3(0,0,0)), vel(vec3(0,0,0)), acc(vec3(0,0,0)) {};

    //updates position using velocity along a timestep
    void updatePos(float timestep);

    //these set position, in slightly different ways
    void setPos(vec3 newPos);
    void superSetPos(vec3 newPos);
};

#endif 