#include "simulator.h"
#include "configurator.h"

#include "ProjectConfig.h"
#include <iostream>

int main(int argc, char* argv[]) {
	std::cout << "SC2Sim version " 
		      << SC2CombatSim_VERSION_MAJOR 
		      << "."
		      << SC2CombatSim_VERSION_MINOR 
		      << std::endl;
	SimulatorConfig config(PATH_CONFIG);
	Simulator simulator(argc, argv, config);

	simulator.Begin();

	return 0;
}
