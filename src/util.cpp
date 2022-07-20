#include "util.h"

#include "lodepng.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

namespace Util {

	// Convert BGR image (24bpp) to RGB and save as .png
	void save_BGR_image(const std::string& filename, const char* imagedata, int32_t width, int32_t height) {
		int32_t length = width * height * 3; // 3 bytes per pixel

		std::vector<unsigned char> map_RGB(length);
		for (int i = 0; i < length; i += 3) {
			//BRG to RGB
			map_RGB[i] = imagedata[i + 2];
			map_RGB[i + 1] = imagedata[i + 1];
			map_RGB[i + 2] = imagedata[i];
		}

		unsigned error = lodepng::encode(filename, (unsigned char*)map_RGB.data(), width, height, LCT_RGB, 8);
		if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	}

	std::string gettimestr() {
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

	std::tuple<std::vector<sc2::UnitTypeID>, std::vector<int>, std::vector<sc2::UnitTypeID>, std::vector<int> >
		ReadPresetSquad(const std::string& path) {
		std::ifstream fin;

		std::vector<sc2::UnitTypeID> squad_unittypeid1;
		std::vector<int> squad_quantity1;
		std::vector<sc2::UnitTypeID> squad_unittypeid2;
		std::vector<int> squad_quantity2;

		std::string line;
		std::stringstream linestream;
		std::string buffer;

		std::cout << "\nReading sqauds from : " << path << std::endl;
		fin.open(path);
		if (!fin.good()) {
			std::cout << "Failed to open file." << std::endl;
			fin.close();
			exit(1);
		}

		std::getline(fin, line);
		std::cout << line << std::endl;
		linestream << line;
		while (std::getline(linestream, buffer, ' ')) {
			squad_unittypeid1.push_back(std::stoi(buffer));
		}
		linestream.clear();
		std::getline(fin, line);
		std::cout << line << std::endl;
		linestream << line;
		while (std::getline(linestream, buffer, ' ')) {
			squad_quantity1.push_back(std::stoi(buffer));
		}
		linestream.clear();
		std::getline(fin, line);
		std::cout << line << std::endl;
		linestream << line;
		while (std::getline(linestream, buffer, ' ')) {
			squad_unittypeid2.push_back(std::stoi(buffer));
		}
		linestream.clear();
		std::getline(fin, line);
		std::cout << line << std::endl;
		linestream << line;
		while (std::getline(linestream, buffer, ' ')) {
			squad_quantity2.push_back(std::stoi(buffer));
		}
		fin.close();
		std::cout << squad_unittypeid1.size() << squad_quantity1.size() << squad_unittypeid2.size() << squad_quantity2.size() << std::endl;
		return std::make_tuple(squad_unittypeid1, squad_quantity1, squad_unittypeid2, squad_quantity2);

	}

}