#include "Sphere.h"
#include <cmath>

void Sphere::updatePos(float timestep)
{
    vec3 oldPos = pos;
    //update position vector
    pos += vel*timestep;

    //update transformation index
    trans_mat = translate(trans_mat, pos-oldPos);
}


void Sphere::setPos(vec3 newPos)
{
    vec3 oldPos = pos;
    pos = newPos;
    trans_mat = translate(trans_mat, pos-oldPos);
}

void Sphere::superSetPos(vec3 newPos)
{
    pos = newPos;
    trans_mat = translate(identity_mat4(), newPos);
}
