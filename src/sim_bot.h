#pragma once
#include <iostream>
#include <fstream>

#include <sc2api/sc2_api.h>

#include "json/json-forwards.h"

#include "combinator.h"

using namespace sc2;

class SimBot : public Agent {
private:
	std::string botname;
	Vector2D offset;
	Point2D genpos;
	Point2D centerpos;
	Combinator combinator;

public:
	SimBot(std::string name, Vector2D offset) :
		Agent(), botname(name), offset(offset) {};

	~SimBot() {};

	virtual void OnGameStart() final override {
		std::cout << botname << " Hello, World!" << std::endl;
		const GameInfo game_info = Observation()->GetGameInfo();
		centerpos = (game_info.playable_max + game_info.playable_min) / 2.0f;
		//centerpos = Point2D(game_info.width / 2.0f, game_info.height / 2.0f);
		genpos = offset + centerpos;
		combinator.SetUnitData(Observation());
	}

	virtual void OnUnitIdle(const Unit* unit) final override {
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

	virtual void OnStep() final override {
		const ObservationInterface* observation = Observation();
		//Units liberators = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_LIBERATOR));
		//Actions()->UnitCommand(liberators, ABILITY_ID::MORPH_LIBERATORAGMODE, Point2D(15.0f, 22.5f));
		//size_t medivac_size = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MEDIVAC)).size();
		//size_t my_units_size = observation->GetUnits(Unit::Alliance::Self).size();
		//size_t enemy_units_size = observation->GetUnits(Unit::Alliance::Enemy).size();
		//if (enemy_units_size && my_units_size == medivac_size) {
		//	d_killunit();
		//}
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
	}

	size_t CountUnitType(UnitTypeID unit_type) {
		return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
	}
	size_t CountUnitType() {
		return Observation()->GetUnits(Unit::Alliance::Self).size();
	}

	void d_countunits(Json::Value& squad);
	bool d_deployunit(UnitTypeID unit, uint32_t numbers);
	bool d_deployunit(Json::Value squad);
	void d_killunit();
	void d_init();
	void d_finish();
	bool d_construct_squad(Race race, int32_t& mineral, int32_t& gas, int32_t& food, Json::Value& squad_json, float probability = 1.0f);
	void screencapture(const std::string& filename, int compression = 5);
};