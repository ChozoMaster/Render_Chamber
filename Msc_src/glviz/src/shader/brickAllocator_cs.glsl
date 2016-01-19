#version 440


//Nodes encoded after Crassin et al.:
//One Node conists of 64Bits (Luminance + Alpha Channel)

//Luminance Channel (32Bits):
//30 Bits encode child adress for inner nodes, zero means there is no child
//1 Bit: Max subdivision flag, indicates whether the node is refined to a maximum
//1 Bit : Node Type flag, indicates whether content is a RGBA8 value or described by a brick

//Alpha Channel (32Bits):
//Either constant color 3x10Bits
//Or Brick Adress
//Masked to read and save the values
#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_MASK_TAG (0x00000001 << 31)
#define NODE_MASK_BRICK (0x00000001 << 30)
#define NODE_MASK_TAG_STATIC (0x00000003 << 30)
#define NODE_NOT_FOUND 0xFFFFFFFF

layout(binding = 4, r32ui) uniform uimageBuffer nodePool_children;
layout(binding = 5, r32ui) uniform uimageBuffer nodePool_attribute;
layout(binding = 6, r32f) uniform imageBuffer brickpool_occlusion;

layout(binding = 0) uniform atomic_uint nextFreeBrickAddress;
uniform int numberOfNodes; 


layout (local_size_x = 8, local_size_y = 8, local_size_z = 1 ) in;
//Iterate through all nodes/tiles and allocate a Brick for each
void main()
{   
		int idx = int(gl_GlobalInvocationID.y * 1024U + gl_GlobalInvocationID.x);

		//TODO ADD Variable SUMOFNODE
		if(idx  >= numberOfNodes)
			return;
		
	//	for(uint i = 0; i < 8; i++){
			//Get the next free adress, times 9U because a brick has 9 pixels
			
			uint childrenValue = imageLoad(nodePool_children, idx).x;
			if(childrenValue == 0U)
				return; 

			uint freeAddr = atomicCounterIncrement(nextFreeBrickAddress) * 9U;
			memoryBarrier();
			// Store brick-pointer
			imageStore(nodePool_attribute, idx, uvec4(freeAddr, 0, 0, 0));
			//for(uint k = 0; k < 9; k++){
			//	imageStore(brickpool_occlusion, int(freeAddr), vec4(0.0));
			//}
		//}
		
	
}































