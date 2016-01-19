#version 430
// This file is part of GLviz.
//
// Copyright (C) 2014 by Sebastian Lipponer.
// 
// GLviz is free software: you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// GLviz is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with GLviz. If not, see <http://www.gnu.org/licenses/>.

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
		vec3 normal;
	#endif
	vec3 position;
	vec3 color;
}
Out;

out vec3 voxelSpacePosition;

out vec3 relativPosition;
out vec3 relativNormal;
#ifdef DYNAMIC
//	uniform mat4 transform;
#endif

void main()
{
	voxelSpacePosition = position.xyz * 512;
	vec4 position_eye = vec4(position, 1.0);
	#ifdef DYNAMIC
		position_eye = transform * position_eye;
	#endif
		relativPosition = vec3((position_eye.xyz + 1.0)/ 2.0);
		position_eye = modelview_matrix * position_eye;

	#ifdef SMOOTH
		vec4 tempNormal = vec4(normal, 0.0);
		#ifdef DYNAMIC
			tempNormal = transform * tempNormal;
		#endif
		relativNormal = tempNormal.xyz;
		Out.normal = mat3(modelview_matrix) * tempNormal.xyz;
		
	#endif
	Out.position = vec3(position_eye);
	
	//Out.color = color;
	Out.color = vec3(1.0);
    gl_Position = projection_matrix * position_eye;
 // gl_Position = vec4(position, 1.0);
}
