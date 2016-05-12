#version 440


#define NEIGHBOR_NOT_FOUND 0xFFFFFFFF




layout (local_size_x = 8, local_size_y = 8, local_size_z = 1 ) in;

//Octree Buffer
//layout(binding = 4, r32ui) uniform uimageBuffer nodePool_children;
//layout(binding = 5, r32ui) uniform uimageBuffer nodePool_attribute;
//TODO: Has to be float not int
layout(binding = 6, r32f) uniform imageBuffer brickpool_occlusion;

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

layout(binding = 0) uniform atomic_uint nextFreeAddress_counter;
//Iterate through all nodes/tiles and allocate a Brick for each
void main()
{   
		int idx = int(gl_GlobalInvocationID.y * 1024U + gl_GlobalInvocationID.x);
		//TODO ADD Variable NODESONTHISLEVEL
		if(idx  >= numberOfNodes)
			return;

		//atomicCounterIncrement(nextFreeAddress_counter);	
		uint nodeAddress = leafOffset + idx;
	//	uint brickAdress = imageLoad(nodePool_attribute, int(nodeAddress)).x;
		uint brickAdress = nodeAddress * 27U;
		uint neighborAddress = 0U;
		
		//One side of a voxel has 9 bricks Cells
		//Brick A is are the offsets in the current voxel
		
		uint brick_A_Offset_1 = 0U;
		uint brick_A_Offset_2 = 0U;
		uint brick_A_Offset_3 = 0U;
		uint brick_A_Offset_4 = 0U;
		uint brick_A_Offset_5 = 0U;
		uint brick_A_Offset_6 = 0U;
		uint brick_A_Offset_7 = 0U;
		uint brick_A_Offset_8 = 0U;
		uint brick_A_Offset_9 = 0U;
		
		//Brick B are the offsets in the neighboring voxel
		uint brick_B_Offset_1 = 0U;
		uint brick_B_Offset_2 = 0U;
		uint brick_B_Offset_3 = 0U;
		uint brick_B_Offset_4 = 0U;
		uint brick_B_Offset_5 = 0U;
		uint brick_B_Offset_6 = 0U;
		uint brick_B_Offset_7 = 0U;
		uint brick_B_Offset_8 = 0U;
		uint brick_B_Offset_9 = 0U;
		
		
		//X-Axis - right Neighbor
		if(axis == 0U){
			//atomicCounterIncrement(nextFreeAddress_counter);
			neighborAddress = (nodeAddress*3U) + 0U;
			brick_A_Offset_1 = 2U;
			brick_A_Offset_2 = 5U;
			brick_A_Offset_3 = 8U;
			brick_A_Offset_4 = 11U;
			brick_A_Offset_5 = 14U;
			brick_A_Offset_6 = 17U;
			brick_A_Offset_7 = 20U;
			brick_A_Offset_8 = 23U;
			brick_A_Offset_9 = 26U;
			
			brick_B_Offset_1 = 0U;
			brick_B_Offset_2 = 3U;
			brick_B_Offset_3 = 6U;
			brick_B_Offset_4 = 9U;
			brick_B_Offset_5 = 12U;
			brick_B_Offset_6 = 15U;
			brick_B_Offset_7 = 18U;
			brick_B_Offset_8 = 21U;
			brick_B_Offset_9 = 24U;
		}
		//Y-Axis - Top neighbor
		if(axis == 1U){
			neighborAddress = (nodeAddress*3U) + 1U;
			brick_A_Offset_1 = 6U;
			brick_A_Offset_2 = 7U;														
			brick_A_Offset_3 = 8U;														
			brick_A_Offset_4 = 15U;														
			brick_A_Offset_5 = 16U;														
			brick_A_Offset_6 = 17U;														
			brick_A_Offset_7 = 24U;														
			brick_A_Offset_8 = 25U;														
			brick_A_Offset_9 = 26U;														
																						
			brick_B_Offset_1 = 0U;
			brick_B_Offset_2 = 1U;
			brick_B_Offset_3 = 2U;
			brick_B_Offset_4 = 9U;
			brick_B_Offset_5 = 10U;
			brick_B_Offset_6 = 11U;
			brick_B_Offset_7 = 18U;
			brick_B_Offset_8 = 19U;
			brick_B_Offset_9 = 20U;
		}
		//Z-Axis - Back Neighbor 
		if(axis == 2U){
			neighborAddress = (nodeAddress*3U) + 2U;
			brick_A_Offset_1 = 18U;
			brick_A_Offset_2 = 19U;
			brick_A_Offset_3 = 20U;
			brick_A_Offset_4 = 21U;
			brick_A_Offset_5 = 22U;
			brick_A_Offset_6 = 23U;
			brick_A_Offset_7 = 24U;
			brick_A_Offset_8 = 25U;
			brick_A_Offset_9 = 26U;
			
			brick_B_Offset_1 = 0U;
			brick_B_Offset_2 = 1U;
			brick_B_Offset_3 = 2U;
			brick_B_Offset_4 = 3U;
			brick_B_Offset_5 = 4U;
			brick_B_Offset_6 = 5U;
			brick_B_Offset_7 = 6U;
			brick_B_Offset_8 = 7U;
			brick_B_Offset_9 = 8U;
		}
		
		uint neighborNodeAddress = imageLoad(nodePool_Neighbors, int(neighborAddress)).x;
		
		if(neighborNodeAddress == 0U)
		return;

		uint neighborBrickAdress = neighborNodeAddress * 27U;

		float brick_A_Value_1 = imageLoad(brickpool_occlusion, int(brickAdress + brick_A_Offset_1)).x;
		float brick_A_Value_2 = imageLoad(brickpool_occlusion, int(brickAdress + brick_A_Offset_2)).x;
		float brick_A_Value_3 = imageLoad(brickpool_occlusion, int(brickAdress + brick_A_Offset_3)).x;
		float brick_A_Value_4 = imageLoad(brickpool_occlusion, int(brickAdress + brick_A_Offset_4)).x;
		float brick_A_Value_5 = imageLoad(brickpool_occlusion, int(brickAdress + brick_A_Offset_5)).x;
		float brick_A_Value_6 = imageLoad(brickpool_occlusion, int(brickAdress + brick_A_Offset_6)).x;
		float brick_A_Value_7 = imageLoad(brickpool_occlusion, int(brickAdress + brick_A_Offset_7)).x;
		float brick_A_Value_8 = imageLoad(brickpool_occlusion, int(brickAdress + brick_A_Offset_8)).x;
		float brick_A_Value_9 = imageLoad(brickpool_occlusion, int(brickAdress + brick_A_Offset_9)).x;
		
		//Brick B are the offsets in the neighboring voxel
		float brick_B_Value_1 = imageLoad(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_1)).x;
		float brick_B_Value_2 = imageLoad(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_2)).x;
		float brick_B_Value_3 = imageLoad(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_3)).x;
		float brick_B_Value_4 = imageLoad(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_4)).x;
		float brick_B_Value_5 = imageLoad(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_5)).x;
		float brick_B_Value_6 = imageLoad(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_6)).x;
		float brick_B_Value_7 = imageLoad(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_7)).x;
		float brick_B_Value_8 = imageLoad(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_8)).x;
		float brick_B_Value_9 = imageLoad(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_9)).x;
		
		//imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_1), vec4((brick_A_Value_1 + brick_B_Value_1) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_2), vec4((brick_A_Value_2 + brick_B_Value_2) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_3), vec4((brick_A_Value_3 + brick_B_Value_3) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_4), vec4((brick_A_Value_4 + brick_B_Value_4) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_5), vec4((brick_A_Value_5 + brick_B_Value_5) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_6), vec4((brick_A_Value_6 + brick_B_Value_6) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_7), vec4((brick_A_Value_7 + brick_B_Value_7) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_8), vec4((brick_A_Value_8 + brick_B_Value_8) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_9), vec4((brick_A_Value_9 + brick_B_Value_9) / 2.0, 0.0, 0.0, 0.0));
		
		//imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_1), vec4((brick_A_Value_1 + brick_B_Value_1) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_2), vec4((brick_A_Value_2 + brick_B_Value_2) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_3), vec4((brick_A_Value_3 + brick_B_Value_3) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_4), vec4((brick_A_Value_4 + brick_B_Value_4) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_5), vec4((brick_A_Value_5 + brick_B_Value_5) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_6), vec4((brick_A_Value_6 + brick_B_Value_6) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_7), vec4((brick_A_Value_7 + brick_B_Value_7) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_8), vec4((brick_A_Value_8 + brick_B_Value_8) / 2.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_9), vec4((brick_A_Value_9 + brick_B_Value_9) / 2.0, 0.0, 0.0, 0.0));

		imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_1), vec4((brick_A_Value_1 + brick_B_Value_1), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_2), vec4((brick_A_Value_2 + brick_B_Value_2), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_3), vec4((brick_A_Value_3 + brick_B_Value_3), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_4), vec4((brick_A_Value_4 + brick_B_Value_4), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_5), vec4((brick_A_Value_5 + brick_B_Value_5), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_6), vec4((brick_A_Value_6 + brick_B_Value_6), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_7), vec4((brick_A_Value_7 + brick_B_Value_7), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_8), vec4((brick_A_Value_8 + brick_B_Value_8), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(brickAdress + brick_A_Offset_9), vec4((brick_A_Value_9 + brick_B_Value_9), 0.0, 0.0, 0.0));
		
		imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_1), vec4((brick_A_Value_1 + brick_B_Value_1), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_2), vec4((brick_A_Value_2 + brick_B_Value_2), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_3), vec4((brick_A_Value_3 + brick_B_Value_3), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_4), vec4((brick_A_Value_4 + brick_B_Value_4), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_5), vec4((brick_A_Value_5 + brick_B_Value_5), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_6), vec4((brick_A_Value_6 + brick_B_Value_6), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_7), vec4((brick_A_Value_7 + brick_B_Value_7), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_8), vec4((brick_A_Value_8 + brick_B_Value_8), 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, int(neighborBrickAdress + brick_B_Offset_9), vec4((brick_A_Value_9 + brick_B_Value_9), 0.0, 0.0, 0.0));
		 
}































