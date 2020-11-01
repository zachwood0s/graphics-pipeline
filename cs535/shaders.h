#pragma once
#include <functional>

#include "vec3d.h"

class Scene;
class WorldView;
class Material;
struct InterpVal;

struct ShaderInputs 
{
	Scene &scene;
	WorldView &view;
	Material &mat;
	InterpVal &interp;
	Vec3d currColor;
	Vec3d currP3D;
	Vec3d currPScreen;
};

using Shader = std::function<Vec3d(ShaderInputs inputs)>;
using BackgroundShader = std::function<void(Scene &scene, WorldView &view)>;

namespace Shaders
{
	Vec3d phongShading(ShaderInputs inputs);
	Vec3d depthShading(ShaderInputs inputs);
	Vec3d projectiveTextures(ShaderInputs inputs);
	Vec3d invisibilityShader(ShaderInputs inputs);
	Vec3d reflectionShader(ShaderInputs inputs);

	void solidBackground(Scene &scene, WorldView &view);
	void environmentMap(Scene &scene, WorldView &view);
}
