#include "bots.h"

#include <iostream>
#include <fstream>

using namespace sc2;

void Camerabot::OnGameStart() {
	std::cout << botname << "Hello! I am Camerabot " << botname << std::endl;
	const GameInfo game_info = Observation()->GetGameInfo();
	centerpos = (game_info.playable_max + game_info.playable_min) / 2.0f;
	campos = camoffset + centerpos;
}

void Camerabot::OnStep() {
	const ObservationInterface* observation = Observation();

	Units allyunits = observation->GetUnits(Unit::Alliance::Ally);
	Units enemyunits = observation->GetUnits(Unit::Alliance::Enemy);
	Point2D allycenter = campos;
	Point2D enemycenter = campos;
	if (allyunits.size()) {
		for (const auto& unit : allyunits) {
			allycenter += Point2D(unit->pos);
		}
		allycenter /= static_cast<float>(allyunits.size());
	}
	if (enemyunits.size()) {
		for (const auto& unit : enemyunits) {
			enemycenter += Point2D(unit->pos);
		}
		enemycenter /= static_cast<float>(enemyunits.size());
	}

	Debug()->DebugMoveCamera(allycenter);
	//Debug()->SendDebug();
}


void Simbot::OnGameStart() {
	std::cout << botname << "Hello! I am SimBot " << botname << std::endl;
	const GameInfo game_info = Observation()->GetGameInfo();
	centerpos = (game_info.playable_max + game_info.playable_min) / 2.0f;
	//centerpos = Point2D(game_info.width / 2.0f, game_info.height / 2.0f);
	campos = camoffset + centerpos;
}

void Simbot::OnUnitIdle(const Unit* unit) {
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

void Simbot::OnStep() {
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
		allycenter = campos;
	}
	Debug()->DebugMoveCamera(allycenter);
	//Debug()->SendDebug();

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
