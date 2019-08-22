#pragma once
#include <sc2api/sc2_api.h>

#include "json/json-forwards.h"

using namespace sc2;

class Combinator
{
public:
	//init
	void SetUnitData(const ObservationInterface* observation);

	// get the squad randomly (in dirichlet distribution).
	bool construct_squad(Race race, int32_t& mineral, int32_t& gas, int32_t& food, Json::Value& squad_json, float probability);
	// old dirichlet
	bool construct_squad_d1(Race race, int32_t& mineral, int32_t& gas, int32_t& food, Json::Value& squad_json, float probability);
	// old
	bool construct_squad_old(Race race, int32_t& mineral, int32_t& gas, int32_t& food, Json::Value& squad_json, float probability);

private:
	UnitTypes unitdata;

	// get list of units and pick with inclusion probability p.
	int32_t get_squad_permutation(Race race, std::vector<UnitTypeID>& unit_permutations, float probability = 0.5);

	// Warning: Input name must not include race : ex) zergling
	UnitTypeID NameToUnitTypeID(std::string name);

	// and pick units with inclusion probability p and rearrange orders.
	int32_t get_squad_permutation(const std::vector<UnitTypeID>& candidate_units, std::vector<UnitTypeID>& unit_permutations, float probability);

	// get max number of the given unit that we can buy with the given resource.
	float get_unit_affordable(UnitTypeID unittypeid, int32_t mineral, int32_t gas, int32_t food);

	// subtract resource with the amount of the cost of the unit.
	void subtract_unit_resource(UnitTypeID unittypeid, int32_t& mineral, int32_t& gas, int32_t& food, int32_t num_units);

	// buy some specific unit with some uniformly random number. 
	int32_t buy_unit_uniformly(UnitTypeID unittypeid, int32_t& mineral, int32_t& gas, int32_t& food);
};

