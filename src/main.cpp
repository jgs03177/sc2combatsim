#include <string>
#include <iostream>
#include <fstream>

#include <sc2api/sc2_api.h>
#include <sc2utils/sc2_manage_process.h>

#include "sim_bot.h"
#include "simulator.h"

#include "ProjectConfig.h"

constexpr auto num_round_ = 2000;
constexpr auto num_repeat_ = 5;
constexpr auto stepsize_ = 5;

constexpr auto limit_ore_ = 10000;
constexpr auto limit_gas_ = 5000;
constexpr auto limit_food_ = 150;

using namespace sc2;

int main(int argc, char* argv[]) {
	Vector2D loc1 = Vector2D(0, -10);
	Vector2D loc2 = Vector2D(0, 10);
	int num_round = num_round_;
	int num_repeat = num_repeat_;
	int stepsize = stepsize_;
	int limit_ore = limit_ore_;
	int limit_gas = limit_gas_;
	int limit_food = limit_food_;
	int race = RACE1R | RACE2R;
	int port = 8167;
	std::string mapname = PATH_MAPS;
	std::string inputpath = "";
	bool hasinput = false;

	Json::Value configjson;
	Json::Value paramjson;
	std::ifstream configf(PATH_CONFIG);
	if (configf.good()) {
		configf >> configjson;
		if (configjson.isMember("inputpath")) {
			inputpath = configjson["inputpath"].asString();
			hasinput = true;
		}
		loc1.x = configjson["loc1"].get("x", 0.0f).asFloat();
		loc1.y = configjson["loc1"].get("y", -10.0f).asFloat();
		loc2.x = configjson["loc2"].get("x", 0.0f).asFloat();
		loc2.y = configjson["loc2"].get("y", 10.0f).asFloat();
		num_round = configjson.get("num_round", num_round_).asInt();
		num_repeat = configjson.get("num_repeat", num_repeat_).asInt();
		stepsize = configjson.get("stepsize", stepsize_).asInt();
		race = configjson.get("race", race).asInt();
		port = configjson.get("port", port).asInt();
		limit_ore = configjson.get("ore", limit_ore_).asInt();
		limit_gas = configjson.get("gas", limit_gas_).asInt();
		limit_food = configjson.get("food", limit_food_).asInt();
		mapname = configjson.get("map", mapname).asString();
		paramjson["img_compression"] = configjson.get("img_compression", 5).asInt();
		paramjson["prob_combination"] = configjson.get("prob_combination", 1.0f).asFloat();
		paramjson["note"] = configjson.get("note", Json::Value(Json::ValueType::arrayValue));
	}
	else {
		std::cout << "cannot read config.json. using default settings." << std::endl;
	}
	configf.close();
	
	SimBot *sim1, *sim2;
	//RACE1P, RACE2T
	//RACE1T | RACE2T
	Simulator simulator(paramjson, num_round, num_repeat, race, stepsize);

	//SetResources(10000, 3300, 50);
	simulator.SetResources(limit_ore, limit_gas, limit_food);
	simulator.setcoordinator(argc, argv);
	sim1 = new SimBot("sim1", loc1);
	sim2 = new SimBot("sim2", loc2);
	simulator.SetSims(sim1, sim2);	// simulator.coordinator()->setparticipants();
	simulator.coordinator()->SetPortStart(port);
	if (hasinput) {
		simulator.SetInputpath(inputpath);
	}

#if defined(__linux__)
#ifdef screencapture
#if LINUX_USE_SOFTWARE_RENDER
	simulator.coordinator()->AddCommandLine("-osmesapath /usr/lib/x86_64-linux-gnu/libOSMesa.so");
#else
	simulator.coordinator()->AddCommandLine("-eglpath libEGL.so");
#endif
#endif // screencapture
#endif // defined(__linux__)

	while (!simulator.IsSimulationFinished()) {

		std::cout << "launch!" << std::endl;
		simulator.coordinator()->LaunchStarcraft();

		std::cout << "start!" << std::endl;
		// maximum 2 player, visions are shared.
		simulator.coordinator()->StartGame(mapname);

		std::cout << "Game Begins!" << std::endl;
		simulator.Begin();	// while (simulator.update()){}

		bool properexit;

#if defined(__linux__)
		properexit = false;
#else
// code to check disconnection in windows OS.
		properexit = true;

		if (sim1->Control()->GetAppState() != AppState::normal ||
			sim2->Control()->GetAppState() != AppState::normal) {
			std::cout << "sim1 is " << static_cast<int>(sim1->Control()->GetAppState()) << std::endl;
			std::cout << "sim2 is " << static_cast<int>(sim2->Control()->GetAppState()) << std::endl;
			properexit = false;
		}
		if (sim1->Control()->IsInGame() || !sim1->Control()->IsReadyForCreateGame()) {
			std::cout << "sim1 is in game!" << std::endl;
			properexit = false;
		}
		if (sim2->Control()->IsInGame() || !sim2->Control()->IsReadyForCreateGame()) {
			std::cout << "sim2 is in game!" << std::endl;
			properexit = false;
		}
#endif
		// if the game is not properly terminated, do these.
		if (!properexit){
			std::cout << "Something's wrong! Trying to reset!" << std::endl;
			simulator.coordinator()->LeaveGame();
			SleepFor(1000);

			delete sim1;
			delete sim2;

			simulator.resetcoordinator();
			simulator.setcoordinator(argc, argv);
			sim1 = new SimBot("sim1", loc1);
			sim2 = new SimBot("sim2", loc2);
			simulator.SetSims(sim1, sim2);	// simulator.coordinator()->setparticipants();
			simulator.coordinator()->SetPortStart(port);
		}
		
		// wait child processes for 1 sec to die.
		SleepFor(1000);
	}
	delete sim1;
	delete sim2;

	return 0;
}
