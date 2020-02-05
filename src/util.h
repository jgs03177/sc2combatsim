#pragma once

#include <sc2api/sc2_api.h>

#include <string>

namespace Util {

	// Convert BGR image (24bpp) to RGB and save as .png
	void save_BGR_image(const std::string& filename, const char* imagedata, int32_t width, int32_t height);

	// Get filename timestring.
	std::string gettimestr();

	std::tuple<std::vector<sc2::UnitTypeID>, std::vector<int>, std::vector<sc2::UnitTypeID>, std::vector<int> > read(const std::string& path);
}