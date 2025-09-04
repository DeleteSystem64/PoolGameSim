/**SOLVER
 * Just a collection of functions that work to advance position and velocity of spheres in Control.
 * Uses leapfrog integration
 */


#include "Control.h"
#include "CollisionHandler.h"
#include <unordered_set>
using namespace std;

void leapfrogSolve(Control* control, float dt, float friction);
void advanceVelocity(Control* control, double time_step, float friction);
void advancePosition(Control* control, float dt);

//Solver using leapfrog integration
void leapfrogSolve(Control* control, float dt, float friction)
{
    advanceVelocity(control, dt/2.0, friction);
    advancePosition(control, dt);
    advanceVelocity(control, dt/2.0, friction);
}


//A hash function, which is like a pre-hash function before a pair of ints
//are inserted into an unordered_set. This is to avoid using std::pairs
int encode(unsigned int a, unsigned int b)
{
	return a*1000000000 + b;
}

//updates velocity using dt
//in this case, only source of acceleration is friction
void advanceVelocity(Control* control, double time_step, float friction)
{
    for(int i = 0; i < control->num_spheres; i++)
    {
        //friction is a vector in opposite direction of velocity with less magnitude
        control->spheres[i].vel -= control->spheres[i].vel*friction*time_step;
        //if velocity is close to zero, set it to zero
        if(length(control->spheres[i].vel) < 1e-6) control->spheres[i].vel = vec3(0,0,0);
    }
}

/*** NOTES FROM CSPC 4190 CONSULTED FOR THIS FUNCTION */
void advancePosition(Control* control, float dt)
{
    //CollisionHandler has all our collision-related methods
	CollisionHandler ch;
	
    //These are used to ensure we do not handle the same collision twice in one loop
    //which can cause a infinite loop
    unordered_set<int> handled_sphere_plane_pairs;
    unordered_set<int> handled_sphere_sphere_pairs;

    //Any sphere that is involved with collision handling already has moved to time=dt
    //we'll use this set to ensure we don't update position again of these spheres
    unordered_set<int> handled_spheres;

    //The logic here is we check every pair of sphere-plane, sphere-pocket, and sphere-sphere.
    //Whichever collision happens soonest, we handle that first, update dt and positions, then check repeat the process
    //when no collisions are detected, or we have spanned the entire timestep, the loop ends
    bool checkingForCollisions = true;
    while(checkingForCollisions)
    {
        checkingForCollisions = false;

        /** *******************DETECTING SPHERE-POCKET COLLISIONS*************************************/
        //pocket_index: index of pocket involved in soonest pocket-sphere collision
        int pocket_index = 0;
        //sphereP_index: index of sphere involved in soonest pocket-sphere collision
        int sphereP_index = 0;

        //the time of the soonest pocket-sphere collision
        float pocket_min_hit_time = MAXFLOAT;
        
        for(int i = 0; i < control->num_pockets; i++)
        {
            for(int j = 0; j < control->num_spheres; j++)
            {
                float hit_time;
                if(!control->deactivated[j] && ch.willCollide(control->spheres[j], control->pockets[i], dt) && handled_sphere_plane_pairs.find(encode(i,j)) == handled_sphere_plane_pairs.end())
                {
                    checkingForCollisions = true;
                    hit_time = ch.getCollisionTime(control->spheres[j], control->pockets[i], dt);
                    if(pocket_min_hit_time > hit_time && hit_time >= 0) 
                    {
                        pocket_min_hit_time = hit_time;
                        pocket_index = i;
                        sphereP_index = j;
                    }
                }
            }
        }

        /**********************DETECTING SPHERE-PLANE COLLISIONS*************************************/
        //The time of soonest plane-sphere collision
        float plane_min_hit_time = MAXFLOAT;
        
        //stores index of plane involved in soonest collision
        int plane_index = 0;
        //stores index of sphere involved in soonest sphere-plane collision
        int sphere_index = 0;

        for(int i = 0; i < control->num_planes; i++)
        {
            for(int j = 0; j < control->num_spheres; j++)
            {
                float hit_time;
                if(!control->deactivated[j] && ch.willCollide(control->spheres[j], control->planes[i], dt) && handled_sphere_plane_pairs.find(encode(i,j)) == handled_sphere_plane_pairs.end())
                {
                    checkingForCollisions = true;
                    hit_time = ch.getCollisionTime(control->spheres[j], control->planes[i], dt);
                    if(plane_min_hit_time > hit_time && hit_time >= 0) 
                    {
                        plane_min_hit_time = hit_time;
                        plane_index = i;
                        sphere_index = j;
                    }
                }
            }
            
        }

        /**********************DETECTING SPHERE-SPHERE COLLISIONS*************************************/
        //sphere_min_hit_time: time of soonest sphere-sphere collission
        float sphere_min_hit_time = MAXFLOAT;

        //sphere_index1: index of sphere involved in soonest sphere-sphere collision
        int sphere_index1;
        //sphere_index2: index of sphere involved in soonest sphere-sphere collision
        int sphere_index2;
        for(int i = 0; i < control->num_spheres; i++)
        {
            for(int j = i+1; j < control->num_spheres; j++)
            {
                float hit_time;
                float alpha;
                if(!control->deactivated[j] && !control->deactivated[i] && ch.willCollide(control->spheres[i],control->spheres[j], dt, alpha) 
                    && handled_sphere_sphere_pairs.find(encode(i,j)) == handled_sphere_sphere_pairs.end())
                {
                    //std::cout << "****** COLLISION " <<  count++ << "******" << std::endl;
                    checkingForCollisions = true;
                    hit_time = dt*alpha;
                    if(sphere_min_hit_time > hit_time)
                    {
                        sphere_min_hit_time = hit_time;
                        sphere_index1 = i;
                        sphere_index2 = j;
                    }
                }
            }
        }

        /************************************************COLLISION HANDLING **********************************************/
        //of all the collisions we checked, whichever happens earliest in timestep, we will handle
        //on other hand, if no collision happens within timestep, this 'if' statement is skipped and loop ends
        if(checkingForCollisions)
        {
            //if the soonest collision is sphere-sphere, handle that
            if(sphere_min_hit_time < plane_min_hit_time && sphere_min_hit_time < pocket_min_hit_time)
            {
                
                dt = dt - sphere_min_hit_time;
                //advance spheres to hit time
                for(int i = 0; i < control->num_spheres; i++)
                {
                    control->spheres[i].updatePos(sphere_min_hit_time);
                }
                //handle the collision
                ch.handleCollision(control->spheres[sphere_index1], control->spheres[sphere_index2], dt);
                handled_sphere_sphere_pairs.insert(encode(sphere_index1,sphere_index2));
                handled_spheres.insert(sphere_index1);
                handled_spheres.insert(sphere_index2);
            }
            //if the soonest collision is between a pocket and sphere, we'll handle that
            else if (pocket_min_hit_time < plane_min_hit_time)
            {
                dt = dt - pocket_min_hit_time;
                //advance spheres to hit time
                for(int i = 0; i < control->num_spheres; i++)
                {
                    control->spheres[i].updatePos(pocket_min_hit_time);
                }
                //if sphere is cueball: reset its position
                if(sphereP_index == 0)
                {
                    control->spheres[0].superSetPos(control->cueball_starting_pos);
                    control->spheres[0].vel = vec3(0,0,0);
                    cout << "Pocket scratch! Resetting pos of cueball..." << endl;
                }
                //if sphere is not cueball, deactivate it
                else
                {
                    control->deactivateSphere(sphereP_index);

                    //Printing a little message depending on which ball they hit into the pocket
                    if(sphereP_index == 8)
                    {
                        cout << "You sunk the 8-ball!" << endl;
                    }
                    else if(sphereP_index < 8)
                    {
                        cout << "You sunk a solid!" << endl;
                    }
                    else
                    {
                        cout << "You sunk a stripe!" << endl;
                    }
                }
            }
            //if soonest collision is sphere-plane, handle that
            else
            {
                dt = dt - plane_min_hit_time;
                //advance spheres to hit time
                for(int i = 0; i < control->num_spheres; i++)
                {
                    control->spheres[i].updatePos(plane_min_hit_time);
                }
                //reflect colliding sphere's velocity
                ch.handleCollision(control->spheres[sphere_index], control->planes[plane_index]);
                handled_sphere_plane_pairs.insert(encode(plane_index, sphere_index));
            }
            
        }
        if(dt <= 0 ) checkingForCollisions = false;
    }
    //whatever is left of the timestep, if anything, advance positions of spheres to that time

    //sphere.updatePos(dt);
    for(int i = 0; i < control->num_spheres; i++)
    {
        //only advance spheres that aren't deactivated, and haven't already been handled
        if(!control->deactivated[i] && handled_spheres.find(i) == handled_spheres.end())
            control->spheres[i].updatePos(dt);
    }
}

