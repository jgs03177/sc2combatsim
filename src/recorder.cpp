#include "recorder.h"

#include "player.h"
#include "configurator.h"
#include "combinator.h"

#include "util.h"

#include "json/json.h"

#include <string>
#include <fstream>

Recorder::Recorder() {
	std::string timestr = Util::gettimestr();
	ofs_error.open("logs/" + timestr + " log.txt");
}

Recorder::~Recorder() {
	ofs_error.close();
}

Json::Value dump_squad(
	const std::vector<sc2::UnitTypeID>& squad_unittypeid,
	const std::vector<int>& squad_quantity) {
	size_t length = squad_unittypeid.size();
	assert(length == squad_quantity.size());
	Json::Value squad;
	for (int i = 0; i < length; i++) {
		squad[i]["UnitTypeID"] = static_cast<int>(squad_unittypeid[i]);
		squad[i]["Quantity"] = static_cast<int>(squad_quantity[i]);
	}
	return squad;
}

void Recorder::record_configs(const SimulatorConfig& config) {
	config.dump(o_config);
}

void Recorder::record_combination(const Player& player1, const Player& player2) {
	std::vector<sc2::UnitTypeID> squad_unittypeid;
	std::vector<int> squad_quantity;
	// initial squad combination of players.
	Json::Value& o_comb_p1 = o_combination["squad_p1"];
	Json::Value& o_comb_p2 = o_combination["squad_p2"];
	// resource left after generating squad of players.
	Json::Value& o_rscleft_p1 = o_combination["remaining_resource_p1"];
	Json::Value& o_rscleft_p2 = o_combination["remaining_resource_p2"];
	int mineral;
	int gas;
	float food;

    // Record remaining resources of players
	player1.combinator().get_resources(mineral, gas, food);
	o_rscleft_p1[0u] = mineral;
	o_rscleft_p1[1u] = gas;
	o_rscleft_p1[2u] = food;

	player2.combinator().get_resources(mineral, gas, food);
	o_rscleft_p2[0u] = mineral;
	o_rscleft_p2[1u] = gas;
	o_rscleft_p2[2u] = food;

	// Record squads of players
	player1.combinator().get_squad(squad_unittypeid, squad_quantity);
	o_comb_p1 = dump_squad(squad_unittypeid, squad_quantity);
	squad_unittypeid.clear();
	squad_quantity.clear();
	player2.combinator().get_squad(squad_unittypeid, squad_quantity);
	o_comb_p2 = dump_squad(squad_unittypeid, squad_quantity);
}

void Recorder::record_result(const Player& player1, const Player& player2,
	const std::string& result, int cframe) {
	std::vector<sc2::UnitTypeID> squad_unittypeid;
	std::vector<int> squad_quantity;
	Json::Value o_item;
	// remaining squad combination of players
	Json::Value& o_remain_p1 = o_item["squad_p1"];
	Json::Value& o_remain_p2 = o_item["squad_p2"];
	// combat result. timeout / p1_win / p2_win / draw
	Json::Value& o_result = o_item["result"];
	// frame count.
	Json::Value& o_frame = o_item["frame"];
	o_result = result;
	o_frame = cframe;

	// Record squads of players
	player1.PlacedUnit(squad_unittypeid, squad_quantity);
	o_remain_p1 = dump_squad(squad_unittypeid, squad_quantity);
	squad_unittypeid.clear();
	squad_quantity.clear();
	player2.PlacedUnit(squad_unittypeid, squad_quantity);
	o_remain_p2 = dump_squad(squad_unittypeid, squad_quantity);

	o_items.append(o_item);
}

void Recorder::writefile(const std::string& filename) const {
	Json::StreamWriterBuilder builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "";
	std::unique_ptr<Json::StreamWriter> const writer(
		builder.newStreamWriter());

	Json::Value o_text;
	o_text["config"] = o_config;
	o_text["combination"] = o_combination;
	o_text["items"] = o_items;

	std::ofstream ofs_output(filename);
	writer->write(o_text, &ofs_output);
	ofs_output.close();
}
