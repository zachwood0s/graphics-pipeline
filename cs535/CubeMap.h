#pragma once

#include "WorldView.h"
#include <memory>

class CubeMap
{
private:
	int lastSuccessful;

	void SetupViews(int w, int h);

public:

	enum Face
	{
		LEFT,
		RIGHT,
		FORWARD,
		BACKWARD,
		UP,
		DOWN,
		NUM_FACES
	};
	
	std::unique_ptr<WorldView> views[6];

	CubeMap();
	static std::unique_ptr<CubeMap> FromSingleTiff(const char * fname, bool vertical = false);
	unsigned int Lookup(Vec3d point3d);
	
};

