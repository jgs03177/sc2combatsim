#pragma once

#include "json/json.h"

#include <fstream>

class Player;
class SimulatorConfig;

class Recorder {

private:
	//simulator configuration.
	Json::Value o_config;
	Json::Value o_combination;
	Json::Value o_items;
	std::ofstream ofs_error;

public:
	Recorder();
	~Recorder();

	void record_configs(const SimulatorConfig& config);
	void record_combination(const Player& player1, const Player& player2);
	void record_result(const Player& p1, const Player& p2, const std::string& result, int cframe);

	void writefile(const std::string& filename) const;
	void clear();

	std::ostream& err() { return ofs_error; }
};