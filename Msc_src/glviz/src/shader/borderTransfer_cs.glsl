#version 440


#define NEIGHBOR_NOT_FOUND 0xEFFFFFFF
#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_MASK_TAG (0x00000001 << 31)
#define NODE_MASK_BRICK (0x00000001 << 30)
#define NODE_MASK_TAG_STATIC (0x00000003 << 30)
#define NODE_NOT_FOUND 0xFFFFFFFF

//Calculate the 3D index out of a linear index with a given cube size
ivec3 unravelIndex(in int idx, in int size){
	return ivec3(idx % size, (idx/size) % size, (idx /(size*size)));
}

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1 ) in;

//Octree Buffer
layout(binding = 4, r32ui) uniform uimageBuffer nodePool_children;
//layout(binding = 5, r32ui) uniform uimageBuffer nodePool_attribute;
//TODO: Has to be float not int
layout(binding = 8, r32f) uniform image3D brickpool_occlusion;

//Neighbor Buffer
//Mirrors the addresses of the nodepoolChildren times 6 and saves the neighbor address
//A NeighborTile consists of 6 Neighbors
//Offset 0 : Right Neighbor
//Offset 1 : Bottom Neighbor
//Offset 2 : Back Neighbor
layout(binding = 3, r32ui) uniform uimageBuffer nodePool_Neighbors;

uniform int leafOffset; 
uniform int numberOfNodes;

//0 - X-Axis
//1 - Y-Axis
//2 - Z-Axis
uniform uint axis;
uniform int brickDimension;
//Iterate through all nodes/tiles and allocate a Brick for each
void main()
{   
		int idx = int(gl_GlobalInvocationID.y * 1024U + gl_GlobalInvocationID.x);
		//TODO ADD Variable NODESONTHISLEVEL
		if(idx  >= numberOfNodes)
			return;

		//atomicCounterIncrement(nextFreeAddress_counter);	
		int nodeAddress = leafOffset + idx;
	//	uint nodeAddressFlag = imageLoad(nodePool_children, nodeAddress).x;
	//	nodeAddressFlag = (nodeAddressFlag & NODE_MASK_TAG);
		//if(nodeAddressFlag == 0U)
		//	return;
		//int brickX = (nodeAddress % brickDimension) * 3;
		//int brickY = ((nodeAddress % (brickDimension * brickDimension)) / brickDimension) * 3;
		//int brickZ = (nodeAddress / (brickDimension * brickDimension)) * 3;
		ivec3 brickPosition = 3 * unravelIndex(nodeAddress, brickDimension);
		int neighborAddress = 0;
		
		//One side of a voxel has 9 bricks Cells
		//Brick A is are the offsets in the current voxel
		
		ivec3 brick_A_Offset_1 = ivec3(0, 0, 0);
		ivec3 brick_A_Offset_2 = ivec3(0, 0, 0);
		ivec3 brick_A_Offset_3 = ivec3(0, 0, 0);
		ivec3 brick_A_Offset_4 = ivec3(0, 0, 0);
		ivec3 brick_A_Offset_5 = ivec3(0, 0, 0);
		ivec3 brick_A_Offset_6 = ivec3(0, 0, 0);
		ivec3 brick_A_Offset_7 = ivec3(0, 0, 0);
		ivec3 brick_A_Offset_8 = ivec3(0, 0, 0);
		ivec3 brick_A_Offset_9 = ivec3(0, 0, 0);
		
		//Brick B are the offsets in the neighboring voxel
		ivec3 brick_B_Offset_1 = ivec3(0, 0, 0);
		ivec3 brick_B_Offset_2 = ivec3(0, 0, 0);
		ivec3 brick_B_Offset_3 = ivec3(0, 0, 0);
		ivec3 brick_B_Offset_4 = ivec3(0, 0, 0);
		ivec3 brick_B_Offset_5 = ivec3(0, 0, 0);
		ivec3 brick_B_Offset_6 = ivec3(0, 0, 0);
		ivec3 brick_B_Offset_7 = ivec3(0, 0, 0);
		ivec3 brick_B_Offset_8 = ivec3(0, 0, 0);
		ivec3 brick_B_Offset_9 = ivec3(0, 0, 0);
		
		
		//X-Axis - right Neighbor
		if(axis == 0U){
			//atomicCounterIncrement(nextFreeAddress_counter);
			neighborAddress = (nodeAddress*3) + 0;
			brick_A_Offset_1 = ivec3(2, 0, 0);
			brick_A_Offset_2 = ivec3(2, 1, 0);
			brick_A_Offset_3 = ivec3(2, 2, 0);
			brick_A_Offset_4 = ivec3(2, 0, 1);
			brick_A_Offset_5 = ivec3(2, 0, 2);
			brick_A_Offset_6 = ivec3(2, 1, 1);
			brick_A_Offset_7 = ivec3(2, 1, 2);
			brick_A_Offset_8 = ivec3(2, 2, 1);
			brick_A_Offset_9 = ivec3(2, 2, 2);
			
			brick_B_Offset_1 = ivec3(0, 0, 0);
			brick_B_Offset_2 = ivec3(0, 1, 0);
			brick_B_Offset_3 = ivec3(0, 2, 0);
			brick_B_Offset_4 = ivec3(0, 0, 1);
			brick_B_Offset_5 = ivec3(0, 0, 2);
			brick_B_Offset_6 = ivec3(0, 1, 1);
			brick_B_Offset_7 = ivec3(0, 1, 2);
			brick_B_Offset_8 = ivec3(0, 2, 1);
			brick_B_Offset_9 = ivec3(0, 2, 2);
		}
		//Y-Axis - Top neighbor
		if(axis == 1U){
			neighborAddress = (nodeAddress*3) + 1;
			brick_A_Offset_1 = ivec3(0, 2, 0);
			brick_A_Offset_2 = ivec3(1, 2, 0);													
			brick_A_Offset_3 = ivec3(2, 2, 0);													
			brick_A_Offset_4 = ivec3(0, 2, 1);													
			brick_A_Offset_5 = ivec3(0, 2, 2);													
			brick_A_Offset_6 = ivec3(1, 2, 1);													
			brick_A_Offset_7 = ivec3(1, 2, 2);													
			brick_A_Offset_8 = ivec3(2, 2, 1);													
			brick_A_Offset_9 = ivec3(2, 2, 2);													
																					
			brick_B_Offset_1 = ivec3(0, 0, 0);
			brick_B_Offset_2 = ivec3(1, 0, 0);	
			brick_B_Offset_3 = ivec3(2, 0, 0);	
			brick_B_Offset_4 = ivec3(0, 0, 1);	
			brick_B_Offset_5 = ivec3(0, 0, 2);	
			brick_B_Offset_6 = ivec3(1, 0, 1);	
			brick_B_Offset_7 = ivec3(1, 0, 2);	
			brick_B_Offset_8 = ivec3(2, 0, 1);	
			brick_B_Offset_9 = ivec3(2, 0, 2);	
		}
		//Z-Axis - Front Neighbor 
		if(axis == 2U){
			neighborAddress = (nodeAddress*3) + 2;
			brick_A_Offset_1 = 	 ivec3(0, 0, 2);
			brick_A_Offset_2 = 	 ivec3(1, 0, 2);
			brick_A_Offset_3 = 	 ivec3(2, 0, 2);
			brick_A_Offset_4 = 	 ivec3(0, 1, 2);
			brick_A_Offset_5 = 	 ivec3(0, 2, 2);
			brick_A_Offset_6 = 	 ivec3(1, 1, 2);
			brick_A_Offset_7 = 	 ivec3(1, 2, 2);
			brick_A_Offset_8 = 	 ivec3(2, 1, 2);
			brick_A_Offset_9 = 	 ivec3(2, 2, 2);
			
			brick_B_Offset_1 =   ivec3(0, 0, 0);	
			brick_B_Offset_2 =   ivec3(1, 0, 0);	
			brick_B_Offset_3 =   ivec3(2, 0, 0);	
			brick_B_Offset_4 =   ivec3(0, 1, 0);	
			brick_B_Offset_5 =   ivec3(0, 2, 0);	
			brick_B_Offset_6 =   ivec3(1, 1, 0);	
			brick_B_Offset_7 =   ivec3(1, 2, 0);	
			brick_B_Offset_8 =   ivec3(2, 1, 0);	
			brick_B_Offset_9 =   ivec3(2, 2, 0);	

			//brick_A_Offset_1 = 	 0U;
			//brick_A_Offset_2 = 	 1U;
			//brick_A_Offset_3 = 	 2U;
			//brick_A_Offset_4 = 	 3U;
			//brick_A_Offset_5 = 	 4U;
			//brick_A_Offset_6 = 	 5U;
			//brick_A_Offset_7 = 	 6U;
			//brick_A_Offset_8 = 	 7U;
			//brick_A_Offset_9 = 	 8U;
			
			//brick_B_Offset_1 =   18U;	
			//brick_B_Offset_2 =   19U;	
			//brick_B_Offset_3 =   20U;	
			//brick_B_Offset_4 =   21U;	
			//brick_B_Offset_5 =   22U;	
			//brick_B_Offset_6 =   23U;	
			//brick_B_Offset_7 =   24U;	
			//brick_B_Offset_8 =   25U;	
			//brick_B_Offset_9 =   26U;	
		}
		
		int neighborNodeAddress = int(imageLoad(nodePool_Neighbors, neighborAddress).x);
		
		if(neighborNodeAddress == 0U)
			return;

		ivec3 neighborBrickPosition = 3 * unravelIndex(neighborNodeAddress, brickDimension);


		float brick_A_Value_1 = imageLoad(brickpool_occlusion, brickPosition + brick_A_Offset_1).x;
		float brick_A_Value_2 = imageLoad(brickpool_occlusion, brickPosition + brick_A_Offset_2).x;
		float brick_A_Value_3 = imageLoad(brickpool_occlusion, brickPosition + brick_A_Offset_3).x;
		float brick_A_Value_4 = imageLoad(brickpool_occlusion, brickPosition + brick_A_Offset_4).x;
		float brick_A_Value_5 = imageLoad(brickpool_occlusion, brickPosition + brick_A_Offset_5).x;
		float brick_A_Value_6 = imageLoad(brickpool_occlusion, brickPosition + brick_A_Offset_6).x;
		float brick_A_Value_7 = imageLoad(brickpool_occlusion, brickPosition + brick_A_Offset_7).x;
		float brick_A_Value_8 = imageLoad(brickpool_occlusion, brickPosition + brick_A_Offset_8).x;
		float brick_A_Value_9 = imageLoad(brickpool_occlusion, brickPosition + brick_A_Offset_9).x;
		//Check if Cell is completely empty, if so, its a fake neighbor
		//TODO BAD THINKING
		float testValue1 = brick_A_Value_1 + brick_A_Value_2 + brick_A_Value_3 + brick_A_Value_4 + brick_A_Value_5 + brick_A_Value_6 + brick_A_Value_7 + brick_A_Value_8 + brick_A_Value_9;
		//Brick B are the offsets in the neighboring voxel
		float brick_B_Value_1 = imageLoad(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_1).x;
		float brick_B_Value_2 = imageLoad(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_2).x;
		float brick_B_Value_3 = imageLoad(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_3).x;
		float brick_B_Value_4 = imageLoad(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_4).x;
		float brick_B_Value_5 = imageLoad(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_5).x;
		float brick_B_Value_6 = imageLoad(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_6).x;
		float brick_B_Value_7 = imageLoad(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_7).x;
		float brick_B_Value_8 = imageLoad(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_8).x;
		float brick_B_Value_9 = imageLoad(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_9).x;
		float testValue2 = brick_B_Value_1 + brick_B_Value_2 + brick_B_Value_3 + brick_B_Value_4 + brick_B_Value_5 + brick_B_Value_6 + brick_B_Value_7 + brick_B_Value_8 + brick_B_Value_9;


		//imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_1, vec4((0.5), 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_2, vec4((0.5), 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_3, vec4((0.5), 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_4, vec4((0.5), 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_5, vec4((0.5), 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_6, vec4((0.0), 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_7, vec4((0), 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_8, vec4((0), 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_9, vec4((0), 0.0, 0.0, 0.0));
		

		imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_1, vec4((brick_A_Value_1 + brick_B_Value_1), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_2, vec4((brick_A_Value_2 + brick_B_Value_2), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_3, vec4((brick_A_Value_3 + brick_B_Value_3), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_4, vec4((brick_A_Value_4 + brick_B_Value_4), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_5, vec4((brick_A_Value_5 + brick_B_Value_5), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_6, vec4((brick_A_Value_6 + brick_B_Value_6), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_7, vec4((brick_A_Value_7 + brick_B_Value_7), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_8, vec4((brick_A_Value_8 + brick_B_Value_8), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, brickPosition + brick_A_Offset_9, vec4((brick_A_Value_9 + brick_B_Value_9), 0.0, 0.0, 0.0));
		
		imageStore(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_1, vec4((brick_A_Value_1 + brick_B_Value_1), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_2, vec4((brick_A_Value_2 + brick_B_Value_2), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_3, vec4((brick_A_Value_3 + brick_B_Value_3), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_4, vec4((brick_A_Value_4 + brick_B_Value_4), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_5, vec4((brick_A_Value_5 + brick_B_Value_5), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_6, vec4((brick_A_Value_6 + brick_B_Value_6), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_7, vec4((brick_A_Value_7 + brick_B_Value_7), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_8, vec4((brick_A_Value_8 + brick_B_Value_8), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, neighborBrickPosition + brick_B_Offset_9, vec4((brick_A_Value_9 + brick_B_Value_9), 0.0, 0.0, 0.0));
		 return;
		 
}































