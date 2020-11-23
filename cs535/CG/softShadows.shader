
struct VertexDataInput {
	float4 vertex: POSITION;
	float3 normal: NORMAL;
	float3 color: COLOR;
	float2 coord: TEXCOORD0;
};

struct VertexDataOutput {
	float4 projv  : POSITION;
	float3 normal: TEXCOORD0;
	float3 vxyz: TEXCOORD1;
	float3 color : COLOR;
	float2 coord: TEXCOORD2;
	float4 projvpos  : TEXCOORD3;
};

struct PixelDataOutput {
	float3 color : COLOR;
};

VertexDataOutput VertexMain(VertexDataInput vdi, uniform float4x4 modelViewProj) {

  VertexDataOutput ret;

  ret.projv = mul(modelViewProj, vdi.vertex);
  ret.projvpos = ret.projv;
  ret.normal = vdi.normal;
  ret.vxyz = vdi.vertex.xyz;
  ret.color = vdi.color;
  ret.coord = vdi.coord;
  return ret;

}

// guidelines for the fragment shader for A6.2, billboard rendering
// create billboard rectangle and texture as a preprocess
// turn on shader when rendering reflective teapot
// use conventional vertex shader, just projection
// shader uniform parameters: billboard vertices, billboard texture (sampler2D), eye 
//		(i.e. current position of the user, center of projection of output image)

// 1. compute eye ray, as difference between eye and surface point
// 2. reflect eye ray about normal to obtain reflected ray
// 3. intersect refleted ray with billboard rectangle
// 4. if intersection, lookup billboard texture at intersection point
		// 5. if color looked up is background, then no intersection, set  color to background
					// environment mapping for extra credit
		// 6. if color is not background, then set color to looked up color
// 6. if no intersection, set color to background (environment mapping for extra credit)

// guidelines for the soft shadow A6.3 question
// shader uniform parameters: light rectangle, the floor quad, the vertices of box 1,
//		the vertices of box 2
// shader algorithm
//		1. hard code sampling factor of light, sfl = 2 for 2x2, = 16 for 16x16
//		1.5 lv = sfl x sfl; // assume that none of the light samples are visible
//		2. for every light sample row i
//		3. for every light sample column j
//		4.	intersect ray(xyz, Lij) with all boxes
//		5.		if one intersection is found lv--, move on to next light sample
//		6. return color = input.color * lv / sfl / sfl;

// opengl textures
//  Initialization (once per session)
//	1. create texture handle
//	2. create actual texture by passing the pixels to the GPU, which are to be 
//			used as texels, pass the image (fb->pix) to be used as texture
//  For every frame
//  1. turn on texturing for TMesh that has to be rendered with textures
//	2. bind texture to be used with this mesh
//  3. issue texture coordinates, the same way xyz, rgb, nxnynz are issued per vertex

bool signedVolume(float3 a, float3 b, float3 c, float3 d)
{
	return dot((d - a), cross(b - a, c - a)) > 0 ? true : false;
}

bool RayTriangleIntersection(float3 rayC, float3 rayDir, float3 v0, float3 v1, float3 v2)
{
	float3x3 M = float3x3(
		v0[0], v1[0], v2[0],
		v0[1], v1[1], v2[1],
		v0[2], v1[2], v2[2]);

	M = inverse(M);

	float3 q = mul(M, rayC);
	float3 r = mul(M, rayDir);

	float t = (1 - (q[0] + q[1] + q[2])) / (r[0] + r[1] + r[2]);
	if (t < 0.001f || t > 1e5f)
	{
		return false;
	}
	float3 abc = q + r * t;
	if (abc[0] < 0.0f || abc[1] < 0.0f || abc[2] < 0.0f)
	{
		return false;
	}
	return true;
}


bool RayIntersection(float3 rayC, float3 rayDir, float3 vert1, float3 vert2, float3 vert3, float3 vert4)
{
	float3 planeBasisU = vert2 - vert1;
	float3 planeBasisV = vert3 - vert1;
	float3 n = normalize(cross(planeBasisU, planeBasisV)); // plane normal

	float3 rayPoint = vert1 - rayC;
	float nDotR = dot(n, rayDir);

	if (abs(nDotR) < 1e-6f)
	{
		return false;
	}

	float t = dot(n, rayPoint) / nDotR;
	float3 planePoint = rayDir * t + rayC;

	float u = dot(planePoint - vert1, planeBasisU);
	float v = dot(planePoint - vert1, planeBasisV);

	return u >= 0 && u <= length(planeBasisU) && v >= 0.0f && v <= length(planeBasisV);

	// Check if in bounds
}

PixelDataOutput FragmentMain(VertexDataOutput pdi, uniform sampler2D decal, 
	uniform float3 light[4], uniform float3 box1[36], uniform float3 box2[36], uniform float3 box3[36]) {

	int numTris = 12;
	float ka = .1f;

	PixelDataOutput ret;

	float sfl = 8;
	float lv = sfl * sfl;

	float3 lightX = light[1] - light[0];
	float3 lightY = light[2] - light[0];

	float3 lightU = normalize(lightX);
	float3 lightV = normalize(lightY);

	float incX = length(lightX) / sfl;
	float incY = length(lightY) / sfl;

	for (int i = 0; i < sfl; i++)
	{
		for (int j = 0; j < sfl; j++)
		{
			float3 lightSample = light[0] + lightU * i * incX + lightV * j * incY;
			for (int x = 0; x < numTris; x++)
			{
				if (RayTriangleIntersection(pdi.vxyz, lightSample - pdi.vxyz, box1[x * 3], box1[x * 3 + 1], box1[x * 3 + 2]) ||
					RayTriangleIntersection(pdi.vxyz, lightSample - pdi.vxyz, box2[x * 3], box2[x * 3 + 1], box2[x * 3 + 2]) ||
					RayTriangleIntersection(pdi.vxyz, lightSample - pdi.vxyz, box3[x * 3], box3[x * 3 + 1], box3[x * 3 + 2]))
				{
					lv--;
					break;
				}
			}
		}
	}


	// Phong shading
	float3 lightVector = normalize(light[0] - pdi.vxyz);
	float kd = dot(lightVector, pdi.normal);
	
	ret.color = pdi.color * kd;

	ret.color = ret.color * lv / sfl / sfl;
	ret.color += pdi.color * ka;

	return ret;

}