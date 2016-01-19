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

layout(std140) uniform Material
{
    vec3 color;
	float shininess;
}
material;

layout (std140) uniform Wireframe
{
	vec3 color_wireframe;
	ivec2 viewport;
};

in block
{
	#ifdef SMOOTH
		vec3 normal;
	#else
		flat vec3 normal;
	#endif

	vec3 position;
	vec3 color;
	#ifdef WIREFRAME
	    noperspective vec3 distance;
	#endif
}
In;

#define FRAG_COLOR 0
layout(location = FRAG_COLOR, index = 0) out vec4 frag_color;
uniform sampler2D frontTex;
//vec2 texCoord = gl_TexCoord[0].st;

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

	const vec3 light_eye = vec3(0.0, 0.0, 1.0);
	
    float dif = max(dot(light_eye, normal_eye), 0.0);
    vec3 view_eye = normalize(In.position);
    vec3 refl_eye = reflect(light_eye, normal_eye);

	float spe = pow(clamp(dot(refl_eye, view_eye), 0.0, 1.0),
        material.shininess);
    float rim = pow(1.0 + dot(normal_eye, view_eye), 3.0);
    
    //vec3 color = 0.15 * In.color;
	vec3 color = 0.15 * In.color;
    color += 0.6 * dif * In.color;
	color += 0.1 * spe * vec3(1.0);
    color += 0.1 * rim * vec3(1.0);

	#ifdef WIREFRAME
	    float d = min(In.distance.x, min(In.distance.y, In.distance.z));
		float i = exp2(-0.75 * d * d);
	    color = mix(color, color_wireframe, i);
	#endif

    // Gamma correction. Assuming gamma of 2.0 rather than 2.2.
	//vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	//float a = texture2D(frontTex, texCoord).a;
	//if (a > 0.0)
	//{
	//	 get start point and direction to walk through volume texture
	//	color = vec4(texture2D(frontTex, texCoord).rgb, 1.0);
	//	color = vec4(texture2D(frontTex, vec2(gl_FragCoord.x / 500.0, gl_FragCoord.y / 500.0)).rgb, 1.0);
	//	color = vec4(1.0, 0.0, 0.0, 1.0);
	//}
 //  frag_color = color;
  // frag_color = vec4( color, 1.0);
   frag_color = vec4(color, 1.0);
}
