#version 420


flat in uint dominantAxisCase;



uniform int voxel_Res;


flat in vec4 f_AABB;
smooth in vec3 triangle_pos;
smooth in vec3 color_out;
uniform int use_conservative;

//atomic counter 
layout ( binding = 0, offset = 0 ) uniform atomic_uint FragmentCount;
//contains the interpolated colors
layout(binding = 0, r32ui) uniform  uimage3D voxel_color;
//Contains the voxel position for every fragment
layout(binding = 7, rgb10_a2ui) coherent uniform uimageBuffer voxelFragmentList;

//Lookup up table of permutations matrices used to reverse swizzling
const mat3 unswizzleLUT[] = { mat3(0,1,0,0,0,1,1,0,0), mat3(0,0,1,1,0,0,0,1,0), mat3(1,0,0,0,1,0,0,0,1) };

#define FRAG_COLOR 0
layout(location = FRAG_COLOR, index = 0) out vec4 frag_color;

//Conversion from 32Bit values to 4 8Bit Values
vec4 convRGBA8ToVec4(uint val) {
    return vec4( float((val & 0x000000FF)), 
                 float((val & 0x0000FF00) >> 8U), 
                 float((val & 0x00FF0000) >> 16U), 
                 float((val & 0xFF000000) >> 24U));
}

//vice versa
uint convVec4ToRGBA8(vec4 val) {
    return (uint(val.w) & 0x000000FF)   << 24U
            |(uint(val.z) & 0x000000FF) << 16U
            |(uint(val.y) & 0x000000FF) << 8U 
            |(uint(val.x) & 0x000000FF);
}

//conversion from 3x10 bit values to 1x32Bit value (LSB) to describe voxel position with a max resoluton of 1024
uint vec3ToUintXYZ10(uvec3 val) {
    return (uint(val.z) & 0x000003FF)   << 20U
            |(uint(val.y) & 0x000003FF) << 10U 
            |(uint(val.x) & 0x000003FF);
}

//Vice versa
uvec3 uintXYZ10ToVec3(uint val) {
    return uvec3(uint((val & 0x000003FF)),
                 uint((val & 0x000FFC00) >> 10U), 
                 uint((val & 0x3FF00000) >> 20U));
}

void imageAtomicRGBA8Avg(layout( r32ui ) coherent volatile uimage3D imgUI , ivec3 coords , vec4 val) {
	val.rgb *=255.0f; // Optimise following calculations
	uint newVal_u = convVec4ToRGBA8(val);
	uint prevStoredVal_u = 0; 
	uint curStoredVal_u;
	vec4 rval;

	// Loop as long as destination value gets changed by other threads
	while((curStoredVal_u = imageAtomicCompSwap( imgUI , coords , prevStoredVal_u , newVal_u )) != prevStoredVal_u) 
	{
		prevStoredVal_u = curStoredVal_u;
		rval = convRGBA8ToVec4( curStoredVal_u);
		rval.xyz =( rval. xyz* (rval.w)); // Denormalize
		rval += val; // Add new value
		rval.xyz /= (rval.w); // Renormalize
		newVal_u = convVec4ToRGBA8(rval);
	}
//	val = convRGBA8ToVec4(newVal_u);
 //   val.a = 255.0;
 //   newVal_u = convVec4ToRGBA8(val);
	//
  //  imageStore(imgUI, coords, uvec4(newVal_u));
}




void main()
{
	if(bool(use_conservative)){
    if( triangle_pos.x < f_AABB.x || triangle_pos.y < f_AABB.y || triangle_pos.x > f_AABB.z || triangle_pos.y > f_AABB.w )
	   discard ;
	}

	 uint fragmentListIdx = atomicCounterIncrement(FragmentCount);
	
	float x = gl_FragCoord.x;
	float y = gl_FragCoord.y;
	float z = ((gl_FragCoord.z/gl_FragCoord.w))* float(voxel_Res);
	

	vec3 voxelPosition = unswizzleLUT[dominantAxisCase] * vec3(x,y,z);
	imageStore(voxelFragmentList, int(fragmentListIdx), uvec4(voxelPosition,0));
	discard;

}
