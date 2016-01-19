#version 440




//
layout(binding = 4, r32ui) uniform uimageBuffer nodePool_children;
//The first free address for the children which will be created in the process
uniform int freeSpaceStartAddress;

uniform int numberOFThreads;

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1 ) in;


void main()
{   
	int idx = int(gl_GlobalInvocationID.y * 1024U + gl_GlobalInvocationID.x);

		if( idx >= numberOFThreads )
	    return;

	idx += freeSpaceStartAddress;
	imageStore(nodePool_children, idx,uvec4(0U, 0, 0, 0));
}































