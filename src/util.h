#pragma once

#include <string>

namespace Util {

	// Convert BGR image (24bpp) to RGB and save as .png
	void save_BGR_image(const std::string& filename, const char* imagedata, int32_t width, int32_t height);

	// Get filename timestring.
	std::string gettimestr();
}