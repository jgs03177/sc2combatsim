
#include "simulator.h"
#include "configurator.h"

#include "ProjectConfig.h"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
	std::cout << "SC2SquadGen version " 
		      << SC2CombatSim_VERSION_MAJOR 
		      << "."
		      << SC2CombatSim_VERSION_MINOR 
		      << std::endl;
	SimulatorConfig config(PATH_CONFIG);
	Simulator simulator(argc, argv, config);

	simulator.squadgen();

	return 0;
}


void Simulator::squadgen(){
    
    set();
    
    int ngen = 20000;

	std::cout << "launch!" << std::endl;
	Coordinator()->LaunchStarcraft();

	std::cout << "start!" << std::endl;
    Coordinator()->StartGame(config.mapname);

	std::cout << "Game Begins!" << std::endl;

    Coordinator()->Update();
    
    p1.GameInit();
    
    std::vector<sc2::UnitTypeID> squad_unittypeid;
    std::vector<int> squad_quantity;
    std::ofstream fout;
    bool firstelem;

    for (int i=0 ; i < ngen ; i++){
        fout.open("s_" + std::to_string(i) + ".txt");
        squad_unittypeid.clear();
	    squad_quantity.clear();
        p1.combinator().clear_unitlist();
        p1.combinator().reset();
        p1.combinator().pick_and_rearrange_candidates();
        p1.combinator().make_squad();
        std::tie(squad_unittypeid, squad_quantity) = p1.combinator().get_squad();
        
        firstelem = true;
        for (int uid : squad_unittypeid) {
            if (!firstelem) {
                fout << " ";
            }
            firstelem = false;
            fout << uid;
        }
        fout << std::endl;

        firstelem = true;
        for (int uq : squad_quantity) {
            if (!firstelem) {
                fout << " ";
            }
            firstelem = false;
            fout << uq;
        }
        fout << std::endl;

        fout.close();
    }
    
    return;
}
