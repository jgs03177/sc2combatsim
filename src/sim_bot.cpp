#include "json/json.h"
#include "sim_bot.h"

void SimBot::OnGameStart() {
	std::cout << botname << " Hello, World!" << std::endl;
	const GameInfo game_info = Observation()->GetGameInfo();
	centerpos = (game_info.playable_max + game_info.playable_min) / 2.0f;
	//centerpos = Point2D(game_info.width / 2.0f, game_info.height / 2.0f);
	genpos = offset + centerpos;
	combinator.SetUnitData(Observation());
}

void SimBot::OnUnitIdle(const Unit* unit) {
	const ObservationInterface* observation = Observation();
	ActionInterface* actions = Actions();
	//findnearestenemy
	Units allyunits = observation->GetUnits(Unit::Alliance::Self);
	Units enemyunits = observation->GetUnits(Unit::Alliance::Enemy);
	Point2D enemycenter;
	if (enemyunits.size()) {
		for (const auto& unit : enemyunits) {
			enemycenter += Point2D(unit->pos);
		}
		enemycenter /= static_cast<float>(enemyunits.size());
	}
	else {
		enemycenter = centerpos;
	}
	actions->UnitCommand(unit, ABILITY_ID::ATTACK, enemycenter);
}

void SimBot::OnStep() {
	const ObservationInterface* observation = Observation();
	//Units liberators = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_LIBERATOR));
	//Actions()->UnitCommand(liberators, ABILITY_ID::MORPH_LIBERATORAGMODE, Point2D(15.0f, 22.5f));
	//size_t medivac_size = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MEDIVAC)).size();
	//size_t my_units_size = observation->GetUnits(Unit::Alliance::Self).size();
	//size_t enemy_units_size = observation->GetUnits(Unit::Alliance::Enemy).size();
	//if (enemy_units_size && my_units_size == medivac_size) {
	//	d_killunit();
	//}
	
	//2p mode
	Units allyunits = observation->GetUnits(Unit::Alliance::Self);
	Point2D allycenter;
	if (allyunits.size()) {
		for (const auto& unit : allyunits) {
			allycenter += Point2D(unit->pos);
		}
		allycenter /= static_cast<float>(allyunits.size());
	}
	else {
		allycenter = genpos;
	}
	Debug()->DebugMoveCamera(allycenter);
	Debug()->SendDebug();

	//cpu mode
	/*
	if (CountUnitType(2) == 0 || CountUnitType(3) == 0) {
		d_killunit(2);
		d_killunit(3);
		d_deployunit(UNIT_TYPEID::TERRAN_MARINE, 3, centerpos + offset1, 3);
		d_deployunit(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR, 2, centerpos + offset2, 2);
	}
	*/
}

size_t SimBot::CountUnitType(UnitTypeID unit_type, uint32_t playerID) {
	return Observation()->GetUnits([unit_type, playerID](const Unit& unit) {return unit.owner == playerID && IsUnit(unit_type)(unit); }).size();
}
size_t SimBot::CountUnitType(UnitTypeID unit_type) {
	return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}
size_t SimBot::CountUnitType(uint32_t playerID) {
	return Observation()->GetUnits([playerID](const Unit& unit) {return unit.owner == playerID; }).size();
}
size_t SimBot::CountUnitType() {
	return Observation()->GetUnits(Unit::Alliance::Self).size();
}

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

void SimBot::d_countunits(Json::Value& squad, uint32_t playerID) {
	squad.clear();
	Units myunits = Observation()->GetUnits(
		[playerID](const Unit& unit) {return unit.owner == playerID; }
	);
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

bool SimBot::d_deployunit(UnitTypeID unit, uint32_t numbers, Vector2D pos, uint32_t playerID) {
	DebugInterface* debug = Debug();
	debug->DebugCreateUnit(unit, pos, playerID, numbers);
	debug->SendDebug();
	return true;
}

bool SimBot::d_deployunit(UnitTypeID unit, uint32_t numbers) {
	d_deployunit(unit, numbers, genpos, Observation()->GetPlayerID());
	return true;
}

bool SimBot::d_deployunit(Json::Value squad, Vector2D pos, uint32_t playerID) {
	//std::cout << "hello i am d_deployunit" << std::endl;
	DebugInterface* debug = Debug();
	Json::UInt len = squad.size();
	for (Json::UInt i = 0; i < len; i++) {
		int quantity = squad[i]["Quantity"].asInt();
		if (!quantity) continue;
		UnitTypeID unit = squad[i]["UnitTypeID"].asInt();
		std::cout << UnitTypeToName(unit.ToType()) << " " << quantity << std::endl;
		debug->DebugCreateUnit(unit, pos, playerID, quantity);
	}
	debug->DebugMoveCamera(pos);
	debug->SendDebug();
	return true;
}

bool SimBot::d_deployunit(Json::Value squad) {
	d_deployunit(squad, genpos, Observation()->GetPlayerID());
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

void SimBot::d_killunit(uint32_t playerID) {
	DebugInterface* debug = Debug();
	const ObservationInterface* observation = Observation();
	Units units = observation->GetUnits(
		[playerID](const Unit& unit) {return unit.owner == playerID; }
	);
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