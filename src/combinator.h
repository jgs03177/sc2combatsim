#pragma once

#include "configurator.h"
#include "randomsample.h"

#include <sc2api/sc2_api.h>

#include <vector>

class CombinatorConfig;

class Combinator
{
public:
	// init
	void set_unitdata(const sc2::ObservationInterface* observation);
	void set_unitdata(const sc2::UnitTypes& unitdata);
	void set_config(const CombinatorConfig& config); // reset needs this

	// step 1a - reset resources & unitlist as stated in configs
	void reset();

	// step 1b (additional) - set resources
	void clear_resources();
	void set_resources(int mineral, int gas, float food);
	void add_resources(int mineral, int gas, float food);

	// step 1b (additional) - set unitlists
	void clear_unitlist();
	void add_unitlist(int index); // predefined index
	void add_unitlist(const std::string& unitname); // name
	void add_unitlist(const std::vector<sc2::UnitTypeID>& candidates); // unittypeids

	// step 2 - priorities are mixed
	bool pick_and_rearrange_candidates(float probability = 1.0f);
	
	// step 3 - make squad
	bool make_squad();
	bool make_squad_simultaneous();

	// step 4 - get all infos
	std::tuple<int, int, float> get_resources() const;
	std::vector<sc2::UnitTypeID> get_unitlist() const;
	std::tuple< std::vector<sc2::UnitTypeID>, std::vector<int> > get_squad() const;

	// load/dump
	void load_predefined_squad(
		const std::vector<sc2::UnitTypeID>& squad_unittypeid, 
		const std::vector<int>& squad_quantity
	);

private:
	RandomSample random;
    bool use_config = false;
	CombinatorConfig config;
	sc2::UnitTypes unitdata;
	int32_t mineral;
	int32_t gas;
	float food;	// due to zergling

	// candidates (before generation)
	std::vector<sc2::UnitTypeID> candidates;

	// generated squads (after generation)
	std::vector<sc2::UnitTypeID> squad_unittypeid;
	std::vector<int> squad_quantity;

	// Get max number of the given unit that we can buy with the given resource.
	float get_unit_affordable(sc2::UnitTypeID unittypeid);
	float get_unit_affordable(sc2::UnitTypeID unittypeid, int32_t mineral, int32_t gas, float food);

	// Subtract resource with the amount of the cost of the unit.
	void subtract_unit_resource(sc2::UnitTypeID unittypeid, int32_t num_units);

	// Convert namestring to unittypeid.
    // Warning: Input name must not include race : ex) zergling
	sc2::UnitTypeID NameToUnitTypeID(const std::string& name) const;
};
