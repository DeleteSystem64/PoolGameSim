/**************
 * CollisionHandler
 * 
 * Used to detect and handle sphere-sphere collisions and sphere-plane collisions.
 * 
 * 
 * SOME FUNCTIONS ADAPTED FROM NOTES/LECTURE SLIDES FROM CPSC 4190 WITH DR. TESSENDORF
 */

#ifndef __COLLIDE_H__
#define __COLLIDE_H__
#include "Sphere.h"
#include "Plane.h"
#include "maths_funcs.h"
#include <cmath>

class CollisionHandler
{
    public:
    //used to simulate inelastic collisions
    float inelasticity = .8;

    /**********************************---SPHERE-PLANE---*******************************************/
    //returns true iff sphere and plane will collide within time timestep
    //****** Notes from CPSC 4190 with Dr. Tessendorf were consulted
    bool willCollide(Sphere& sphere, const Plane& plane, float time_step);
    //returns the time when sphere and plane will collide
    //****** Notes from CPSC 4190 with Dr. Tessendorf were consulted
    //***** Also consulted this webpage: https:// mathinsight.org/distance_point_plane
    float getCollisionTime(const Sphere& sphere, const Plane& plane, float time_step);
    //reflects a sphere's velocity along plane's normal
    //****** Notes from CPSC 4190 with Dr. Tessendorf were consulted
    void handleCollision(Sphere& sphere, Plane& plane);
    

    /**********************************---SPHERE-SPHERE---*******************************************/
    //returns true iff sphere1 and sphere2 will collide within time timestep
    //ALSO if returns true, fills hit_time with the time in which the two spheres will collide
    /***** Adapted from notes from CPSC 4190 with Dr. Tessendorf ***/
    bool willCollide(Sphere& sphere1, Sphere& sphere2, float time_step,float& hit_time);

    //reflects spheres' velocities along the collision normal and update their position
    /***** Adapted from notes from CPSC 4190 with Dr. Tessendorf ***/
    void handleCollision(Sphere& sphere1, Sphere& sphere2, float time_step);

    //separates two over-lapping spheres
    void separateSpheres(Sphere& sphere0, Sphere& sphere1);
    
   
    
        
};

#endif