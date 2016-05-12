#version 440

#define L     0.7071067811865475244008443621048490392848359376884740	//sqrt(2)/2
#define L_SQR 0.5

flat in uint dominantAxisCase;



uniform int voxel_Res;

	flat in vec2 AABBmin;
	flat in vec2 AABBmax;

smooth in vec3 color_out;
uniform int use_conservative;
smooth in vec3 baryCoords;
smooth in vec3 triangle_pos;
flat in vec3 e0, e1, e2;
flat in float signedDistance_e0, signedDistance_e1, signedDistance_e2;

//Voxel Texture
	#ifdef FORMAT_R32F
	    layout(binding = 7, r32f) uniform  image3D voxelImage;
	#endif

	#ifdef FORMAT_R8
	    layout(binding = 7, r8) uniform  image3D voxelImage;
	#endif



#define FRAG_COLOR 0
layout(location = FRAG_COLOR, index = 0) out vec4 frag_color;

//Lookup up table of permutations matrices used to reverse swizzling
const mat3 unswizzleLUT[] = { mat3(0,1,0,0,0,1,1,0,0), mat3(0,0,1,1,0,0,0,1,0), mat3(1,0,0,0,1,0,0,0,1) };

float squaredLength(vec2 v){
	return dot(v,v);
}

void main()
{
	
	if(any(lessThanEqual(triangle_pos.xy, AABBmin)) || any(greaterThanEqual(triangle_pos.xy, AABBmax))){
	//if(any(lessThanEqual(position, AABBmin)) || any(greaterThanEqual(position, AABBmax))){
		discard;
	}
	//float squaredLength_e0 = squaredLength(e0.xy);
	//float squaredLength_e1 = squaredLength(e1.xy);
	//float squaredLength_e2 = squaredLength(e2.xy);
	//vec3 baryCoords_sqr = baryCoords * baryCoords;
	//
	//float sqrDistance_v0 = baryCoords_sqr.z * squaredLength_e2 + baryCoords_sqr.y * squaredLength_e0 - 2.0 * baryCoords.z * baryCoords.y * dot(e2.xy, e0.xy);
	//float sqrDistance_v1 = baryCoords_sqr.x * squaredLength_e0 + baryCoords_sqr.z * squaredLength_e1 - 2.0 * baryCoords.x * baryCoords.z * dot(e0.xy, e1.xy);
	//float sqrDistance_v2 = baryCoords_sqr.y * squaredLength_e1 + baryCoords_sqr.x * squaredLength_e2 - 2.0 * baryCoords.y * baryCoords.x * dot(e1.xy, e2.xy);
	//
	//float distance_e0 = baryCoords.z * signedDistance_e0;
	//float distance_e1 = baryCoords.x * signedDistance_e1;
	//float distance_e2 = baryCoords.y * signedDistance_e2;
	//
	//float diagonal = L / float(voxel_Res);
	//float diagonal_sqr = L_SQR / float(voxel_Res);
	//if(sqrDistance_v0 > diagonal_sqr && sqrDistance_v1 > diagonal_sqr && sqrDistance_v2 > diagonal_sqr && distance_e0 > diagonal && distance_e1 > diagonal && distance_e2 > diagonal){
	//if(sqrDistance_v0 > diagonal_sqr && sqrDistance_v1 > diagonal_sqr && sqrDistance_v2 > diagonal_sqr){
	//if(distance_e0 > diagonal && distance_e1 > diagonal && distance_e2 > diagonal){
		
	//}
	float ndcDepth = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
	float clipDepth = ndcDepth / gl_FragCoord.w;
	//gl_FragColor = vec4((clipDepth * 0.5) + 0.5); 
	float x = gl_FragCoord.x;
	float y = gl_FragCoord.y;
	//float z = (gl_FragCoord.z*voxel_Res) + float(voxel_Res/ 2.0);
	float halfVoxel = voxel_Res/ 2.0;
	//float z = (clipDepth * 0.5) + halfVoxel;
	float z = (clipDepth) + halfVoxel;
	//float z = 1;
	vec3 voxelPosition = unswizzleLUT[dominantAxisCase] * vec3(x,y,z);
	imageStore(voxelImage, ivec3(voxelPosition), vec4(1.0 ,0.0, 0.0, 0.0));
	discard;
	
	
	//if(bool(use_conservative)){
    //if( triangle_pos.x < f_AABB.x || triangle_pos.y < f_AABB.y || triangle_pos.x > f_AABB.z || triangle_pos.y > f_AABB.w )
	//   discard ;
	//}
	//float x = gl_FragCoord.x;
	//float y = gl_FragCoord.y;
	//float z = ((gl_FragCoord.z/gl_FragCoord.w))* float(voxel_Res);
	//vec3 voxelPosition = unswizzleLUT[dominantAxisCase] * vec3(x,y,z);
	//imageStore(voxelImage, ivec3(voxelPosition), vec4(1.0 ,0.0, 0.0, 0.0));
	//discard;

}
