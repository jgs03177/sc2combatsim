#include "json/json.h"

#include "sim_bot.h"

void SimBot::d_countunits(Json::Value& squad) {
	squad.clear();
	Units myunits = Observation()->GetUnits(Unit::Alliance::Self);
	// make a map
	std::map<UnitTypeID, int> unit_map;
	for (const auto& u : myunits) {
		UnitTypeID id = u->unit_type;
		if (!unit_map.count(id)) {
			unit_map[id] = 0;
		}
		unit_map[id] += 1;
	}
	Json::UInt i = 0;
	for (const auto& e : unit_map) {
		squad[i]["UnitTypeID"] = static_cast<int>(e.first);
		squad[i]["Quantity"] = static_cast<int>(e.second);
		i++;
	}
	return;
}

bool SimBot::d_deployunit(UnitTypeID unit, uint32_t numbers) {
	DebugInterface* debug = Debug();
	debug->DebugCreateUnit(unit, genpos, Observation()->GetPlayerID(), numbers);
	debug->SendDebug();
	return true;
}

bool SimBot::d_deployunit(Json::Value squad) {
	//std::cout << "hello i am d_deployunit" << std::endl;
	DebugInterface* debug = Debug();
	Json::UInt len = squad.size();
	for (Json::UInt i = 0; i < len; i++) {
		int quantity = squad[i]["Quantity"].asInt();
		if (!quantity) continue;
		UnitTypeID unit = squad[i]["UnitTypeID"].asInt();
		std::cout << UnitTypeToName(unit.ToType()) << " " << quantity << std::endl;
		debug->DebugCreateUnit(unit, genpos, Observation()->GetPlayerID(), quantity);
	}
	debug->DebugMoveCamera(genpos);
	debug->SendDebug();
	return true;
}

void SimBot::d_killunit() {
	DebugInterface* debug = Debug();
	const ObservationInterface* observation = Observation();
	Units units = observation->GetUnits(Unit::Alliance::Self);
	for (const auto& unit : units) {
		debug->DebugKillUnit(unit);
	}
	debug->SendDebug();
	return;
}
void SimBot::d_init() {
	DebugInterface* debug = Debug();
	debug->DebugShowMap();
	debug->SendDebug();
}
void SimBot::d_finish() {
	//DebugInterface* debug = Debug();
	//debug->DebugEndGame();
	//debug->SendDebug();
	Control()->RequestLeaveGame();
}

bool SimBot::d_construct_squad(Race race, int32_t& mineral, int32_t& gas, int32_t& food, Json::Value& squad_json, float probability) {
	return combinator.construct_squad(race, mineral, gas, food, squad_json, probability);
}