#pragma once

#include <fstream>

#include <sc2api/sc2_api.h>

#include "json/json.h"

#include "sim_bot.h"

constexpr auto RACE1R = 0;
constexpr auto RACE1T = 1;
constexpr auto RACE1Z = 2;
constexpr auto RACE1P = 3;
constexpr auto RACE2R = 0;
constexpr auto RACE2T = 4;
constexpr auto RACE2Z = 8;
constexpr auto RACE2P = 12;

class Simulator {
private:
	SimBot* sim1;
	SimBot* sim2;

	// output
	std::ofstream ofs_outputraw;
	std::ofstream ofs_error;
 
	const int32_t nround;  // i.e iteration
	const int32_t nrepeat; // number of repetition of same simulation
	int32_t cround;
	int32_t crepeat;

	bool neednewsquad;

	int32_t race;
	const int32_t stepsize;

	//limits
	int32_t ore;
	int32_t gas;
	int32_t food;

	sc2::Race race_sim1;
	sc2::Race race_sim2;
	
	Json::Value o_squad_sim1;
	Json::Value o_squad_sim2;
	Json::Value o_squad_remaining;
	Json::Value o_text;
	Json::Value o_note;

	int img_compression;
	float prob_combination;

	Coordinator* coordinator_;

	std::string inputpath; // constructed squad input
	bool use_input;

public:
	~Simulator();
	Simulator(Json::Value param, int32_t nround, int32_t nrepeat, int32_t race = 0, int32_t stepsize = 1);
	void resetcoordinator();
	void setcoordinator(int argc, char* argv[]);
	Coordinator* coordinator();
	bool IsSimulationFinished();
	void SetSims(SimBot* sim1, SimBot* sim2);
	void SetRaces(int32_t race);
	// setresource is ignored when setinputpath is given.
	void SetResources(int32_t ore, int32_t gas, int32_t food);
	// generate one side of army by picking one raw on the given input array.
	int Begin();
	void recordhead(const Json::Value&);
	void recordbody(const Json::Value&);
	void writefile();
	void SetInputpath(const std::string&);
};
