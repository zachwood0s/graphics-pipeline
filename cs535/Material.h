#pragma once

#include "vec3d.h"
#include "framebuffer.h"

class Scene;

/// <summary>
/// Stores material properties for a mesh. Has functions for getting the color at a specific point which
/// does texture lookup
/// </summary>
class Material 
{
public:
	static Material DEFAULT(Vec3d color);
	Material(Vec3d color, float phong, float kSpec);
	Material(Vec3d color, float phong, float kSpec, TEX_HANDLE texture);

	/// <summary>
	/// Grabs the color of a material at the given texture coordinates which could mean doing a texture map lookup if
	/// the texture exists or just returning the material color if the texture is TEX_INVALID.
	/// </summary>
	/// <param name="scene">The scene containing the textures</param>
	/// <param name="point">The texture coords to lookup (if applicable)</param>
	/// <param name="texDeltas">The change in texture values between interpolation points. Used for mipmapping</param>
	/// <returns>A tuple of color value and alpha value at the given texture coordinate</returns>
	std::tuple<Vec3d, float> GetColor(Scene &scene, Vec3d point, Vec3d texDeltas);

	Vec3d color;
	float phongExponent;
	float kSpecular;

	TEX_HANDLE texture = TEX_INVALID;
};

