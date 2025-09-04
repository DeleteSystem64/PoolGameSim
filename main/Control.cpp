

#include "Control.h"
#include <cassert>

Control::Control()
{
    float height = 15 + (3/sqrt(2));
    float width = 34 + (3/sqrt(2));
    Plane fixme[] = {
        Plane(vec3(0,-30,0), vec3(-19,height,0), 30.0, 30.0), 
        Plane(vec3(0,-30,0), vec3(19, height,0), 30.0, 30.0),
        Plane(vec3(0,30,0), vec3(-19,-height,0), 30.0, 30.0), 
        Plane(vec3(0,30,0), vec3(19,-height,0), 30.0, 30.0),
        Plane(vec3(30,0,0), vec3(-width,0,0), 30.0, 30.0), 
        Plane(vec3(-30,0,0), vec3(width,0,0), 30.0, 30.0),
    };
    for(int i =0; i < num_planes; i++)
    {
        planes[i] = fixme[i];
    }

    height = 15 + (3/sqrt(2));
    width = 30 + (3/sqrt(2));

    pockets[0] = Plane(vec3(0,-30,0), vec3(0,height,0), 8.0, 8.0);
    pockets[1] = Plane(vec3(0,30,0), vec3(0,-height,0), 8.0, 8.0);
    pockets[2] = Plane(vec3(1,-1,0), vec3(-34,height,0), 8.0, 8.0);
    pockets[3] = Plane(vec3(-1,-1,0), vec3(34,height,0), 8.0, 8.0);
    pockets[4] = Plane(vec3(-1,1,0), vec3(34,-height,0), 8.0, 8.0);
    pockets[5] = Plane(vec3(1,1,0), vec3(-34,-height,0), 8.0, 8.0);
    
    for(int i = 0; i < num_spheres; i++)
    {
        spheres[i] = Sphere(1);
        point_counts[i] = loadOBJ("sphere5.obj", sphere_vaos[i]);
    }

    table_point_count = loadOBJ("pooltable.obj", table_vao);

    loadMouseVec(mouse_vec_vao);
}
unsigned int cueball_texture;
unsigned int atlas;
void Control::loadTextures(GLuint shader)
{
    GLuint tex00;
    tex_locs[0] = glGetUniformLocation (shader, "texture00");
    glUniform1i (tex_locs[0], 0);
    glActiveTexture (GL_TEXTURE0);
    assert (load_texture ("cueball.jpg", &tex_locs[0]));

    tex_locs[1] = glGetUniformLocation (shader, "texture00");
    glUniform1i (tex_locs[1], 0);
    glActiveTexture (GL_TEXTURE0);
    assert (load_texture ("solid.jpg", &tex_locs[1]));

    tex_locs[2] = glGetUniformLocation (shader, "texture00");
    glUniform1i (tex_locs[2], 0);
    glActiveTexture (GL_TEXTURE0);
    assert (load_texture ("stripe.jpg", &tex_locs[2]));

    tex_locs[3] = glGetUniformLocation (shader, "texture00");
    glUniform1i (tex_locs[3], 0);
    glActiveTexture (GL_TEXTURE0);
    assert (load_texture ("8_ball.jpg", &tex_locs[3]));

    table_tex_loc = glGetUniformLocation (shader, "texture00");
    glUniform1i (table_tex_loc, 0);
    glActiveTexture (GL_TEXTURE0);
    assert (load_texture ("pooltable.jpg", &table_tex_loc));



}

void Control::deactivateSphere(int index)
{
    deactivated[index] = true;
}
int Control::loadOBJ(char* filename, GLuint& vao)
{
    float* vp;
    float* tex_coords;
    float* normals;
    int point_count;
    load_obj_file(filename, vp, tex_coords, normals, point_count);

    // VAO -- vertex attribute objects bundle the various things associated with vertices
    
    glGenVertexArrays (1, &vao);   // generating and binding is common pattern in OpenGL
    glBindVertexArray (vao);       // basically setting up memory and associating it


    //VERTEX POINTS
    GLuint points_vbo;
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, point_count*3*sizeof(GLfloat), vp, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    //NORMALS
    GLuint normals_vbo;
    glGenBuffers(1, &normals_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
    glBufferData(GL_ARRAY_BUFFER, point_count*3*sizeof(GLfloat), normals, GL_STATIC_DRAW);
    
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);

    //TEXTURE COORDINATES
    GLuint tc_vbo;
    glGenBuffers(1, &tc_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, tc_vbo);
    glBufferData(GL_ARRAY_BUFFER, point_count*2*sizeof(GLfloat), tex_coords, GL_STATIC_DRAW);
    
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(3);

    return point_count;

}

void Control::loadMouseVec(GLuint& vao)
{
    float vp[] = {
        0, 0, 0,
        0, 1, 0
    };
    
    int point_count = 2;
    

    // VAO -- vertex attribute objects bundle the various things associated with vertices
    
    glGenVertexArrays (1, &vao);   // generating and binding is common pattern in OpenGL
    glBindVertexArray (vao);       // basically setting up memory and associating it


    //VERTEX POINTS
    GLuint points_vbo;
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, point_count*3*sizeof(GLfloat), vp, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

}

void Control::drawSphere(int sphere_index, mat4& model, GLuint shader)
{
    glActiveTexture(GL_TEXTURE0);

    //cue ball
    if(sphere_index == 0)
    {
        glBindTexture(GL_TEXTURE_2D, tex_locs[0]);
    }
    //solids
    else if(sphere_index <= 7)
    {
        glBindTexture(GL_TEXTURE_2D, tex_locs[1]);
    }
    //8-ball
    else if(sphere_index == 8 )
    {
        glBindTexture(GL_TEXTURE_2D, tex_locs[3]);
    }
    //stripes
    else
    {
        glBindTexture(GL_TEXTURE_2D, tex_locs[2]);
    }

    //load in the transform matrix into model matrix
    model = spheres[sphere_index].trans_mat;
    int model_mat_location = glGetUniformLocation (shader, "model_mat");
    glUniformMatrix4fv (model_mat_location, 1, GL_FALSE, model.m);
    //now draw the triangles
    glBindVertexArray(sphere_vaos[sphere_index]);
    glDrawArrays(GL_TRIANGLES, 0, 3*point_counts[sphere_index]);
    glBindVertexArray(0);
}

void Control::drawTable(GLuint shader)
{
    mat4 model = scale(identity_mat4(), vec3(.5,.5,.5));
    model = translate(model, vec3(0,-0.5,0));
    int model_mat_location = glGetUniformLocation (shader, "model_mat");
    glUniformMatrix4fv (model_mat_location, 1, GL_FALSE, model.m);
    glBindVertexArray(table_vao);
    
    glBindTexture(GL_TEXTURE_2D, table_tex_loc);
    glDrawArrays(GL_TRIANGLES, 0, table_point_count*3);
    glBindVertexArray(0);
}

//
void Control::setUp8BallGame()
{
    //We first place the cue ball in its starting position
    spheres[0].setPos(cueball_starting_pos);
    //Then, we place the other 15 balls in a pyramid formation
    int num_rows = 5;

    //we assume here that all balls are the same size (same radius)
    float radius = spheres[1].radius_length;

    //used so that the balls start scrambled
    
    int indices[] = {9,7,12,15,8,1,6,10,3,14,11,2,13,4,5};

    //used to place the balls at a certain place on the table
    //rather than just the center
    float x_offset = -12;
    int counter = 0;
    //setting up balls in pyramid
    for(int i = 0; i < num_rows; i++)
    {
        float y_start = -i*radius;
        float y_increment = 2*radius+.1;
        for(int j = 0; j < i+1; j++)
        {
            float x = -i*sqrt(3.0)*radius+.1 + x_offset;
            float y = y_start + y_increment*j;
            //position in pyramid
            spheres[indices[counter++]].setPos(vec3(x,y,0));
            //now translate to correct place on table
            
        }
        
    }

    
}

void Control::drawMouseVector(float mousex, float mousey, vec3 start, GLuint shader, bool alt_mouse_vec)
{
    
    vec3 end = vec3(mousex, mousey, 0);
    //scale mouse_vector proportionally to how much power is behind the ball
    float mag = length(end-start);
    //sign here
    mat4 model;
    if(alt_mouse_vec)
        model = scale(identity_mat4(), vec3(0,-mag*.05,0));
    else
        model = scale(identity_mat4(), vec3(0,-mag*.05,0));

    //rotate mouse_vector so that it points in direction of motion
    //finding angle of vector
    
    vec3 default_vec = vec3(0,1,0);
    //blah
    vec3 angle_vec = end-start;
    
    //sign of angle
    vec3 cro = cross(angle_vec, default_vec);
    float sign;
    if(cro.v[2] > 0) sign = 1;
    else sign = -1;
    float cosTheta = dot(angle_vec, default_vec) / (length(angle_vec) * length(default_vec));
    float thetaRadians = acos(cosTheta);
    float thetaDeg = thetaRadians*57.296*sign;
    model = rotate_z_deg(model, thetaDeg);
    

    //translate mouse vector so that it starts at the cueball
    model = translate(model, vec3(spheres[0].pos.v[0], spheres[0].pos.v[1], 0));

    int model_mat_location = glGetUniformLocation (shader, "model_mat");
    glUniformMatrix4fv (model_mat_location, 1, GL_FALSE, model.m);
    //now drawing the line
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(mouse_vec_vao);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}


void Control::reset()
{
    for(int i = 0; i < num_spheres; i++)
    {
        spheres[i].vel = vec3(0,0,0);
        deactivated[i] = false;
    }
    setUp8BallGame();
}
