#ifndef __PLANE_H__
#define __PLANE_H__

#include "maths_funcs.h"
#include <GL/glew.h>       // Include GLEW (or new version of GL if on Windows).
#include <GLFW/glfw3.h>

class Plane
{
public:
    //normal vector of plane
    vec3 normal;
    //reference point for plane
    vec3 point;
    //when actually visualizing the plane, we'll use these length/width
    //members to draw triangles with GL (though logically, the plane is infinite)
    float width;
    float length;
    vec3 a;
    vec3 b;

    //these are used to set limits on the plane, so that it is finite
    //there can only be a collision if the object in question is within these limits
    vec3 min_bounds;
    vec3 max_bounds;

    Plane(vec3 n, vec3 p, float w, float l) : normal(normalise(n)), point(p), width(w), length(l)
    {
        //we're gonna find two vectors 'on' the plane, i.e. 
        //two vectors orthoganal to the normal
        vec3 v1(1,0,0);
        if(cross(normal, v1) == vec3(0,0,0)) v1 = vec3(0,1,0);

        vec3 v2(0,0,1);
        if( cross(normal, v2) == vec3(0,0,0) ) v2 = vec3(0,1,0);

        //now, we'll generate the points of rectangle in vec3's
        max_bounds = point + (v1 * (width/2)) + (v2 * (length/2));
        min_bounds = point - (v1 * (width/2)) - (v2 * (length/2));

        
        a = cross(normal,v2)*w/2 + point;
        b = cross(normal,v2)*-w/2 + point;
        min_bounds -= absVal(normal*10000);
        max_bounds += absVal(normal*10000);
        
    };

    Plane() {};

    //returns vertices for two triangles that form a quad with width and height
    //used to visualize part of plane in GL
    float* getVertices()
    {
        //we're gonna find two vectors 'on' the plane, i.e. 
        //two vectors orthoganal to the normal
        vec3 v1(1,0,0);
        if(cross(normal, v1) == vec3(0,0,0)) v1 = vec3(0,1,0);

        vec3 v2(0,0,1);
        if( cross(normal, v2) == vec3(0,0,0) ) v2 = vec3(0,1,0);

        //now, we'll generate the points of rectangle in vec3's
        vec3 pp = point + (v1 * (width/2.0)) + (v2 * (length/2.0));
        vec3 np = point - (v1 * (width/2.0)) + (v2 * (length/2.0));
        vec3 nn = point - (v1 * (width/2.0)) - (v2 * (length/2.0));
        vec3 pn = point + (v1 * (width/2.0)) - (v2 * (length/2.0));

        //finally, we'll convert the rectangle calculated above into two triangle
        //and store them all as floats
        const int triangles_per_quad = 2;
        const int vertices_per_triangle = 3;
        const int floats_per_vertex = 3;
        int counter = 0;
        float* vertex_points = new float[triangles_per_quad*vertices_per_triangle*floats_per_vertex];
        vec3 triangles[triangles_per_quad][vertices_per_triangle] = {{pp,pn,nn}, {pp,np,nn}};
        for(int i = 0; i < triangles_per_quad; i++)
        {
            for(int j = 0; j < vertices_per_triangle; j++)
            {
                for(int k = 0; k < floats_per_vertex; k++)
                {
                    vertex_points[counter++] = triangles[i][j].v[k];
                }
            }
        }
        return vertex_points;
    }
};

#endif