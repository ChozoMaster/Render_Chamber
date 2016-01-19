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
#define EPSILON 0.00000001
#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_MASK_TAG (0x00000001 << 31)
#define NODE_MASK_BRICK (0x00000001 << 30)
#define NODE_MASK_TAG_STATIC (0x00000003 << 30)
#define NODE_NOT_FOUND 0xFFFFFFFF

const uvec3 childOffsets[8] = {
  uvec3(0, 0, 0),
  uvec3(1, 0, 0),
  uvec3(0, 1, 0),
  uvec3(1, 1, 0),
  uvec3(0, 0, 1),
  uvec3(1, 0, 1),
  uvec3(0, 1, 1), 
  uvec3(1, 1, 1)
  };


in vec4 color_out;

#define FRAG_COLOR 0
layout(location = FRAG_COLOR, index = 0) out vec4 frag_color;

//layout(binding = 4, r32ui) uniform uimageBuffer nodePool_children;

//layout(binding = 6, r32f) uniform imageBuffer brickpool_occlusion;
layout(binding=1) uniform sampler3D voxelImage;

//layout(binding = 8, RGBA16F) uniform  image2D frameImage;
layout(binding=0) uniform sampler2D frontTex;

uniform int resX;
uniform int resY;
uniform int level;
const float cubeDiagonal = sqrt(3.0);
// int level = 3;

const float nodeSizes[] = {1, 0.5, 0.25, 0.125, 0.0625, 0.03125, 0.015625, 0.0078125, 0.00390625, 0.001953125, 0.0009765625};

vec3 m_lightDiffuse = vec3(0.8,0.8,0.8);
vec3 m_lightAmbient = vec3(1.0,1.0,1.0);
vec3 m_lightSpecular = vec3(1.0,1.0,1.0);
vec3 lightPos = vec3(2.0,0.5,1.0);


float getIsoValue(in vec3 relativePosition);
int traverseOctree_fromLevel(in vec3 relativePosition, in uint targetLevel, out vec3 nodeBoundaryMin, out vec3 nodeBoundaryMax,out int parentsAddress, out vec3 parentsBoundaryMin, out vec3 parentsBoundaryMax);

void main()
{
	vec3 nLightPos = normalize(lightPos);
	vec4 end = color_out;
	
   vec4 start = vec4(texture2D(frontTex, vec2(gl_FragCoord.x / 1000.0, gl_FragCoord.y / 1000.0)));
 //  frag_color = end;
   if(start.a == 0 || end.a == 0)
		return;
	
	vec3 dir = end.rgb - start.rgb;
	dir = normalize(dir);
	int steps = SAMPLECOUNT;
	float stepsize = cubeDiagonal/float(steps);
	
	//vec4 c = vec4(1.0, 1.0, 1.0, 0.0);
	//bool foundIso = false;
	vec4 c = vec4(1.0, 1.0, 1.0, 0.0);
	for (int i = 0; i < steps; i++)
	{
		vec3 samplePos = start.rgb + i*stepsize*dir;
		// check if still inside cube
		if (all(greaterThanEqual(samplePos, vec3(0.0,0.0,0.0))) && all(lessThanEqual(samplePos,vec3(1.0,1.0,1.0))))
		{
			float l = getIsoValue(samplePos);
			if (l > ISOVALUE)
			{
		//	foundIso = true;
				#ifdef USE_SHADING
					for (int j = 0; j < 200; j++)
					{
						if (l-ISOVALUE > EPSILON)
						{
							samplePos = samplePos-(stepsize/(pow(2.0,(j+1.0))))*dir;
						}
						else if (l-ISOVALUE < EPSILON)
						{
							samplePos = samplePos+(stepsize/(pow(2.0,(j+1.0))))*dir;
						}
						else
						{
							break;
						}
					}
					
						float d = 2.0*stepsize;
						float dx = getIsoValue(samplePos-vec3(d,0.0,0.0)) - getIsoValue(samplePos + vec3(d,0.0,0.0));
						float dy = getIsoValue(samplePos-vec3(0.0,d,0.0)) - getIsoValue(samplePos + vec3(0.0,d,0.0));
						float dz = getIsoValue(samplePos-vec3(0.0,0.0,d)) - getIsoValue(samplePos + vec3(0.0,0.0,d));
			
						vec3 gradient = normalize(vec3(dx,dy,dz));
						vec3 diffuse = 0.8*max(0.0, dot(nLightPos, gradient))*m_lightDiffuse;
						//vec3 specular = pow(max(0.0, dot(gradient, normalize(nCamPos+nLightPos))), 60.0)*m_lightSpecular;
						vec3 ambient = 0.2*m_lightAmbient;
						c = vec4(diffuse+ambient, 1.0);
				#else
					c = vec4(0.5, 0.5, 0.5, 1.0);
				#endif
				
			}
		}	

}
//if(!foundIso)
//discard;
	//frag_color = end;
   frag_color = c;
  //  frag_color = vec4(1.0, 0.0, 0.0, 1.0);
}


float getIsoValue(in vec3 relativePosition){

	return textureLod(voxelImage, relativePosition, 0.0).x;
	//return 0.0;
}

//float getIsoValue(in vec3 relativePosition){
//	vec3 nodeBoundaryMin;
//	vec3 nodeBoundaryMax;
//	int parentsAddress;
//	vec3 parentsBoundaryMin;
//	vec3 parentsBoundaryMax;
//	float sizeOfChild = nodeSizes[level];
//	int nodeAddress = traverseOctree_fromLevel(relativePosition, level, nodeBoundaryMin, nodeBoundaryMax, parentsAddress, parentsBoundaryMin, parentsBoundaryMax);

//	if(nodeAddress == 0)
//	return 0.0;

//	int brickAddress = nodeAddress * 27;
//	vec3 brickPosition = ((relativePosition - nodeBoundaryMin)/ sizeOfChild);
//	ivec3 brickCoordinates = ivec3(brickPosition * 3.0);
//	//Calculate Address Offset to load the value;
//	int addressOffset = brickCoordinates.x + brickCoordinates.y * 3 +  brickCoordinates.z * 9;
//	return imageLoad(brickpool_occlusion, int(brickAddress + addressOffset)).x;
//}
 //Traversing function for Cone Tracing, it will traverse to the given position in the targetLevel
//the return value is the childs address
//Also the childrens and parents boundary positions and the parentsAddress are written back
//int traverseOctree_fromLevel(in vec3 relativePosition, in uint targetLevel, 
//							out vec3 nodeBoundaryMin, out vec3 nodeBoundaryMax,
//							out int parentsAddress, out vec3 parentsBoundaryMin, out vec3 parentsBoundaryMax){
//	//relative Positions of the OctreeVoxel
//	nodeBoundaryMin = vec3(0.0);
//	nodeBoundaryMax = vec3(1.0);
	
//	parentsBoundaryMin = vec3(0.0);
//	parentsBoundaryMax = vec3(1.0);
	
//	//The output Address of the Octree brick
//	int nodeAddress = 0; 
	
//	//foundOnLevel = 0;
	
//	//Sidelength on level 0
//	float sideLength = 1.0;
	
//	for(uint level = 0; level < targetLevel; ++level){
//		//Load the childs Address out of the NodePool
//		uint nextNode = imageLoad(nodePool_children, nodeAddress).x;
//		//Extract the Address using the defined Mask
//		uint childAddress = nextNode & NODE_MASK_VALUE;
//		//If the child address is zero, it means we found have to further split the node and we are finished in this step
//		if(childAddress == 0U){
//			nodeAddress = 0;
//			return nodeAddress;
//			//foundOnLevel = level;
//		//	break;
//		}
//		//If not we go one level deeper
//		//Remember an Octree node consists of 2x2x2 Children, 
//		//which in our implementation lie in 8 consecutive addresses.
//		//The following equations address these children according to the relative position of the Fragment	
//		uvec3 offsetVec = uvec3(2.0 * (relativePosition ));


//		uint offsetU = offsetVec.x + offsetVec.y * 2U + offsetVec.z * 4U;
		
//		//restart recursion with the the childs address
//		//old Address + child Offset 
//		parentsAddress = nodeAddress;
//		nodeAddress = int(childAddress + offsetU);

//		//Every Iteration the sideLength devided by half
//		sideLength = sideLength / 2.0;
		
//		parentsBoundaryMin = nodeBoundaryMin;
//		parentsBoundaryMax = nodeBoundaryMax;
		
//		//calculate Voxel Boundarys on the actual level, in the range of 0-1
//		nodeBoundaryMin +=  vec3(childOffsets[offsetU]) * vec3(sideLength);
//		nodeBoundaryMax = nodeBoundaryMin + vec3(sideLength);
		
//		//The actual relative position of the voxel will be the inverse in the next level,
//		//if the relative position is over 0.5, as long as it is under 0.5 it will still 
//		//remain in the same Octree Sector
//		//Remember offsetVec is already rounded so it is either 0 or 1
		
//		relativePosition = 2.0 * relativePosition - vec3(offsetVec);
//	}
	
//	//The Method will return the Address of the Node who has no children yet
//	return nodeAddress;
//}
  
