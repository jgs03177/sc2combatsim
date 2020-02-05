#pragma once

#include <sc2api/sc2_api.h>

#include "json/json-forwards.h"

#include <string>

struct CombinatorConfig {
public:
	std::string inputpath;
	float limit_ore;
	float limit_gas;
	float limit_food;

	int index;
	std::vector<sc2::UnitTypeID> unitlist;

	void load(const Json::Value& o_combinatorconfig);
	void use_default();
	void dump(Json::Value& o_combinatorconfig) const;
};

struct PlayerConfig {
public:
	sc2::Vector2D offset;
	sc2::Race race;
	std::string name;

	void load(const Json::Value& o_playerconfig);
	void use_default(bool right=true);
	void dump(Json::Value& o_playerconfig) const;
};

struct SimulatorConfig {
public:
	SimulatorConfig();
	SimulatorConfig(const std::string& path);

	int32_t numround;
	int32_t numrepeat;
	int32_t stepsize;
	int32_t port;
	std::string mapname;
	std::string outpath;
	std::string squadpath;
	enum class SimMode { PvP, PvC, CvC, unknown =-1 } simmode;

	CombinatorConfig  combin1;
	CombinatorConfig  combin2;
	PlayerConfig      player1;
	PlayerConfig      player2;

	void read_from_path(const std::string& path);
	void load(const Json::Value& o_config);
	void dump(Json::Value& o_config) const;
	void write_to_path(const std::string& path) const;
	void use_default();
};
