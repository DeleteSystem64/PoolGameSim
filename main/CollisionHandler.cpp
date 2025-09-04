#include "CollisionHandler.h"
#include <iostream>

using namespace std;

/************************************************SPHERE-PLANE****************************************************/
bool CollisionHandler::willCollide(Sphere& sphere, const Plane& plane, float time_step)
{
    //if there's no velocity there is no collision
    //or if the velocity is perpindicular to surface, there is no collision
    if(dot(plane.normal, sphere.vel) == 0) return false;

    //if the sphere is outside the boundaries of the plane, there is no collision
    
    for(int i = 0; i < 3; i++)
    {
        if(sphere.pos.v[i] > plane.max_bounds.v[i] || sphere.pos.v[i] < plane.min_bounds.v[i])
        {
            return false;
        }
    }
    vec3 newPos = sphere.pos + sphere.vel*time_step;

    //get distance between the sphere's new center and plane
    float D = -1.0*dot(plane.normal, plane.point);
    float distance = dot(plane.normal, newPos);
    distance += D;

    //if distance is negative, sphere has passed through plane, so there's a collision
    //if the distance is less than the radius, then there's a collision
    //
    return distance <= sphere.radius_length;
}

float CollisionHandler::getCollisionTime(const Sphere& sphere, const Plane& plane, float time_step)
{
    float D = -1.0*dot(plane.normal,plane.point);
    float denominator = dot(plane.normal,sphere.vel);

    //a special case is when the dot product of plane's normal and sphere's velocity is 0
    //(i.e. sphere is moving parallel to plane)
    //we don't do the equation to avoid a divide by zero. Instead, we'll return a negative number,
    //because this will be intepreted as "no collision", which is all we need
    if(fabs(denominator) < 1e-6)
    {
        return -1;
    }
    float hit_time = (sphere.radius_length*length(plane.normal) - dot(plane.normal,sphere.pos) - D) / denominator;
   
   
    return hit_time;
} 

void CollisionHandler::handleCollision(Sphere& sphere, Plane& plane)
{
    sphere.vel = (sphere.vel - plane.normal*2.0*dot(plane.normal , sphere.vel))*inelasticity;
}

/************************************************SPHERE-SPHERE***************************************************/
bool CollisionHandler::willCollide(Sphere& sphere0, Sphere& sphere1, float time_step, float& alpha)
{
    vec3 relative_position = sphere0.pos - sphere1.pos;
    
    vec3 relative_velocity = sphere0.vel - sphere1.vel;
    float combined_radius = sphere0.radius_length + sphere1.radius_length;

    //if spheres are already overlapping, separate them, and return an immediate collision
    if(length(relative_position) < combined_radius + 1e-6)
    {
        separateSpheres(sphere0, sphere1);
        alpha=0;
        return true;
    }

    float relative_speed_sq = pow(length(relative_velocity), 2);
    float relative_position_dot_velocity = dot(relative_position, relative_velocity);
    float distance_sq = pow(length(relative_position), 2);
    float radical = pow(relative_position_dot_velocity, 2) 
                        - relative_speed_sq * (distance_sq - pow(combined_radius, 2));

    if (radical < -1e-6 || relative_speed_sq < 1e-6) return false;

    radical = sqrt(fmax(0.0f, radical)); 
    float time1 = (-relative_position_dot_velocity + radical) / relative_speed_sq;
    float time2 = (-relative_position_dot_velocity - radical) / relative_speed_sq;

    float alpha1 = time1 / time_step;
    float alpha2 = time2 / time_step;

    //check for collision between 0-1
    if (alpha1 > 0 && alpha1 <= 1) {
        if (alpha2 > 0 && alpha2 <= 1 && alpha2 < alpha1) {
            alpha = alpha2;
            return true;
        }
        alpha = alpha1;
        return true;
    }
    else if (alpha2 > 0 && alpha2 <= 1) {
        alpha = alpha2;
        return true;
    }
    return false;
}

void CollisionHandler::handleCollision(Sphere& sphere1, Sphere& sphere2, float time_step)
{
    vec3 system_center_of_mass = (sphere1.vel*sphere1.mass + sphere2.vel*sphere2.mass)/(sphere1.mass + sphere2.mass);
    vec3 relative_vel = sphere2.vel - sphere1.vel;

    vec3 surface_normal = normalise(sphere1.pos - sphere2.pos);
    vec3 reflected_rel_vel = relative_vel - surface_normal*2*dot(surface_normal,relative_vel);

    sphere1.vel = system_center_of_mass - reflected_rel_vel*(sphere2.mass /(sphere1.mass + sphere2.mass)) * inelasticity;
    sphere2.vel = system_center_of_mass + reflected_rel_vel*(sphere1.mass /(sphere1.mass + sphere2.mass)) * inelasticity;

    sphere1.updatePos(time_step);
    sphere2.updatePos(time_step);
}

void CollisionHandler::separateSpheres(Sphere& sphere0, Sphere& sphere1)
{
    //distance between the spheres' centers
    vec3 relative_position = sphere1.pos - sphere0.pos;
    float distance = length(relative_position);
   
    float combined_radius = sphere0.radius_length + sphere1.radius_length;
    
    //calculate the overlap distance
    float overlap = combined_radius - distance;
    //unit vector in the direction from sphere0 to sphere1
    vec3 direction = normalise(relative_position);

    //movesphere0 and sphere1 apart
    sphere0.setPos( sphere0.pos - direction * (overlap / 2));
    sphere1.setPos( sphere1.pos + direction * (overlap / 2));
    
}