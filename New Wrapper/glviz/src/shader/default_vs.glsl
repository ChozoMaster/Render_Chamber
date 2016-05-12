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


layout(location = 3) in vec3 color;

#define ATTR_POSITION 0
layout(location = ATTR_POSITION) in vec3 position;

out vec4 color_out;

void main()
{
	vec4 position_eye = modelview_matrix * vec4(position, 1.0);
//	Out.position = vec3(position_eye);
	color_out = vec4(color, 1.0);
    gl_Position = projection_matrix * position_eye;
 // gl_Position = position_eye;
//  gl_Position =  vec4(position, 1.0);
}
