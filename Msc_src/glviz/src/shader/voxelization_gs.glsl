#version 420
#define L     0.7071067811865475244008443621048490392848359376884740	//sqrt(2)/2

flat out uint dominantAxisCase;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int voxel_Res;
smooth out vec3 triangle_pos;
smooth out vec3 baryCoords;


in vec3 vs_color[];
smooth out vec3 color_out;

	flat out vec2 AABBmin;
	flat out vec2 AABBmax;
//flat out vec3 e0, e1, e2;
//smooth float z0, z1, z2;
//flat out float signedDistance_e0, signedDistance_e1, signedDistance_e2;


void computeBaryCoords(inout vec3 b0, inout vec3 b1, inout vec3 b2, vec2 v0, vec2 v1, vec2 v2)
{
	vec3 vb;
	float invDetA = 1.0 / ((v0.x*v1.y) - (v0.x*v2.y) - (v1.x*v0.y) + (v1.x*v2.y) + (v2.x*v0.y) - (v2.x*v1.y));

	vb.x = ( (b0.x*v1.y) - (b0.x*v2.y) - (v1.x*b0.y) + (v1.x*v2.y) + (v2.x*b0.y) - (v2.x*v1.y) ) * invDetA;
	vb.y = ( (v0.x*b0.y) - (v0.x*v2.y) - (b0.x*v0.y) + (b0.x*v2.y) + (v2.x*v0.y) - (v2.x*b0.y) ) * invDetA;
	vb.z = 1 - vb.x - vb.y;
	b0 = vb;

	vb.x = ( (b1.x*v1.y) - (b1.x*v2.y) - (v1.x*b1.y) + (v1.x*v2.y) + (v2.x*b1.y) - (v2.x*v1.y) ) * invDetA;
	vb.y = ( (v0.x*b1.y) - (v0.x*v2.y) - (b1.x*v0.y) + (b1.x*v2.y) + (v2.x*v0.y) - (v2.x*b1.y) ) * invDetA;
	vb.z = 1 - vb.x - vb.y;
	b1 = vb;

	vb.x = ( (b2.x*v1.y) - (b2.x*v2.y) - (v1.x*b2.y) + (v1.x*v2.y) + (v2.x*b2.y) - (v2.x*v1.y) ) * invDetA;
	vb.y = ( (v0.x*b2.y) - (v0.x*v2.y) - (b2.x*v0.y) + (b2.x*v2.y) + (v2.x*v0.y) - (v2.x*b2.y) ) * invDetA;
	vb.z = 1 - vb.x - vb.y;
	b2 = vb;
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
				//Transform world coordinates in voxel coordinates
		//vec3 v0 = ((gl_in[0].gl_Position.xyz + 1.0)/2.0) * voxel_Res;
		//vec3 v1 = ((gl_in[1].gl_Position.xyz + 1.0)/2.0) * voxel_Res;
		//vec3 v2 = ((gl_in[2].gl_Position.xyz + 1.0)/2.0) * voxel_Res;
		float halfVoxelRes = voxel_Res/2.0;
		vec3 v0 = ((gl_in[0].gl_Position.xyz));
		vec3 v1 = ((gl_in[1].gl_Position.xyz));
		vec3 v2 = ((gl_in[2].gl_Position.xyz));

		//vec3 v0 = gl_in[0].gl_Position.xyz;
		//vec3 v1 = gl_in[1].gl_Position.xyz;
		//vec3 v2 = gl_in[2].gl_Position.xyz;
		vec3 normal = normalize(cross(v1 - v0, v2 - v1));
		
		swizzle(v0, v1, v2, normal);

		vec3 v0_new	= v0;
		vec3 v1_new	= v1;
		vec3 v2_new	= v2;


	//Edges for swizzled vertices;
	vec3 e0 = v1 - v0;	//figure 17/18 line 2
	vec3 e1 = v2 - v1;	//figure 17/18 line 2
	vec3 e2 = v0 - v2;	//figure 17/18 line 2

	vec2 n_e0_xy_norm ;
	vec2 n_e1_xy_norm ;
	vec2 n_e2_xy_norm ;
	
	
	if(normal.z >= 0){
	//INward Facing edge normals XY
	n_e0_xy_norm = normalize(-vec2(-e0.y, e0.x)) ; 
	n_e1_xy_norm = normalize(-vec2(-e1.y, e1.x)) ; 
	n_e2_xy_norm = normalize(-vec2(-e2.y, e2.x)) ; 
	}
	else{
	n_e0_xy_norm = normalize(-vec2(e0.y, -e0.x)) ; 
	n_e1_xy_norm = normalize(-vec2(e1.y, -e1.x)) ; 
	n_e2_xy_norm = normalize(-vec2(e2.y, -e2.x)) ; 
	}
	
	float halfPixel =  1.0 / (1.0*float(voxel_Res));

	float diagonal = L;
	AABBmin = min(min(v0.xy, v1.xy), v2.xy) - vec2(0.5);
	AABBmax = max(max(v0.xy, v1.xy), v2.xy) + vec2(0.5);


	//signedDistance_e0 = dot(n_e0_xy_norm, e1.xy);
	//signedDistance_e1 = dot(n_e1_xy_norm, e2.xy);
	//signedDistance_e2 = dot(n_e2_xy_norm, e0.xy);
	
	v0_new.xy = v0.xy + diagonal * ( e2.xy / ( dot(e2.xy, n_e0_xy_norm) ) + e0.xy / ( dot(e0.xy, n_e2_xy_norm) ) );
	v1_new.xy = v1.xy + diagonal * ( e0.xy / ( dot(e0.xy, n_e1_xy_norm) ) + e1.xy / ( dot(e1.xy, n_e0_xy_norm) ) );
	v2_new.xy = v2.xy + diagonal * ( e1.xy / ( dot(e1.xy, n_e2_xy_norm) ) + e2.xy / ( dot(e2.xy, n_e1_xy_norm) ) );
	
	v0_new.z =   v0.z;
	v1_new.z =   v1.z;
	v2_new.z =   v2.z;
	//vec3 b0,b1,b2; 
	//computeBaryCoords(b0, b1, b2, v0.xy, v1.xy, v2.xy);

	
	//v0.z = (-dot(nProj.xy, v0.xy) + dTri) * nzInv;
		//v1.z = (-dot(nProj.xy, v1.xy) + dTri) * nzInv;
	//v2.z = (-dot(nProj.xy, v2.xy) + dTri) * nzInv;


	//}
	//gl_Position = vec4(v0, 1.0);
	//baryCoords = b0;
	//triangle_pos = v0_new.xyz;
	//color_out = vs_color[0];
	//EmitVertex();
	//gl_Position = vec4(v1, 1.0);
	//baryCoords = b1;
	//triangle_pos = v1_new.xyz;
	//color_out = vs_color[1];
	//EmitVertex();
	//gl_Position = vec4(v2, 1.0);
	//baryCoords = b2;
	//triangle_pos = v2_new.xyz;
	//color_out = vs_color[2];
	//EmitVertex();


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
