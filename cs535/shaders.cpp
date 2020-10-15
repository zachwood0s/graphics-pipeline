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
	Vec3d viewDir = (input.view.GetPPC()->C - input.currP3D).Normalized();
	Vec3d currColor = input.currColor * input.view.kAmbient;
	for (auto& light : input.scene.lights)
	{
		if (!light->IsInShadow(input.currP3D))
			currColor = currColor + light->LightPixel(input.currP3D, input.currColor, normalVector, viewDir, input.mat);
	}
	return currColor;
}

Vec3d Shaders::depthShading(ShaderInputs input)
{
	// Show a depth map
	float val = 1 / input.interp.zVal;
	return Vec3d(val, val, val);
}

Vec3d Shaders::projectiveTextures(ShaderInputs input)
{
	Vec3d currColor = input.currColor;
	for (auto& projector : input.scene.projectors)
	{
		if (!projector->IsInShadow(input.currP3D))
		{
			Vec3d newColor;
			if (projector->GetColorAt3dPoint(input.scene, input.currP3D, newColor))
			{
				currColor = newColor;
			}
		}
	}
	return currColor;
}

Vec3d Shaders::invisibilityShader(ShaderInputs input)
{
	auto& viewBlank = input.scene.views[2];
	auto& viewProjector = input.scene.projectors[0]->shadowMap;

	// Find color that we should draw from the blank scene
	Vec3d colorOfPixel = Vec3d::FromColor(viewBlank->GetFB()->Get((int)input.currPScreen[0], (int)input.currPScreen[1]));

	if (!input.scene.projectors[0]->IsInShadow(input.currP3D))
	{
		// Now find where that should be drawn on our projector as
		Vec3d projectedCam;
		if (!viewProjector->GetPPC()->Project(input.currP3D, projectedCam))
			return input.currColor;

		// Set the found pixel on the camera to what should be shown
		viewProjector->GetFB()->Set((int)projectedCam[0], (int)projectedCam[1], colorOfPixel.GetColor());
	}

	return input.currColor;
}