#include "configurator.h"

#include "ProjectConfig.h"

#include "json/json.h"

#include <iostream>
#include <fstream>

void CombinatorConfig::load(const Json::Value& o_combinatorconfig) {
	inputpath = o_combinatorconfig["inputpath"].asString();
	limit_ore = o_combinatorconfig["limit_ore"].asFloat();
	limit_gas = o_combinatorconfig["limit_gas"].asFloat();
	limit_food = o_combinatorconfig["limit_food"].asFloat();
	index = o_combinatorconfig["index"].asInt();
}

void CombinatorConfig::dump(Json::Value& o_combinatorconfig) const {
	o_combinatorconfig["inputpath"] = inputpath;
	o_combinatorconfig["limit_ore"] = limit_ore;
	o_combinatorconfig["limit_gas"] = limit_gas;
	o_combinatorconfig["limit_food"] = limit_food;
	o_combinatorconfig["index"] = index;
}

void CombinatorConfig::use_default() {
	limit_ore = 10000;
	limit_gas = 5000;
	limit_food = 150;
	inputpath = "";
}

void PlayerConfig::load(const Json::Value& o_playerconfig) {
	offset.x = o_playerconfig["offset"][0u].asFloat();
	offset.y = o_playerconfig["offset"][1u].asFloat();
	race = sc2::Race(o_playerconfig["race"].asInt());
	name = o_playerconfig["name"].asString();
}

void PlayerConfig::dump(Json::Value& o_playerconfig) const {
	o_playerconfig["offset"][0u] = offset.x;
	o_playerconfig["offset"][1u] = offset.y;
	o_playerconfig["race"] = int(race);
	o_playerconfig["name"] = name;
}

void PlayerConfig::use_default(bool right) {
	race = sc2::Race::Random;
	offset = right? sc2::Vector2D(20, 0) : sc2::Vector2D(-20, 0);
}

SimulatorConfig::SimulatorConfig() {
	use_default();
}

SimulatorConfig::SimulatorConfig(const std::string& path) {
	read_from_path(path);
}

void SimulatorConfig::use_default(){
	numround = 2000;
	numrepeat = 5;
	stepsize = 5;
	mapname = PATH_MAP;
	outpath = PATH_OUTPUT;
	squadpath = "";

	combin1.use_default();
	combin2.use_default();
	player1.use_default(false);
	player2.use_default(true);

	port = 8167;
	mapname = "";
}

void SimulatorConfig::read_from_path(const std::string& path) {

	std::ifstream configf(path);

	if (!configf.good()) {
		std::cerr << "Opening config.json failed... Using default settings."
			<< std::endl;
		configf.close();
		use_default();
		return;
	}

	Json::Value o_config;
	configf >> o_config;
	configf.close();
	load(o_config);
}

void SimulatorConfig::write_to_path(const std::string& path) const {

	Json::Value o_config;
	dump(o_config);

	std::ofstream configf(path);
	if (!configf.good()) {
		std::cerr << "Opening config.json failed..."
			<< std::endl;
		configf.close();
		return;
	}

	configf << o_config;
	configf.close();
}

void SimulatorConfig::load(const Json::Value& o_config) {
	numround = o_config["numround"].asInt();
	numrepeat = o_config["numrepeat"].asInt();
	stepsize = o_config["stepsize"].asInt();
	port = o_config["port"].asInt();
	mapname = o_config.get("mapname", PATH_MAP).asString();
	outpath = o_config.get("outpath", PATH_OUTPUT).asString();
	squadpath = o_config.get("squadpath", "").asString();

	std::string s_simmode = o_config["simmode"].asString();

	if (s_simmode.compare("PvP") == 0) {
		simmode = SimMode::PvP;
	}
	else if (s_simmode.compare("PvC") == 0) {
		simmode = SimMode::PvC;
	}
	else if (s_simmode.compare("CvC") == 0) {
		simmode = SimMode::CvC;
	}
	else {
		simmode = SimMode::unknown;
		std::cerr << "Unknown simmode. Halting. (simmode is not one of PvP, PvC, CvC)"
			<< std::endl;
		exit(1);
	}

	const Json::Value& o_player1 = o_config["player1"];
	const Json::Value& o_player2 = o_config["player2"];

	const Json::Value& o_combin1 = o_config["combin1"];
	const Json::Value& o_combin2 = o_config["combin2"];

	player1.load(o_player1);
	player2.load(o_player2);

	combin1.load(o_combin1);
	combin2.load(o_combin2);
}

void SimulatorConfig::dump(Json::Value& o_config) const {
	o_config["numround"] = numround;
	o_config["numrepeat"] = numrepeat;
	o_config["stepsize"] = stepsize;
	o_config["port"] = port;
	o_config["mapname"] = mapname;
	o_config["outpath"] = outpath;
	o_config["squadpath"] = squadpath;

	switch (simmode) {
	case SimMode::PvP:
		o_config["simmode"] = "PvP";
		break;
	case SimMode::PvC:
		o_config["simmode"] = "PvC";
		break;
	case SimMode::CvC:
		o_config["simmode"] = "CvC";
		break;
	default:
		o_config["simmode"] = "unknown";
		break;
	}

	Json::Value& o_player1 = o_config["player1"];
	Json::Value& o_player2 = o_config["player2"];

	Json::Value& o_combin1 = o_config["combin1"];
	Json::Value& o_combin2 = o_config["combin2"];

	player1.dump(o_player1);
	player2.dump(o_player2);

	combin1.dump(o_combin1);
	combin2.dump(o_combin2);
}
