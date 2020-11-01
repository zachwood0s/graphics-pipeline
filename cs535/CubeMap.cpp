#include "CubeMap.h"


CubeMap::CubeMap() : lastSuccessful(0)
{
}

unsigned int CubeMap::Lookup(Vec3d point3d)
{
	// Find the projection that succeeds
	for (unsigned int i = 0; i < Face::NUM_FACES; i++)
	{
		unsigned int face = i; // (lastSuccessful + i) % Face::NUM_FACES;
		//std::cout << face << std::endl;
		Vec3d p;
		if (!views[face]->GetPPC()->Project(point3d, p))
			continue;

		int newU = (int)p[0];
		int newV = (int)p[1];

		if (newU < 0 || newU >= views[face]->GetFB()->w || newV < 0 || newV >= views[face]->GetFB()->h)
			continue;

		lastSuccessful = face;
		float s = p[0] / views[face]->GetFB()->w;
		float t = p[1] / views[face]->GetFB()->h;
		auto[color, alpha] = views[face]->GetFB()->GetTexVal(Vec3d(s, t, 0), 0);
		return color.GetColor();
	}
	std::cout << "failed" << std::endl;
	return Vec3d(1, 0, 0).GetColor();
}

std::unique_ptr<CubeMap> CubeMap::FromSingleTiff(const char * fname, bool vertical)
{
	// Load in overall tiff. 
	FrameBuffer fb = FrameBuffer(0, 0);
	if (!fb.LoadTiff(fname))
		return nullptr;

	// Figure out w/h of cube map
	int squareWidth = fb.w / 4;
	int squareHeight = fb.h / 3;

	// Create new cubemap
	auto map = std::make_unique<CubeMap>();

	// Copy in all of the frames
	std::tuple<int, int> imageCoords[6];

	imageCoords[Face::LEFT] = std::make_tuple(0, 1);
	imageCoords[Face::RIGHT] = std::make_tuple(2, 1);
	imageCoords[Face::FORWARD] = std::make_tuple(1, 1);
	imageCoords[Face::BACKWARD] = std::make_tuple(3, 1);
	imageCoords[Face::UP] = std::make_tuple(1, 0);
	imageCoords[Face::DOWN] = std::make_tuple(1, 2);

	if (vertical)
	{
		squareWidth = fb.w / 3;
		squareHeight = fb.h / 4;

		imageCoords[Face::BACKWARD] = std::make_tuple(1, 3);
	}

	map->SetupViews(squareWidth, squareHeight);

	if (vertical)
	{
		map->views[Face::BACKWARD]->GetPPC()->SetPose(Vec3d::ZEROS, Vec3d::ZAXIS, Vec3d::YAXIS *-1.0f);
	}

	for (int face = 0; face < Face::NUM_FACES; face++)
	{
		auto faceFb = map->views[face]->GetFB();
		int startX = std::get<0>(imageCoords[face]) * squareWidth;
		int startY = std::get<1>(imageCoords[face]) * squareHeight;

		for (int u = 0; u < squareWidth; u++)
		{
			for (int v = 0; v < squareHeight; v++)
			{
				faceFb->Set(u, v, fb.Get(u + startX, v + startY));
				faceFb->SetZ(u, v, 1.0f);
			}
		}
	}


	return std::move(map);
}

void CubeMap::SetupViews(int w, int h)
{
	for (int i = 0; i < Face::NUM_FACES; i++)
	{
		views[i] = std::make_unique<WorldView>(w, h, 90, -1);
	}

	views[Face::LEFT]->GetPPC()->SetPose(Vec3d::ZEROS, Vec3d::XAXIS * -1.0f, Vec3d::YAXIS);
	views[Face::RIGHT]->GetPPC()->SetPose(Vec3d::ZEROS, Vec3d::XAXIS, Vec3d::YAXIS);
	views[Face::FORWARD]->GetPPC()->SetPose(Vec3d::ZEROS, Vec3d::ZAXIS * -1.0f, Vec3d::YAXIS);
	views[Face::BACKWARD]->GetPPC()->SetPose(Vec3d::ZEROS, Vec3d::ZAXIS, Vec3d::YAXIS);
	views[Face::UP]->GetPPC()->SetPose(Vec3d::ZEROS, Vec3d::YAXIS, Vec3d::ZAXIS);
	views[Face::DOWN]->GetPPC()->SetPose(Vec3d::ZEROS, Vec3d::YAXIS * -1.0f, Vec3d::ZAXIS * -1.0f);
}
