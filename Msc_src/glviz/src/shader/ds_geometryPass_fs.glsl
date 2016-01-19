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

#define SHININESS 0.1

in block
{
	#ifdef SMOOTH
		vec3 relativNormal;
		vec3 normal;
		vec3 tangent;
	#else
		flat vec3 normal;
		flat vec3 relativNormal;
		flat vec3 tangent;
	#endif

	vec3 position;
	vec3 color;
	vec3 relativPosition;
}
In;


layout (location = 0) out vec4 position; 
layout (location = 1) out vec3 normal; 
layout (location = 2) out vec3 tangent; 
layout (location = 3) out vec3 bitangent;
layout (location = 4) out vec3 diffuseColor;
//layout (location = 4) out vec3 valid;  


uniform sampler2D frontTex;

void main()
{
	#ifdef SMOOTH
		vec3 normal_eye = normalize(In.normal);
	#else
		vec3 normal_eye = In.normal;
	#endif

	if (!gl_FrontFacing)
	{
	    normal_eye = -normal_eye;
	}

//	const vec3 light_eye = vec3(0.0, 0.0, 1.0);
	
  
		position = vec4(In.relativPosition, 0.0);
		normal = In.relativNormal;
		tangent = In.tangent;
		bitangent = normalize(cross(normal, tangent));

   const vec3 light_eye = vec3(0.0, 0.0, 1.0);
	
    float dif = max(dot(light_eye, normal_eye), 0.0);
    vec3 view_eye = normalize(In.position);
    vec3 refl_eye = reflect(light_eye, normal_eye);

	float spe = pow(clamp(dot(refl_eye, view_eye), 0.0, 1.0), SHININESS);
    float rim = pow(1.0 + dot(normal_eye, view_eye), 3.0);
    //vec3 a = vec3(1.0);
    //vec3 color = 0.15 * a;
	vec3 color = 0.7 * In.color;
  //  color += 0.6 * dif * a;
	color += 0.1 * spe * vec3(1.0);
    color += 0.1 * rim * vec3(1.0);	
	//bitangent = color;
	diffuseColor =  color;
	//diffuseColor = vec3(1.0, 0.0, 0.0);
}
