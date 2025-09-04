#version 410


// Define INPUTS from fragment shader
uniform mat4 view_mat;

layout (location = 2) in vec3 vtxNormal;

// These come from the VAO for texture coordinates.
//in vec2 texture_coords;

// And from the uniform outputs for the textures setup in main.cpp.
uniform sampler2D texture00;
uniform sampler2D texture01;

uniform int phong_exponent;
uniform vec3 point_light_pos;
uniform vec3 dir_light_vec;

//if nonzero, use specular effects; if zero, don't use specular effects
uniform int use_spec;
//if nonzero, use diffuse effects; if zero, don't use diffuse effects
uniform int use_diff;
//if nonzero, use texture as diff. albedo; if zero, don't
uniform int use_tex;

//if nonzero, use roughness map. if zero, don't
uniform int use_rough;

in vec3 opPosition;
in vec3 opNormal;
in vec2 opTextCoor;
in vec3 opColor;

out vec4 fragment_color; //RGBA color


vec3 directional(vec3 diffColor, vec3 light, vec3 rough)
{
    
    vec3 normal = normalize(opNormal);
    vec3 lightVec = normalize(light);
    //todo: pass as uniform
    vec3 camPos = vec3(0,0,5.0);
    vec3 diffContrib;
    if(use_diff != 0)
    {
        //diffuse calculations
        float diffuse = max(dot(lightVec, normal), 0);
        vec3 diff = diffColor * diffuse;
        float diff_coef = 1;
        diffContrib = diff_coef * diff;
    }
    else
    {
        diffContrib = vec3(0.0,0.0,0.0);
    }
    vec3 specContrib;
    if(use_spec != 0)
    {
        //specular calculations
        vec3 viewVec = normalize(camPos - opPosition);
        vec3 halfVec = normalize(lightVec + viewVec);
        float spec = max(dot(halfVec, normal),0.0);
        vec3 specColor = vec3(1.0,1.0,1.0);
        float spec_coef = 0.5;
        float phong_coeff = 1.0;
        if(use_rough != 0) phong_coeff = 1 - 2*rough.x + 1.1;
        float phongEx = max(ceil(phong_exponent * phong_coeff),2.0);
        spec = pow(spec, phongEx);
        specContrib = spec_coef*specColor*spec;
    }
    else
    {
        specContrib = vec3(0.0,0.0,0.0);
    }

    vec3 lightContrib = diffContrib + specContrib;
    return lightContrib;
}

vec3 ambient(vec3 ambColor)
{
    //ambient calculations
    float amb_coef = .1;
    vec3 ambContrib = amb_coef*ambColor;
    return ambContrib;
}
vec3 point(vec3 diffColor, vec3 lightPos, vec3 rough)
{
    //todo: pass as uniform
    vec3 camPos = vec3(0,0,5.0);
    
    
    vec3 light = vec3(lightPos - opPosition);
    vec3 normal = normalize(opNormal);
    vec3 lightVec = normalize(light);
    vec3 diffContrib;
    if(use_diff != 0)
    {
        //diffuse calculations
        float diffuse = max(dot(lightVec, normal), 0);
        vec3 diff = diffColor * diffuse;
        float diff_coef = 1;
        diffContrib = diff_coef * diff;
    }
    else
    {
        diffContrib = vec3(0.0,0.0,0.0);
    }

    
    vec3 specContrib;
    if(use_spec != 0)
    {
        //specular calculations
        vec3 viewVec = normalize(camPos - opPosition);
        vec3 halfVec = normalize(lightVec + viewVec);
        float spec = max(dot(halfVec, normal),0.0);
        vec3 specColor = vec3(1.0,1.0,1.0);
        float spec_coef = 0.5;
        float phong_coeff = 1.0;
        if(use_rough != 0) phong_coeff = 1 - 2*rough.x + 1.1;
        float phongEx = max(ceil(phong_exponent * phong_coeff),2.0);
        spec = pow(spec, phongEx);
        specContrib = spec_coef*specColor*spec;
    }
    else
    {
        specContrib = vec3(0.0,0.0,0.0);
    }
    
    vec3 lightContrib = diffContrib + specContrib;
    return lightContrib;
}


void main () 
{
    vec4 texColor = texture(texture00, opTextCoor);
    vec4 roughColor = texture(texture01, opTextCoor);
    vec3 diffColor;
    if(use_tex != 0)
    {
        diffColor = texColor.rgb;
    }
    else
    {
        diffColor = opColor;
    }
    vec3 lightContrib = directional(diffColor, dir_light_vec, roughColor.xyz) + point(diffColor, point_light_pos, roughColor.xyz) + ambient(diffColor);
    fragment_color = vec4(lightContrib, 1.f); 
    //fragment_color = vec4(vec3(1.0,0,1.0), 1.f);
}
