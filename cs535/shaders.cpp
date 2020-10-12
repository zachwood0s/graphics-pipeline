#include "TMesh.h"
#include "scene.h"
#include "WorldView.h"
#include "Material.h"
#include "shaders.h"

Vec3d Shaders::phongShading(ShaderInputs input)
{
	// Normal at current pixel
	Vec3d normalVector = input.interp.normals.Normalized();

	// Light vector
	Vec3d viewDir = (input.view.GetPPC()->C - input.currP).Normalized();
	Vec3d currColor = input.currColor * input.view.kAmbient;
	for (auto light : input.scene.lights)
	{
		if (!light->IsInShadow(input.currP))
			currColor = currColor + light->LightPixel(input.currP, input.currColor, normalVector, viewDir, input.mat);
	}
	return currColor;
}

Vec3d Shaders::depthShading(ShaderInputs input)
{
	// Show a depth map
	float val = 1 - 1 / (1 + input.interp.zVal / 5);
	return Vec3d(val, val, val);
}

Vec3d Shaders::projectiveTextures(ShaderInputs input)
{
	Vec3d currColor = input.currColor;
	for (auto projector : input.scene.projectors)
	{
		if (!projector->IsInShadow(input.currP))
		{
			Vec3d newColor;
			if (projector->GetColorAt3dPoint(input.scene, input.currP, newColor))
			{
				currColor = newColor;
			}
		}
	}
	return currColor;
}