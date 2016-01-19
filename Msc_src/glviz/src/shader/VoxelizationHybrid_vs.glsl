#version 440
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



//layout(location = 3) in vec3 color;

#define ATTR_POSITION 0
layout(location = 0) in vec3 position;

//layout ( binding = 0, offset = 0 ) uniform atomic_uint FragmentCount;
uniform int voxel_Res;

out vec3 vs_color;

void main()
{   
	vs_color = vec3(0.0, 1.0, 0.0);
	vec4 position_eye = vec4(position * (voxel_Res/2.0), 1.0);
	#ifdef DYNAMIC
		position_eye = transform * position_eye;
	#endif
	gl_Position = position_eye;

}
