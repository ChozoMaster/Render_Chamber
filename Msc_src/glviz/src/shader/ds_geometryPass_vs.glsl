#version 430


layout(std140, column_major) uniform Camera
{
    mat4 modelview_matrix;
	mat4 projection_matrix;
};


//layout(location = 3) in vec3 color;

#define ATTR_POSITION 0
layout(location = ATTR_POSITION) in vec3 position;

#ifdef SMOOTH
	#define ATTR_NORMAL 1
	layout(location = ATTR_NORMAL) in vec3 normal;
#endif


out block
{	
	#ifdef SMOOTH
		vec3 relativNormal;
		vec3 normal;
	#endif

	vec3 tangent;
	vec3 position;
	vec3 color;
	vec3 relativPosition;
}
Out;


void main()
{
	vec4 position_eye = vec4(position, 1.0);
	Out.relativPosition = vec3((position_eye.xyz + 1.0)/ 2.0);
	position_eye = modelview_matrix * position_eye;

	#ifdef SMOOTH
		vec4 tempNormal = vec4(normal, 0.0);
		Out.relativNormal = tempNormal.xyz;
		Out.normal = mat3(modelview_matrix) * tempNormal.xyz;	
	#endif
	Out.position = vec3(position_eye);
	
	//Out.color = color;
	Out.color = vec3(0.8, 0.8 , 0.8);
    gl_Position = projection_matrix * position_eye;
}
