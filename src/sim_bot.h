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
	Vector2D offset1;
	Vector2D offset2;
	Point2D genpos;
	Point2D centerpos;
	Combinator combinator;

public:
	SimBot(std::string name, Vector2D offset) :
		Agent(), botname(name), offset(offset) {};

	SimBot(std::string name, Vector2D offset1, Vector2D offset2) :
		Agent(), botname(name), offset1(offset1), offset2(offset2) {};

	~SimBot() {};

	void OnGameStart() final override;

	void OnUnitIdle(const Unit* unit) final override;

	void OnStep() final override;

	size_t CountUnitType(UnitTypeID unit_type, uint32_t playerID);
	size_t CountUnitType(UnitTypeID unit_type);
	size_t CountUnitType(uint32_t playerID);
	size_t CountUnitType();

	void d_countunits(Json::Value& squad, uint32_t playerID);
	void d_countunits(Json::Value& squad);
	bool d_deployunit(UnitTypeID unit, uint32_t numbers, Vector2D pos, uint32_t playerID);
	bool d_deployunit(UnitTypeID unit, uint32_t numbers);
	bool d_deployunit(Json::Value squad, Vector2D pos, uint32_t playerID);
	bool d_deployunit(Json::Value squad);
	void d_killunit(uint32_t playerID);
	void d_killunit();
	void d_init();
	void d_finish();
	bool d_construct_squad(Race race, int32_t& mineral, int32_t& gas, int32_t& food, Json::Value& squad_json, float probability = 1.0f);
	void screencapture(const std::string& filename, int compression = 5);
};