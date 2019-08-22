#include "lodepng.h"
#include <iostream>
#include "sim_bot.h"
#include "sc2renderer/sc2_renderer.h"
#include <vector>

void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height) {
	//Encode the image
	unsigned error = lodepng::encode(filename, image, width, height);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

// must be in render mode
void SimBot::screencapture(const std::string& filename, int compression) {
	const SC2APIProtocol::Observation* observation = Observation()->GetRawObservation();
	const SC2APIProtocol::ObservationRender& render = observation->render_data();

	const SC2APIProtocol::ImageData& minimapraw = render.minimap();
	const SC2APIProtocol::ImageData& mapraw = render.map();

	//BRG, 24bit(3 channels)
	std::string map = mapraw.data();
	std::string minimap = minimapraw.data();

	int32_t width = mapraw.size().x();
	int32_t height = mapraw.size().y();
	int32_t length = map.length();

	//std::cout << width << height << length << std::endl;
	std::vector<unsigned char> map_RGB(length, 255);
	for (int i = 0; i < length; i += 3) {
		//BRG to RGB
		map_RGB[i    ] = map[i + 2];
		map_RGB[i + 1] = map[i + 1];
		map_RGB[i + 2] = map[i    ];
	}
	std::vector<unsigned char> a;
	unsigned error;
	//error = lodepng::encode(filename, (unsigned char*)mapraw.data().data(), width, height, LCT_RGB, 8);
	//if (error) std::cout << "decoder1 error " << error << ": " << lodepng_error_text(error) << std::endl;
	error = lodepng::encode(filename, (unsigned char *)map_RGB.data(), width, height, LCT_RGB, 8);
	if (error) std::cout << "decoder2 error " << error << ": " << lodepng_error_text(error) << std::endl;
}