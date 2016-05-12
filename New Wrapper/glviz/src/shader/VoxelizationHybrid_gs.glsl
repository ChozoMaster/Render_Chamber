#version 420
#define L     0.7071067811865475244008443621048490392848359376884740	//sqrt(2)/2
//#define CUTOFF 6
flat out uint dominantAxisCase;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int voxel_Res;
uniform float cutoff;
smooth out vec3 triangle_pos;
//smooth out vec3 baryCoords;


in vec3 vs_color[];
smooth out vec3 color_out;

flat out vec3 AABBmin;
flat out vec3 AABBmax;

layout ( binding = 0, offset = 0 ) uniform atomic_uint FragmentCount;

//#ifdef USEVOXELFRAGMENTLIST
layout(binding = 6, rgb10_a2ui) uniform uimageBuffer voxelFragmentList1;
//#else
//#ifdef FORMAT_R32F
  //  layout(binding = 6, r32f) uniform  image3D voxelImage1;
//	#endif

//	#ifdef FORMAT_R8
//	    layout(binding = 7, r8) uniform  image3D voxelImage;
//	#endif
//#endif
//Contains the voxel position for every fragment


//After rawendall we use the 2D area of the triangle after dominant axis transformation
// True leads to triangle parallel triangle voxelization and false to fragment triangle
bool classifyTriangleSize(vec3 v0, vec3 v1, vec3 v2, float cutoff)
{
	float triangleArea2D;
	triangleArea2D = abs(v0.x*(v1.y-v2.y) + v1.x*(v2.y-v0.y) + v2.x*(v0.y-v1.y)) * 0.5;
	bool result = (triangleArea2D <= cutoff);
	return result;

}

bool classifyTriangleDepth(vec3 planeOrigin, vec3 normal, vec3 AABBmin, vec3 AABBmax, out int zMin)
{

	//float d = dot(normal, planeOrigin);
	//float nzInv = 1.0/normal.z;
	//float  zMinInt = d - dot(normal.xy, AABBmin.xy) * nzInv;
	//float  zMaxInt = d - dot(normal.xy, AABBmax.xy) * nzInv;
	
	float zMinFloor = floor(AABBmin.z);
	float zMaxCeil  =  ceil(AABBmax.z);
	
	zMin = int(zMinFloor) - int(AABBmin.z == AABBmin.z);
	int zMax = int(zMaxCeil ) + int(AABBmax.z  == AABBmax.z);
//	zMin = float(AABBmin.z);
	bool zExpand = (zMax - zMin) == 1.0;
	return zExpand;

}

void swizzle(inout vec3 v0, inout vec3 v1, inout vec3 v2, inout vec3 normal)
{

float maxVal = max(abs(normal.x) , max(abs(normal.y) , abs(normal.z)));
		//Case 1 x is dominant axis
		//z --> x
		//y --> y
		//x --> z

		//find 3 triangle edge plane

		if(maxVal == abs(normal.x)){
		dominantAxisCase = 0;
		v0 = v0.yzx;                        
		v1 = v1.yzx;                        
		v2 = v2.yzx;
		normal = normal.yzx;
		}
		
		//Case 2 y is dominant axis
		//x --> x
		//-z --> y
		//-y --> z
		else if(maxVal == abs(normal.y))
		{
		dominantAxisCase = 1;
		v0 =  v0.zxy; 
		v1 =  v1.zxy;                            
		v2 =  v2.zxy;
		normal = normal.zxy;
		}
		
		else if(maxVal == abs(normal.z))
		{
		//Case 3 z is dominant axis
		dominantAxisCase = 2;
		}

}

void main()
{
		
	vec3 v0 = gl_in[0].gl_Position.xyz;
	vec3 v1 = gl_in[1].gl_Position.xyz;
	vec3 v2 = gl_in[2].gl_Position.xyz;
	vec3 normal = normalize(cross(v1 - v0, v2 - v1));
	
	float halfVoxelRes = voxel_Res/2.0;
	
	swizzle(v0, v1, v2, normal);

	vec3 v0_new	= v0;
	vec3 v1_new	= v1;
	vec3 v2_new	= v2;
	
	//Edges for swizzled vertices;
	vec3 e0 = v1 - v0;	//figure 17/18 line 2
	vec3 e1 = v2 - v1;	//figure 17/18 line 2
	vec3 e2 = v0 - v2;	//figure 17/18 line 2
	
	vec2 n_e0_xy,n_e1_xy, n_e2_xy;
	vec2 n_e0_yz, n_e1_yz, n_e2_yz ;
	vec2 n_e0_zx, n_e1_zx, n_e2_zx ;
	
	
	if(normal.z >= 0){
	//INward Facing edge normals XY
	n_e0_xy = normalize(-vec2(-e0.y, e0.x)) ; 
	n_e1_xy = normalize(-vec2(-e1.y, e1.x)) ; 
	n_e2_xy = normalize(-vec2(-e2.y, e2.x)) ; 
	
	}
	else{
	n_e0_xy= normalize(-vec2(e0.y, -e0.x)) ; 
	n_e1_xy= normalize(-vec2(e1.y, -e1.x)) ; 
	n_e2_xy= normalize(-vec2(e2.y, -e2.x)) ; 

	}
	
	AABBmin = min(min(v0, v1), v2);
	AABBmax = max(max(v0, v1), v2);
	
	int zMin;
	bool useTriParallel = classifyTriangleSize(v0, v1, v2, cutoff);
	bool depthExpand = classifyTriangleDepth(v0, normal, AABBmin, AABBmax, zMin);

	if(useTriParallel && depthExpand){
		v0 += halfVoxelRes;
		v1 += halfVoxelRes;
		v2 += halfVoxelRes;

		AABBmin += 	 halfVoxelRes;
		AABBmax += 	 halfVoxelRes;

		n_e0_xy	*= -1;
		n_e1_xy	*= -1;
		n_e2_xy	*= -1;

		
		float d_e0_xy = -dot(n_e0_xy, v0.xy) + max(0.0f, n_e0_xy.x) + max(0.0f, n_e0_xy.y);	//figure 17 line 7
		float d_e1_xy = -dot(n_e1_xy, v1.xy) + max(0.0f, n_e1_xy.x) + max(0.0f, n_e1_xy.y);	//figure 17 line 7
		float d_e2_xy = -dot(n_e2_xy, v2.xy) + max(0.0f, n_e2_xy.x) + max(0.0f, n_e2_xy.y);	//figure 17 line 7
			
		

		ivec3 minVoxIndex = ivec3(clamp(floor(AABBmin), ivec3(0), ivec3(voxel_Res)));
		ivec3 maxVoxIndex = ivec3(clamp( ceil(AABBmax), ivec3(0), ivec3(voxel_Res)));
	
		ivec3 p;					//voxel coordinate
		for(p.x = minVoxIndex.x; p.x < maxVoxIndex.x; p.x++)	//figure 17 line 13, figure 18 line 12
		{
			for(p.y = minVoxIndex.y; p.y < maxVoxIndex.y; p.y++)	//figure 17 line 14, figure 18 line 13
			{
				float dd_e0_xy = d_e0_xy + dot(n_e0_xy, p.xy);
				float dd_e1_xy = d_e1_xy + dot(n_e1_xy, p.xy);
				float dd_e2_xy = d_e2_xy + dot(n_e2_xy, p.xy);
			
				bool xy_overlap = (dd_e0_xy >= 0) && (dd_e1_xy >= 0) && (dd_e2_xy >= 0);
	
				if(xy_overlap)	//figure 17 line 15, figure 18 line 14
				{
					
					p.z = zMin;
					ivec3 origCoord = (dominantAxisCase == 0) ? p.zxy : (dominantAxisCase == 1) ? p.yzx : p.xyz;
					//#ifdef USEVOXELFRAGMENTLIST
									uint fragmentListIdx = atomicCounterIncrement(FragmentCount);	
									imageStore(voxelFragmentList1, int(fragmentListIdx), uvec4(origCoord,0));
								//#else
								//	imageStore(voxelImage1, origCoord, vec4(1.0 ,0.0, 0.0, 0.0));
									
								//#endif
					}
				}
			}	
	
	}else{
	
	AABBmin -= vec3(0.5);
	AABBmax += vec3(0.5);
	
	v0_new.xy = v0.xy + L * ( e2.xy / ( dot(e2.xy, n_e0_xy) ) + e0.xy / ( dot(e0.xy, n_e2_xy) ) );
	v1_new.xy = v1.xy + L * ( e0.xy / ( dot(e0.xy, n_e1_xy) ) + e1.xy / ( dot(e1.xy, n_e0_xy) ) );
	v2_new.xy = v2.xy + L * ( e1.xy / ( dot(e1.xy, n_e2_xy) ) + e2.xy / ( dot(e2.xy, n_e1_xy) ) );
	
	v0_new.z =   v0.z;
	v1_new.z =   v1.z;
	v2_new.z =   v2.z;

	gl_Position = vec4(v0_new, halfVoxelRes);
	//baryCoords = b0;
	triangle_pos = v0_new.xyz;
	color_out = vs_color[0];

	EmitVertex();
	gl_Position = vec4(v1_new, halfVoxelRes);
	//baryCoords = b1;
	triangle_pos = v1_new.xyz;
	color_out = vs_color[1];
	EmitVertex();
	gl_Position = vec4(v2_new, halfVoxelRes);
	//baryCoords = b2;
	triangle_pos = v2_new.xyz;
	color_out = vs_color[2];
	EmitVertex();
	}
}
