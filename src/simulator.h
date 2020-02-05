#pragma once

#include <sc2api/sc2_api.h>

#include "player.h"
#include "recorder.h"
#include "configurator.h"

class Simulator {
private:
	int argc;
	char** argv;
	const SimulatorConfig config;
	Recorder recorder;
	
	Player p1;
	Player p2;
	sc2::Agent* a1;
	sc2::Agent* a2;

	bool neednewsquad;

	const int32_t nround;  // i.e iteration
	const int32_t nrepeat; // number of repetition of same simulation
	int32_t cround;
	int32_t crepeat;

	const int32_t stepsize;

	sc2::Coordinator* coordinator_;

public:
	~Simulator();
	Simulator(int argc, char* argv[], const SimulatorConfig& config);
	sc2::Coordinator* coordinator();
	void set();
	void reset();
	int Begin();
	int Update();
	void squadgen();
};
