#version 410

layout (location = 0) in vec3 vtxPosition;
layout (location = 2) in vec3 vtxNormal;
layout (location = 1) in vec3 vtxColor;
layout (location = 3) in vec2 vtxTextCoor;
uniform mat4 proj_mat, view_mat, model_mat;

out vec3 opNormal;
out vec3 opPosition;
out vec2 opTextCoor;
out vec3 opColor;

void main () 
{
	gl_PointSize = 10.0; // Set point size
	opNormal = normalize(transpose(inverse(mat3(view_mat * model_mat))) * vtxNormal);
	//opNormal = normalize(vec3(view_mat * model_mat * vec4(vtxNormal,1.0)));
	opPosition = vec3(view_mat * model_mat* vec4(vtxPosition, 1.0));
	opTextCoor = vtxTextCoor;
	opColor = vtxColor;
	gl_Position = proj_mat * view_mat * model_mat * vec4(vtxPosition, 1.0);
}
