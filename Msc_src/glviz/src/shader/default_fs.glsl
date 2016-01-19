#version 420
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


in vec4 color_out;

#define FRAG_COLOR 0
layout(location = FRAG_COLOR, index = 0) out vec4 frag_color;
//layout(location = 1) out vec4 color;
//layout(binding = 8, RGBA16F) uniform  image2D frameImage;

void main()
{


	//imageStore(frameImage, vec2(gl_FragCoord.x, gl_FragCoord.y), color_out);
   frag_color = color_out;
  //  frag_color = vec4(1.0, 0.0, 0.0, 1.0);
	//color = vec4(1.0, 0.0, 0.0, 1.0);
  // frag_color = vec4( material.color, 1.0);
}
