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

	int32_t width = mapraw.size().x();
	int32_t height = mapraw.size().y();
	//map = mapraw.data();
	//minimap = minimapraw.data();

	//BRG

	bool result = lodepng::encode(filename, (unsigned char *)mapraw.data().data(), width, height, LCT_RGB, 8);
}