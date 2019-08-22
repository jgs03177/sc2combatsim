#include <iomanip>

#include "sc2utils/sc2_manage_process.h"

#include "simulator.h"

#define SCREENCAPTURE

std::string gettimestr() {
	time_t rawtime;
	struct tm timeinfo;
	time(&rawtime);
#if defined(_WIN32)
	localtime_s(&timeinfo, &rawtime);
#else
	timeinfo = *localtime(&rawtime);
#endif
	std::ostringstream o;
	//char buffer[80];
	//strftime(buffer, 80, "%Y-%m-%d %H-%M-%S", &timeinfo);
	o << timeinfo.tm_year + 1900 << '-'
		<< std::setw(2) << std::setfill('0') << timeinfo.tm_mon + 1 << '-'
		<< std::setw(2) << std::setfill('0') << timeinfo.tm_mday << ' '
		<< std::setw(2) << std::setfill('0') << timeinfo.tm_hour << '-'
		<< std::setw(2) << std::setfill('0') << timeinfo.tm_min << '-'
		<< std::setw(2) << std::setfill('0') << timeinfo.tm_sec;

	return o.str();
}

int Simulator::Begin() {
	enum {
		onstart,
		onchange,
		oncreate,
		indelay,
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
#if !defined(__linux__)
		sc2::SleepFor(1);	// to reduce load to cpu and prevent disconnection.
#endif
							//std::cout << cround << std::endl;
		if (simflag != oncheck) {
			ofs_error << simflag;
		}
		switch (simflag) {
			// initialization, turn on vision
		case onstart: {
			sim1->d_init();
			sim2->d_init();
			simflag = onchange;
			break;
		}
					  // fetch battles randomly
		case onchange: {
			if (neednewsquad) {
				int32_t ore1, gas1, food1;
				int32_t ore2, gas2, food2;

				ore1 = ore2 = ore;
				gas1 = gas2 = gas;
				food1 = food2 = food;
				Json::Value diff_resource;
				// use input in inputpath
				if (use_input) {
					std::ifstream inputfile = std::ifstream(inputpath + "/r_" + std::to_string(cround) + ".txt");
					if (!inputfile.good()) {
						std::cout << "cannot read r_" << std::to_string(cround) << ".txt. halting." << std::endl;
						ofs_error << "cannot read r_" << std::to_string(cround) << ".txt. halting." << std::endl;
						exit(1);
					}
					Json::Value i_text;
					inputfile >> i_text;
					inputfile.close();

					o_squad_sim1 = i_text["squad1"];
					o_squad_sim2 = i_text["squad2"];

					std::cout << "read successfully." << std::endl;

					ore = i_text["limit"]["ore"].asInt();
					gas = i_text["limit"]["gas"].asInt();
					food = i_text["limit"]["food"].asInt();
					diff_resource = i_text["diff"];
				}
				else {
					sim1->d_construct_squad(race_sim1, ore1, gas1, food1, o_squad_sim1, prob_combination);
					sim2->d_construct_squad(race_sim2, ore2, gas2, food2, o_squad_sim2, prob_combination);
					diff_resource["ore1"] = ore1;
					diff_resource["gas1"] = gas1;
					diff_resource["food1"] = food1;
					diff_resource["ore2"] = ore2;
					diff_resource["gas2"] = gas2;
					diff_resource["food2"] = food2;
				}
				recordhead(diff_resource);
				neednewsquad = false;
				crepeat = 0;
			}

			cdelay = 0;
#if !defined(__linux__)
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
			sim1->d_deployunit(o_squad_sim1);
			sim2->d_deployunit(o_squad_sim2);

			simflag = increate;
			break;
		}
					   // wait until the units to be created,
					   // because units are not created instantly.
		case increate: {
			size_t sim1size = sim1->CountUnitType();
			size_t sim2size = sim2->CountUnitType();
			if (sim1size != 0 && sim2size != 0) {
				cframe = 0;
				simflag = oncheck;
#if !defined(__linux__)
#ifdef SCREENCAPTURE
				sim1->screencapture("output/s_" + std::to_string(cround) + "_a" + std::to_string(crepeat) + ".png", img_compression);
				sim2->screencapture("output/s_" + std::to_string(cround) + "_b" + std::to_string(crepeat) + ".png", img_compression);
#endif // SCREENCAPTURE
#endif // !defined(__linux__)
			}
			break;
		}
					   // check if one side is eliminated.
		case oncheck: {
			size_t sim1size = sim1->CountUnitType();
			size_t sim2size = sim2->CountUnitType();
			// in battle
			if (sim1size != 0 && sim2size != 0 && cframe < nframe) {
				cframe++;
				break;
			}
			// battle ends
			else {
				std::cout << "elapsed frames: " << cframe << std::endl;
				simflag = oncount;
				break;
			}
		}
		case oncount: {
			size_t sim1size = sim1->CountUnitType();
			size_t sim2size = sim2->CountUnitType();

			std::string result;
			// timeout
			if (cframe >= nframe) {
				o_squad_remaining.clear();
				result = "timeout";
			}
			// sim1 win
			else if (sim1size != 0) {
				sim1->d_countunits(o_squad_remaining);
				result = "squad1_win";
			}
			// sim2 win
			else if (sim2size != 0) {
				sim2->d_countunits(o_squad_remaining);
				result = "squad2_win";
			}
			// draw
			else {
				o_squad_remaining.clear();
				result = "draw";
			}

			Json::Value o_item;

			o_item["result"] = result;
			o_item["frame"] = cframe;
			o_item["remain"] = o_squad_remaining;
			o_item["score"] = 0;

			recordbody(o_item);
			cbattle++;
			crepeat++;
			if (crepeat >= nrepeat) {
				writefile();
				neednewsquad = true;
				cround++; // after printing, count the rounds.
			}
			
			if (cround <= nround && cbattle < nbattle) {
				simflag = onremove;
				break;
			}
			else { // finished all iteration.
				simflag = onfinish;
				break;
			}
		}
		case onremove: {
			sim1->d_killunit();
			sim2->d_killunit();
			simflag = inremove;
			break;
		}
					   // wait until all units are cleared
		case inremove: {
			size_t sim1size = sim1->CountUnitType();
			size_t sim2size = sim2->CountUnitType();
			if (sim1size == 0 && sim2size == 0) {
				simflag = onchange;
			}
			break;
		}
					   // simulation end
		case onfinish: {
			sim1->d_finish();
			sim2->d_finish();

			simflag = infinish;
			break;
		}
					   // it takes about 4 steps to finish
		case infinish: {
			break;
		}
					   // error
		default: {
			std::cout << "State error." << std::endl;
			ofs_error << "State error." << std::endl;
			exit(1);
			break;
		}
		}
	}

	return 0;
}

Simulator::~Simulator() {
	ofs_outputraw << "]}" << std::endl;
	ofs_outputraw.close();
	ofs_error.close();
	delete coordinator_;
}

Simulator::Simulator(Json::Value param, int32_t nround, int32_t nrepeat, int32_t race, int32_t stepsize)
	: nround(nround), nrepeat(nrepeat), stepsize(stepsize) {
	SetRaces(race);
	SetResources(15000, 5000, 75);
	cround = 1;
	neednewsquad = true;
	img_compression = param.get("img_compression", 5).asInt();
	prob_combination = param.get("prob_combination", 1.0f).asFloat();
	o_note = param.get("note", Json::Value(Json::ValueType::arrayValue));;
	std::string timestr = gettimestr();
	ofs_outputraw.open("logs/" + timestr + " raw.txt");
	ofs_error.open("logs/" + timestr + " log.txt");
	ofs_outputraw << "{\"battles\" : [" << std::endl;
	coordinator_ = new Coordinator();
	use_input = false;
}

void Simulator::resetcoordinator() {
	delete coordinator_;
	coordinator_ = new Coordinator();
}

void Simulator::setcoordinator(int argc, char* argv[]) {

#if !defined(__linux__)
#ifdef SCREENCAPTURE
	sc2::RenderSettings settings(800, 600, 300, 300);
	coordinator_->SetRender(settings);
#endif // SCREENCAPTURE
#endif // !defined(__linux__)

	coordinator_->LoadSettings(argc, argv);
	coordinator_->SetWindowSize(960, 720);
	coordinator_->SetRealtime(false);
	coordinator_->SetMultithreaded(true);
	coordinator_->SetStepSize(stepsize);
}

Coordinator* Simulator::coordinator() {
	return coordinator_;
}

bool Simulator::IsSimulationFinished() {
	return nround < cround;
}

void Simulator::SetSims(SimBot* sim1, SimBot* sim2) {
	coordinator_->SetParticipants({
	CreateParticipant(race_sim1, sim1),
	CreateParticipant(race_sim2, sim2),
		});
	this->sim1 = sim1;
	this->sim2 = sim2;
}

void Simulator::SetRaces(int32_t race) {
	switch (race & 3) { // race p1
	case RACE1T:
		race_sim1 = Terran;
		break;
	case RACE1Z:
		race_sim1 = Zerg;
		break;
	case RACE1P:
		race_sim1 = Protoss;
		break;
	case RACE1R:
	default:
		race_sim1 = Random;
		break;
	}
	switch (race & 12) {
	case RACE2T:
		race_sim2 = Terran;
		break;
	case RACE2Z:
		race_sim2 = Zerg;
		break;
	case RACE2P:
		race_sim2 = Protoss;
		break;
	case RACE2R:
	default:
		race_sim2 = Random;
		break;
	}
}

void Simulator::SetResources(int32_t ore, int32_t gas, int32_t food) {
	this->ore = ore;
	this->gas = gas;
	this->food = food;
}

void Simulator::recordhead(const Json::Value& o_diff) {
	o_text.clear();
	o_text["stepsize"] = stepsize;
	o_text["squad1"] = o_squad_sim1;
	o_text["squad2"] = o_squad_sim2;
	if (o_note.isArray()) {
		o_text["note"] = o_note;
	}
	else {
		o_text["note"].append("nothing");
	}
	o_text["limit"]["ore"] = static_cast<int>(ore);
	o_text["limit"]["gas"] = static_cast<int>(gas);
	o_text["limit"]["food"] = static_cast<int>(food);
	o_text["diff"] = o_diff;
}

void Simulator::recordbody(const Json::Value& o_item) {
	o_text["item"].append(o_item);
}

void Simulator::writefile() {
	// json output 파일 크기 줄이기 (공백 지우기)
	Json::StreamWriterBuilder builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "";
	std::unique_ptr<Json::StreamWriter> const writer(
		builder.newStreamWriter());
	writer->write(o_text, &ofs_outputraw);
	//ofs_outputraw << o_text;
	if (cround <= nround) {
		ofs_outputraw << "," << std::endl;
	}

	std::ofstream ofs_output_chunk("output/r_" + std::to_string(cround) + ".txt");
	ofs_output_chunk << o_text;
	ofs_output_chunk.close();
	o_text.clear();
}

void Simulator::SetInputpath(const std::string& path) {
	inputpath = path;
	use_input = true;
}