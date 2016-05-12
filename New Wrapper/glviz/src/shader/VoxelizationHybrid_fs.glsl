#version 440

#define L     0.7071067811865475244008443621048490392848359376884740	//sqrt(2)/2
#define L_SQR 0.5

flat in uint dominantAxisCase;



uniform int voxel_Res;

	flat in vec3 AABBmin;
	flat in vec3 AABBmax;

smooth in vec3 color_out;
smooth in vec3 triangle_pos;

//atomic counter 
layout ( binding = 0, offset = 0 ) uniform atomic_uint FragmentCount;

//#ifdef USEVOXELFRAGMENTLIST
layout(binding = 7, rgb10_a2ui) coherent uniform uimageBuffer voxelFragmentList;
//#else
//#ifdef FORMAT_R32F
//	  layout(binding = 6, r32f) uniform  image3D voxelImage;
//	#endif

//	#ifdef FORMAT_R8
//	    layout(binding = 7, r8) uniform  image3D voxelImage;
//	#endif
//#endif;

#define FRAG_COLOR 0
layout(location = FRAG_COLOR, index = 0) out vec4 frag_color;

//Lookup up table of permutations matrices used to reverse swizzling
const mat3 unswizzleLUT[] = { mat3(0,1,0,0,0,1,1,0,0), mat3(0,0,1,1,0,0,0,1,0), mat3(1,0,0,0,1,0,0,0,1) };

float squaredLength(vec2 v){
	return dot(v,v);
}

void main()
{
	if(any(lessThanEqual(triangle_pos.xy, AABBmin.xy)) || any(greaterThanEqual(triangle_pos.xy, AABBmax.xy))){
	//if(any(lessThanEqual(position, AABBmin)) || any(greaterThanEqual(position, AABBmax))){
		discard;
	}
	float ndcDepth = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
	float z = ndcDepth / gl_FragCoord.w;
	
//	float y = gl_FragCoord.y;
	z += voxel_Res/ 2.0;
	vec3 voxelPosition1 = vec3(gl_FragCoord.x, gl_FragCoord.y, z);
	voxelPosition1 = (dominantAxisCase == 0) ? voxelPosition1.zxy : (dominantAxisCase == 1) ? voxelPosition1.yzx : voxelPosition1.xyz;

	//vec3 voxelPosition = unswizzleLUT[dominantAxisCase] * vec3(x,y,z);
	//#ifdef USEVOXELFRAGMENTLIST
		uint fragmentListIdx = atomicCounterIncrement(FragmentCount);	
		imageStore(voxelFragmentList, int(fragmentListIdx), uvec4(voxelPosition1,0));
	//#else
	//
	
	//imageStore(voxelImage, ivec3(voxelPosition), vec4(1.0 ,0.0, 0.0, 0.0));
	
	//#endif

//	float depthSpan = fwidth(gl_FragCoord.z) / 2.0;
////	float x = gl_FragCoord.x;
//	if(abs(depthSpan) < 0.5){
//		return;
//	}
	
//	float z1 = z - depthSpan;
//	float z2 = z + depthSpan;
	
//	//if(z1 >= 0.0 && z2 >= 0.0){
//	vec3 voxelPosition2 = vec3(gl_FragCoord.x, gl_FragCoord.y, z1);
//	vec3 voxelPosition3 = vec3(gl_FragCoord.x, gl_FragCoord.y, z2);
	

//	if(dominantAxisCase == 0){
//		voxelPosition2 = voxelPosition2.zxy;
//		voxelPosition3 = voxelPosition3.zxy;
//	} else {
//		if(dominantAxisCase == 1){
//			voxelPosition2 = voxelPosition2.yzx;
//			voxelPosition3 = voxelPosition3.yzx;
//		}else{
//			voxelPosition2 = voxelPosition2.xyz;
//			voxelPosition3 = voxelPosition3.xyz;
//		}
//	}
//	////vec3 voxelPosition = unswizzleLUT[dominantAxisCase] * vec3(x,y,z);
//	////#ifdef USEVOXELFRAGMENTLIST
//		fragmentListIdx = atomicCounterIncrement(FragmentCount);	
//		imageStore(voxelFragmentList, int(fragmentListIdx), uvec4(voxelPosition2,0));

//		fragmentListIdx = atomicCounterIncrement(FragmentCount);	
//		imageStore(voxelFragmentList, int(fragmentListIdx), uvec4(voxelPosition3,0));
//	//#else
//	//
	
//	//imageStore(voxelImage, ivec3(voxelPosition), vec4(1.0 ,0.0, 0.0, 0.0));
	
//	//#endif

//	//}
	discard;
}
