#pragma once
#include "vec3d.h"
#include "Material.h"
#include "WorldView.h"

/// <summary>
/// A simple point light class with shadow mapping capabilities
/// </summary>
class Light
{
private:
	Vec3d center;

public:
	WorldView * shadowMap;
	static constexpr float SHADOW_EPS = 0.05f;
	Light(Vec3d center, int shadowResX, int shadowResY, float fov);
	~Light();

	float constant = 0.8f;
	float linear = 0.0045f;
	float quadradic = 0.000075f; // Really powerful point lighs

	float ambient = .1f;
	float diffuse = .8f;
	float specular = 1.0f;

	/// <summary>
	/// Takes the current color of a pixel and applies this lights value to it. Does not take into account shadows as 
	/// that should be done before this function is called
	/// </summary>
	/// <param name="point">The 3D point to light</param>
	/// <param name="curColor">The current color of the given point</param>
	/// <param name="normalVector">The normal of the mesh at the given point</param>
	/// <param name="viewDir">The view direction of the camera</param>
	/// <param name="mat">The material at the given point. Used for specular shading</param>
	/// <returns></returns>
	Vec3d LightPixel(Vec3d point, Vec3d curColor, Vec3d normalVector, Vec3d viewDir, Material mat) const;

	/// <summary>
	/// Determines whether or not the given 3D point is in shadow based on this light
	/// </summary>
	/// <param name="point">The 3D point to check</param>
	/// <returns>True if the 3D point is in shadow, false otherwise</returns>
	bool IsInShadow(Vec3d point);

	/// <summary>
	/// Updates the shadow map for the given light. This needs to be done any time the light moves
	/// or a mesh moves
	/// </summary>
	/// <param name="scene">The scene that the shadow map should be based on</param>
	void UpdateShadowMap(Scene &scene);

	/// <summary>
	/// Sets the center of the light to the provided point
	/// </summary>
	/// <param name="center">The new center of the light</param>
	void SetCenter(Vec3d center);

	/// <summary>
	/// Retrieves the center of the light
	/// </summary>
	/// <returns>The center of the light</returns>
	Vec3d GetCenter() const;

	/// <summary>
	/// Sets the attenuation values for the light
	/// </summary>
	/// <param name="constant">The constant attenuation value</param>
	/// <param name="linear">The linear attenuation value</param>
	/// <param name="quadradic">The quadradic attenuation value</param>
	void SetAttenuation(float constant, float linear, float quadradic);
};

