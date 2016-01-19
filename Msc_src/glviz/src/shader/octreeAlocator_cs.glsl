#version 430


//Nodes encoded after Crassin:
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



//layout (local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
layout (local_size_x = 8, local_size_y = 8, local_size_z = 1 ) in;
//
layout(binding = 4, r32ui) uniform volatile coherent uimageBuffer nodePool_children;
//layout(binding = 4, r32ui) uniform volatile uimageBuffer levelAddressBuffer;
//Global Counter to get the next free address for a new Tile in the Buffer
layout(binding = 0) uniform  atomic_uint nextFreeAddress_Acounter;

//The start address of the node who will receive the new Children
uniform int nodeAddress;
//The first free address for the children which will be created in the process
uniform int freeSpaceStartAddress;

uniform int numberOFThreads;
//Temporary
uniform int currentLevel;

void main()
{   
	//Load node depending on the workItem ID
	//int idx = int(gl_GlobalInvocationID.z * 1048576 + gl_GlobalInvocationID.y * 1024U + gl_GlobalInvocationID.x);
	int idx = int(gl_GlobalInvocationID.y * 1024U + gl_GlobalInvocationID.x);
	if(idx >= numberOFThreads)
	return;

	int add = nodeAddress + idx;

	uint nextAddress = imageLoad(nodePool_children , add).x;
	//Check if the node is flagged
	uint flagTest = (nextAddress & NODE_MASK_TAG);
	if(flagTest != 0U){
		//Increment address Counter if a new Tile is created
		//Remember: atomicCounterIncrement returns the last value
		//TODO Shared Counter on global Memory are Slow!
		uint nextFreeTile = atomicCounterIncrement(nextFreeAddress_Acounter);
		//Calculate absolute Address
		uint nextFreeAddress = freeSpaceStartAddress + 8U * nextFreeTile;

		//Save the Start Address of each level in a Buffer
		//imageAtomicMin(levelAddressBuffer, int(currentLevel + 1), int(nextFreeAddress));
		
		//Save value in the right bits and unflag node
		nextFreeAddress = NODE_MASK_VALUE & nextFreeAddress;
		
		//Store the new child address
		 imageStore(nodePool_children, add, uvec4(nextFreeAddress, 0, 0, 0));
		
	}
}































