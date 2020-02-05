#include "simulator.h"

#include "bots.h"
#include "util.h"

#include <sc2api/sc2_api.h>
#include <sc2utils/sc2_manage_process.h>

#include <iostream>

//#define SCREENCAPTURE

int Simulator::Begin() {

	set();

	while (nround >= cround) { // While simulation is not finished. cround |-> Z_[1, nround] 

		std::cout << "launch!" << std::endl;
		coordinator_->LaunchStarcraft();

		std::cout << "start!" << std::endl;
		// maximum 2 player, visions are shared.
		coordinator_->StartGame(config.mapname);

		std::cout << "Game Begins!" << std::endl;
		Update();	// while (simulator.update()){}

		bool properexit;

#if defined(__linux__)
		properexit = false;
#else
		// code to check disconnection in windows OS.
		properexit = true;

		sc2::Agent* sim1 = p1.Bot();
		sc2::Agent* sim2 = p2.Bot();

		if (sim1->Control()->GetAppState() != sc2::AppState::normal ||
			sim2->Control()->GetAppState() != sc2::AppState::normal) {
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
		if (!properexit) {
			std::cout << "Something's wrong! Trying to reset!" << std::endl;
			coordinator_->LeaveGame();
			sc2::SleepFor(1000);

			reset();
		}

		// wait child processes for 1 sec to die.
		sc2::SleepFor(1000);
	}
	return 0;
}

int Simulator::Update() {
	enum {
		onstart,
		onchange,
		indelay,
		oncreate,
		increate,
		oncheck,
		oncount,
		onremove,
		inremove,
		onfinish,
		infinish
	} simflag = onstart;

	// nframe: frame limit for timeout
	// nbattle: battle limit for one sc2 process. total process time < 6:07:09
	const int32_t nframe = 10000 / stepsize;	// currently 5.5minutes
	const int32_t nbattle = 200;
	const int32_t ndelay = 5;
	int32_t cframe = 0;
	int32_t cbattle = 0;
	int32_t cdelay = 0;

	while (coordinator_->Update()) {
		std::cout << std::hex << simflag << std::dec << std::flush;
#if !defined(__linux__)
		sc2::SleepFor(1);	// to reduce load to cpu and prevent disconnection.
#endif
		//std::cout << cround << std::endl;
		if (simflag != oncheck) {
			recorder.err() << simflag;
		}
		switch (simflag) {
		// initialization, turn on vision
		case onstart: {
			p1.GameInit();
			p2.GameInit();
			p1.ShowMap();
			p2.ShowMap();
			simflag = onchange;
			break;
		}
		// fetch battles randomly
		case onchange: {
			if (neednewsquad) {
				if (config.squadpath.compare("") != 0){
					std::vector<sc2::UnitTypeID> squad_unittypeid1, squad_unittypeid2;
					std::vector<int> squad_quantity1, squad_quantity2;

					std::string path = config.squadpath + "/b_" + std::to_string(cround - 1) + ".txt";
					std::tie(squad_unittypeid1, squad_quantity1, squad_unittypeid2, squad_quantity2) = Util::read(path);
					p1.combinator().clear_unitlist();
					p1.combinator().reset();
					p1.combinator().load_predefined_squad(squad_unittypeid1, squad_quantity1);
					p2.combinator().clear_unitlist();
					p2.combinator().reset();
					p2.combinator().load_predefined_squad(squad_unittypeid2, squad_quantity2);
				}
				else{
					p1.combinator().clear_unitlist();
					p1.combinator().reset();
					p1.combinator().pick_and_rearrange_candidates();
					p1.combinator().make_squad();
					p2.combinator().clear_unitlist();
					p2.combinator().reset();
					p2.combinator().pick_and_rearrange_candidates();
					p2.combinator().make_squad();
				}
				recorder.record_combination(p1, p2);
				neednewsquad = false;
				crepeat = 0;
			}

#if !defined(__linux__)
			cdelay = 0;
			simflag = indelay;
#else
			simflag = oncreate;
#endif
			break;
		}
		 // delay for waiting death remainder to disappear
		case indelay: {
			if (cdelay >= ndelay)
				simflag = oncreate;
			cdelay++;
			break;
		}
		// create units for battle
		case oncreate: {
			p1.DeployUnit();
			p2.DeployUnit();

			simflag = increate;
			break;
		}
		// wait until the units to be created,
		// because units are not created instantly.
		case increate: {
			size_t sim1size = p1.CountPlayerUnit();
			size_t sim2size = p2.CountPlayerUnit();
			if (sim1size != 0 && sim2size != 0) {
				cframe = 0;
				simflag = oncheck;
#if !defined(__linux__)
#ifdef SCREENCAPTURE
				const std::string imgname1 = 
					config.outpath + "/s" + std::to_string(cround) + "_a" + std::to_string(crepeat) + ".png";
				const std::string imgname2 = 
					config.outpath + "/s" + std::to_string(cround) + "_b" + std::to_string(crepeat) + ".png";

				// TODO: Move Camera Properly (on CvC)
				p1.MoveCamera();
				p1.ScreenCapture(imgname1);
				p2.MoveCamera();
				p2.ScreenCapture(imgname2);
#endif // SCREENCAPTURE
#endif // !defined(__linux__)
			}
			break;
		}
		// check if one side is eliminated.
		case oncheck: {
			size_t sim1size = p1.CountPlayerUnit();
			size_t sim2size = p2.CountPlayerUnit();
			// in battle
			if (sim1size != 0 && sim2size != 0 && cframe < nframe) {
				cframe++;
				break;
			}
			// battle ends
			else {
				std::cout << " elapsed frames: " << cframe << std::endl;
				simflag = oncount;
				break;
			}
		}
		case oncount: {
			size_t sim1size = p1.CountPlayerUnit();
			size_t sim2size = p2.CountPlayerUnit();

			std::string result;
			// timeout
			if (cframe >= nframe) {
				result = "timeout";
			}
			// sim1 win
			else if (sim1size != 0) {
				result = "p1_win";
			}
			// sim2 win
			else if (sim2size != 0) {
				result = "p2_win";
			}
			// draw
			else {
				result = "draw";
			}

			recorder.record_result(p1, p2, result, cframe);
			cbattle++;
			crepeat++;
			if (crepeat >= nrepeat) {
				recorder.writefile(config.outpath + "/r_" + std::to_string(cround) + ".json");
				recorder.clear();
				neednewsquad = true;
				cround++; // after printing, count the rounds.
			}
			
			if (cround <= nround && cbattle < nbattle) {
				simflag = onremove;
				break;
			}
			else { // finish the simulation (and restart if needed).
				simflag = onfinish;
				break;
			}
		}
		case onremove: {
			p1.KillPlayerUnit();
			p2.KillPlayerUnit();
			simflag = inremove;
			break;
		}
		// wait until all units are cleared
		case inremove: {
			size_t sim1size = p1.CountPlayerUnit();
			size_t sim2size = p2.CountPlayerUnit();
			if (sim1size == 0 && sim2size == 0) {
				simflag = onchange;
			}
			break;
		}
		// simulation end
		case onfinish: {
			coordinator_->LeaveGame();
			if (p1.Bot() == p2.Bot()) {
				p1.Bot()->Control()->RequestQuit();
			}

			cdelay = 0;
			simflag = infinish;
			break;
		}
		// finishing. it takes about 4 steps to finish.
		case infinish: {
			break;
		}
		// error
		default: {
			std::cout << "State error." << std::endl;
			exit(1);
			break;
		}
		}

		p1.SendDebug();
		if (p1.Bot() != p2.Bot()) {
			p2.SendDebug();
		}
	}

	std::cout << "end of a game." << std::endl;

	return 0;
}

Simulator::~Simulator() {
	delete coordinator_;
}

Simulator::Simulator(int argc, char* argv[], const SimulatorConfig& config) :
	cround(1),
	crepeat(0),
	neednewsquad(true),
	argc(argc),
	argv(argv),
	config(config),
	nround(config.numround),
	nrepeat(config.numrepeat),
	stepsize(config.stepsize),
	a1(nullptr),
	a2(nullptr),
	coordinator_(nullptr)
{
}

void Simulator::set(){
	coordinator_ = new sc2::Coordinator();
#ifdef SCREENCAPTURE
#if !defined(__linux__)
	sc2::RenderSettings settings(800, 600, 300, 300);
	coordinator_->SetRender(settings);
#endif // !defined(__linux__)
#endif // SCREENCAPTURE

	coordinator_->LoadSettings(argc, argv);
	coordinator_->SetWindowSize(960, 720);
	coordinator_->SetRealtime(false);
	coordinator_->SetMultithreaded(true);
	coordinator_->SetStepSize(stepsize);

	p1.setConfig(config.player1);
	p2.setConfig(config.player2);
	p1.combinator().set_config(config.combin1);
	p2.combinator().set_config(config.combin2);

	if (config.simmode == SimulatorConfig::SimMode::CvC) {
		a1 = new Camerabot("CameraBot");
		p1.setBot(a1, 3);
		p2.setBot(a1, 4);
		coordinator_->SetParticipants({
			CreateParticipant(config.player1.race, a1)
		});
	}
	else if (config.simmode == SimulatorConfig::SimMode::PvC) {
		a1 = new Simbot("PlayerBot");
		a2 = new Camerabot("CameraBot");
		p1.setBot(a1, 1);
		p2.setBot(a2, 5);
		coordinator_->SetParticipants({
			CreateParticipant(config.player1.race, a1),
			CreateParticipant(config.player2.race, a2),
		});
	}
	else {  // config.simmode == SimulatorConfig::SimMode::PvP
		a1 = new Simbot("Player1Bot");
		a2 = new Simbot("Player2Bot");
		p1.setBot(a1, 1);
		p2.setBot(a2, 2);
		coordinator_->SetParticipants({
			CreateParticipant(config.player1.race, a1),
			CreateParticipant(config.player2.race, a2),
		});
	}
	
	coordinator_->SetPortStart(config.port);

#ifdef SCREENCAPTURE
#if defined(__linux__)
#if LINUX_USE_SOFTWARE_RENDER
	coordinator_->AddCommandLine("-osmesapath /usr/lib/x86_64-linux-gnu/libOSMesa.so");
#else
	coordinator_->AddCommandLine("-eglpath libEGL.so");
#endif // LINUX_USE_SOFTWARE_RENDER
#endif // defined(__linux__)
#endif // SCREENCAPTURE
}

void Simulator::reset() {
	delete coordinator_;
	if (config.simmode == SimulatorConfig::SimMode::CvC) {
		delete a1;
	}
	else if (config.simmode == SimulatorConfig::SimMode::PvC) {
		delete a1;
	}
	else {  // config.simmode == SimulatorConfig::SimMode::PvP
		delete a1;
		delete a2;
	}
	set();
}

sc2::Coordinator* Simulator::coordinator() {
	return coordinator_;
}

