#include "combinator.h"

#include "json/json.h"

#include <iostream>
#include <random>
#include <cmath>
#include <cctype>

std::random_device device;
std::mt19937_64 generator(device());

/* //////////
Terran - ground
    "scv",			// 45
	"marine",		// 48
	"marauder",		// 51
	"reaper",		// 49
	"ghost",		// 50
	"hellion",		// 53
	"helliontank",	// 484
	"siegetank",	// 33, sieged 32
	"cyclone",		// 692 /
	"thor",			// 52, ap 691
	"vikingassault", //assault 34, fighter 35
	// "widowmine",

Terran - air
	"medivac",
	"liberator",    //689, ag 734
	"banshee",
	"raven",
	"battlecruiser",
	
Zerg - ground
	"drone",		// 104
	"queen",		// 126
	"zergling",		// 105
	"baneling",		// 9
	"roach",		// 110
	"ravager",		// 688
	"hydralisk",	// 107
	"ultralisk",	// 109
    //"lurkermpburrowed", // 911, burrowed 912
	                      // mp 502, mpburrowed 503
	//"infestor",   // 111
	//"swarmhostburrowedmp",  //mp 494, burrowedmp 493
	
Zerg - air
	"mutalisk",
	"corruptor",
	"viper",
	"broodlord",
	"overlord",
	"overseer",

Protoss - ground
	"probe",		// 84
	"zealot",		// 73
	"sentry",		// 77
	"stalker",		// 74
	"adept",		// 311
	"hightemplar",	// 75
	"darktemplar",	// 76
	"archon",		// 141
	"immortal",		// 83
	"colossus",		// 4
	//"disruptor",

Protoss - air
	"phoenix",
	"voidray",
	"oracle",
	"tempest",
	"carrier",
	//"mothership",
	//"observer",
	//"warpprism",

////////// */
const static std::vector<std::vector<std::string>> predefined_cand({
	// #0 - reserved.
	{},
	// #1 - Terran units, movable, ground-to-ground weapons.
	{
		//"scv",			// 45
		"marine",		// 48
		"marauder",		// 51
		"reaper",		// 49
		"ghost",		// 50
		"hellion",		// 53
		"helliontank",	// 484
		"siegetank",	// siegetank 33, siegetanksieged 32
		"cyclone",		// 692
		"thor",			// thor 52, thorap 691
		"vikingassault",// vikingassault 34, vikingfighter 35
	},
	// #2 - Zerg, movable, ground-to-ground weapons.
	{
		//"drone",		// 104
		"queen",		// 126
		"zergling",		// 105
		"baneling",		// 9
		"roach",		// 110
		"ravager",		// 688
		"hydralisk",	// 107
		"ultralisk",	// 109
	},
	// #3 - Protoss, movable, ground-to-ground weapons.
	{
		//"probe",		// 84
		"zealot",		// 73
		"sentry",		// 77
		"stalker",		// 74
		"adept",		// 311
		"hightemplar",	// 75
		"darktemplar",	// 76
		"archon",		// 141
		"immortal",		// 83
		"colossus",		// 4
	},
	// #4 - Terran, movable, air-to-air/ground units.
	{
		"liberator",    // liberator 689, liberatorag 734
		"banshee",
		"raven",
		"battlecruiser",
	},
	// #5 - Zerg, movable, air-to-air/ground units.
	{
		"mutalisk",
		"corruptor",
		"viper",
		"broodlord",
	},
	// #6 - Protoss, movable, air-to-air/ground units.
	{
		"phoenix",
		"voidray",
		"tempest",
		"carrier",
	},
	// #7 - Terran, specially excluded.
	{
		/** ground units **/
		"widowmine",          // widowmine: long cooldowns

		// "siegetanksieged", // siegetank: immovable
							  // siegetank 33, siegetanksieged 32

		/*** air  units ***/
		"medivac",		      // medivac: spellcaster, shuttle
	},
	// #8 - Zerg, specially excluded.
	{
		/** ground units **/
		"lurkermpburrowed",   // lurker: immovable
							  // lurkermp 502
							  // lurkermpburrowed 503
							  // (unused) lurker 911
							  // (unused) lurkerburrowed 912

		"infestor",           // infestor: spellcaster
							  // infestor 111

		"swarmhostburrowedmp",// swarmhost: spellcaster
							  // swarmhostmp 494
							  // swarmhostburrowedmp 493

		/*** air  units ***/
		"overlord",	          // overlord: sight, shuttle
		"overseer",	          // detector
	},
	// #9 - Protoss, specially excluded.
	{
		/** ground units **/
		"disruptor",          // spellcaster

		/*** air  units ***/
		"oracle",             // spellcaster?
		"mothership",         // cloaker
		"warpprism",          // shuttle
		"observer",           // detector
	},
});

void dirichlet1(int k, std::vector<float>& ratios) {
	ratios.clear();
	ratios.reserve(k);
	std::gamma_distribution<float> distribution(1.0, 1.0);
	float sum = 0;
	for (int i = 0; i < k; i++) {
		float sample = distribution(generator);
		sum += sample;
		ratios.push_back(sample);
	}
	for (float& ratio : ratios) {
		ratio /= sum;
	}
}

// Beta(1, k)
// see (https://en.wikipedia.org/wiki/Dirichlet_distribution#Marginal_beta_distributions)
float beta1k(int k) {
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	float u = distribution(generator);
	return 1 - pow(1 - u, 1.0f / k);
}

void Combinator::reset(){
	set_resources(config.limit_ore, config.limit_gas, config.limit_food);
	if (config.index) {
		add_unitlist(config.index);
	}
	else {
		add_unitlist(config.unitlist);
	}
}

void Combinator::set_unitdata(const sc2::ObservationInterface* observation) {
	unitdata = observation->GetUnitTypeData();
}

void Combinator::set_unitdata(const sc2::UnitTypes& unitdata) {
	this->unitdata = unitdata;
}

void Combinator::set_config(const CombinatorConfig& config) {
	this->config = config;
	use_config = true;
}

void Combinator::clear_resources() {
	this->mineral = 0;
	this->gas = 0;
	this->food = 0;
}

void Combinator::set_resources(int mineral, int gas, float food) {
	this->mineral = mineral;
	this->gas = gas;
	this->food = food;
}

void Combinator::add_resources(int mineral, int gas, float food) {
	this->mineral += mineral;
	this->gas += gas;
	this->food += food;
}

std::tuple<int, int, float> Combinator::get_resources() const {
	return std::make_tuple(mineral, gas, food);
}

void Combinator::clear_unitlist(){
	this->candidates.clear();
}

void Combinator::add_unitlist(int index){
	// bound check
	const size_t len = predefined_cand.size();
	
	if (0 >= index || index >= len) {
		std::cout << "Index out of bound: " << index << " max: " << len << std::endl;
		exit(1);
	}
	const std::vector<std::string>& p = predefined_cand[index];
	for (const auto& unitname : p) {
		sc2::UnitTypeID id = NameToUnitTypeID(unitname);
		this->candidates.push_back(id);
	}
}

void Combinator::add_unitlist(const std::string& unitname) {
	sc2::UnitTypeID id = NameToUnitTypeID(unitname);
	this->candidates.push_back(id);
}

void Combinator::add_unitlist(const std::vector<sc2::UnitTypeID>& new_candidates){
	this->candidates.insert(this->candidates.end(),
	                        new_candidates.begin(),
						    new_candidates.end());
}

std::vector<sc2::UnitTypeID> Combinator::get_unitlist() const{
	return candidates;
}

bool Combinator::pick_and_rearrange_candidates(float probability){
	if (probability <= 0.0f) {
		std::cerr << "Fatal error : p = 0!" << std::endl;
		exit(1);
	}
	if (this->candidates.empty()) {
		std::cerr << "Fatal error : no candidates!" << std::endl;
		exit(1);
	}
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	// pick each element with probability
	for (const auto& id : this->candidates) {
		if (distribution(generator) <= probability) {
			squad_unittypeid.push_back(id);
		}
	}
	// shuffle
	std::shuffle(squad_unittypeid.begin(), squad_unittypeid.end(), device);
	return true;
}

bool Combinator::make_squad(){
	bool success = false;
	int32_t variables = candidates.size();

	squad_unittypeid.clear();
	squad_quantity.clear();

	std::vector<float> ratios;
	dirichlet1(variables, ratios);
	for (const auto& unittypeid : candidates) {
		float num_affordable = get_unit_affordable(unittypeid);
		float ratio = (variables == 1) ? 1.0f : beta1k(variables - 1);
		int32_t num_bought = static_cast<int32_t>(std::floor(num_affordable * ratio));
		subtract_unit_resource(unittypeid, num_bought);
		variables--;
		// write
		if (num_bought) {
			squad_unittypeid.push_back(unittypeid);
			squad_quantity.push_back(num_bought);
			std::cerr << UnitTypeToName(unittypeid.ToType()) << " " << num_bought << std::endl;
			success = true;
		}
	}
	std::cerr << "mineral " << mineral << " gas " << gas << " food " << food << std::endl;
	return success;
}

bool Combinator::make_squad_simultaneous() {
	bool success = false;
	int32_t variables = candidates.size();

	squad_unittypeid.clear();
	squad_quantity.clear();

	std::vector<float> ratios;
	const int32_t lmineral = mineral;
	const int32_t lgas = gas;
	const int32_t lfood = food;
	dirichlet1(variables, ratios);
	for (const auto& unittypeid : candidates) {
		float num_affordable = get_unit_affordable(unittypeid, lmineral, lgas, lfood);
		float ratio = ratios.back();
		int32_t num_bought = static_cast<int32_t>(std::round(num_affordable * ratio));
		subtract_unit_resource(unittypeid, num_bought);
		ratios.pop_back();
		// write
		if (num_bought) {
			squad_unittypeid.push_back(unittypeid);
			squad_quantity.push_back(num_bought);
			std::cerr << UnitTypeToName(unittypeid.ToType()) << " " << num_bought << std::endl;
			success = true;
		}
	}
	std::cerr << "mineral " << mineral << " gas " << gas << " food " << food << std::endl;
	return success;
}

// get max number of the given unit that we can buy with the given resource.
float Combinator::get_unit_affordable(sc2::UnitTypeID unittypeid, int32_t mineral, int32_t gas, float food) {
	const sc2::UnitTypeData unittypedata = unitdata[unittypeid];

	// cost
	float c_m = static_cast<float>(unittypedata.mineral_cost);
	float c_g = static_cast<float>(unittypedata.vespene_cost);
	float c_f = static_cast<float>(unittypedata.food_required);	// due to zergling

	// max number: use floor function.
	float n_m = c_m ? static_cast<float>(mineral / c_m) : std::numeric_limits<float>::max();
	float n_g = c_g ? static_cast<float>(gas / c_g) : std::numeric_limits<float>::max();
	float n_f = c_f ? static_cast<float>(food / c_f) : std::numeric_limits<float>::max();
	//std::cerr << c_m << " " << c_g << " " << c_f << std::endl;
	//std::cerr << n_m << " " << n_g << " " << n_f << std::endl;
	float n_min = std::min(std::min(n_m, n_g), n_f);

	return std::max(n_min, 0.0f);
}

std::tuple< std::vector<sc2::UnitTypeID>, std::vector<int> > Combinator::get_squad() const {
	return std::make_tuple(squad_unittypeid, squad_quantity);
};

// get max number of the given unit that we can buy with the given resource.
float Combinator::get_unit_affordable(sc2::UnitTypeID unittypeid) {
	const sc2::UnitTypeData unittypedata = unitdata[unittypeid];

	// cost
	float c_m = static_cast<float>(unittypedata.mineral_cost);
	float c_g = static_cast<float>(unittypedata.vespene_cost);
	float c_f = static_cast<float>(unittypedata.food_required);

	// max number: use floor function.
	float n_m = c_m ? static_cast<float>(mineral / c_m) : std::numeric_limits<float>::max();
	float n_g = c_g ? static_cast<float>(gas / c_g) : std::numeric_limits<float>::max();
	float n_f = c_f ? static_cast<float>(food / c_f) : std::numeric_limits<float>::max();
	float n_min = std::min(std::min(n_m, n_g), n_f);
	return std::max(n_min, 0.0f);
}

// subtract resource with the amount of the cost of the unit.
void Combinator::subtract_unit_resource(sc2::UnitTypeID unittypeid, int32_t num_units) {
	const sc2::UnitTypeData unittypedata = unitdata[unittypeid];

	mineral -= static_cast<int32_t>(unittypedata.mineral_cost * num_units);
	gas -= static_cast<int32_t>(unittypedata.vespene_cost * num_units);
	food -= static_cast<float>(unittypedata.food_required * num_units);	// zergling
}

sc2::UnitTypeID Combinator::NameToUnitTypeID(const std::string& name) const {
	std::string lname;
	lname.resize(name.size());
	std::transform(name.begin(), name.end(), lname.begin(), ::tolower);

	std::string ldname;
	for (const sc2::UnitTypeData& data : unitdata)
	{
		ldname.resize(data.name.size());
		std::transform(data.name.begin(), data.name.end(), ldname.begin(), ::tolower);
		if (lname == ldname)
		{
			// std::cerr << "unit name " << name << " is found: " << data.unit_type_id << std::endl;
			return data.unit_type_id;
		}
	}
	std::cerr << "Fatal Error: unit name " << name << " is not found!" << std::endl;
	exit(-1);
	return 0;
}

void Combinator::load_predefined_squad(
	const std::vector<sc2::UnitTypeID>& squad_unittypeid,
	const std::vector<int>& squad_quantity
) {
	this->squad_unittypeid = squad_unittypeid;
	this->squad_quantity = squad_quantity;
}
