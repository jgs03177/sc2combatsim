#include "util.h"

#include "lodepng.h"

#include <iostream>
#include <vector>
#include <iomanip>


// Convert BGR image (24bpp) to RGB and save as .png
void Util::save_BGR_image(const std::string& filename, const char* imagedata, int32_t width, int32_t height) {
	int32_t length = width * height * 3; // 3 bytes per pixel

	std::vector<unsigned char> map_RGB(length);
	for (int i = 0; i < length; i += 3) {
		//BRG to RGB
		map_RGB[i    ] = imagedata[i + 2];
		map_RGB[i + 1] = imagedata[i + 1];
		map_RGB[i + 2] = imagedata[i    ];
	}

	unsigned error = lodepng::encode(filename, (unsigned char*)map_RGB.data(), width, height, LCT_RGB, 8);
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

std::string Util::gettimestr() {
	time_t rawtime;
	struct tm timeinfo;
	time(&rawtime);
#if defined(_WIN32)
	localtime_s(&timeinfo, &rawtime);
#else
	timeinfo = *localtime(&rawtime);
#endif
	char buffer[80];
	strftime(buffer, 80, "%Y-%m-%d %H-%M-%S", &timeinfo);
	return std::string(buffer);
	/*
	std::ostringstream o;
	o << timeinfo.tm_year + 1900 << '-'
		<< std::setw(2) << std::setfill('0') << timeinfo.tm_mon + 1 << '-'
		<< std::setw(2) << std::setfill('0') << timeinfo.tm_mday << ' '
		<< std::setw(2) << std::setfill('0') << timeinfo.tm_hour << '-'
		<< std::setw(2) << std::setfill('0') << timeinfo.tm_min << '-'
		<< std::setw(2) << std::setfill('0') << timeinfo.tm_sec;
	return o.str();
    */
}
