#include <iostream>
#include <random>
#include <cmath>

#include "json/json.h"

#include "combinator.h"

std::random_device device;
std::mt19937_64 generator(device());

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

float beta1k(int k) {
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	float u = distribution(generator);
	return 1 - pow(1 - u, 1.0f / k);
}

void Combinator::SetUnitData(const ObservationInterface* observation) {
	unitdata = observation->GetUnitTypeData();
}

// get list of units and pick with inclusion probability p.
int32_t Combinator::get_squad_permutation(Race race, std::vector<UnitTypeID>& unit_permutations, float probability) {
	//std::cout << "hello i am d_get_squad_permutation" << std::endl;
	std::vector<UnitTypeID> candidate_units;
	const std::vector<std::string>* candidate_units_name;
	const static std::vector<std::string> unit_T({
			"scv",			// 45
			"marine",		// 48
			"marauder",		// 51
			"reaper",		// 49
			"ghost",		// 50
			"hellion",		// 53
			"helliontank",	// 484
			//"widowmine",
			"siegetank",	//33, sieged 32
			"cyclone",		//692 /
			"thor",			//52, ap 691
			"vikingassault"		//assault 34, fighter 35

			/*** air **
			medivac
			liberator		//689, ag 734
			banshee
			raven
			battlecruiser
			*/
		});
	const static std::vector<std::string> unit_Z({
			"drone",		// 104
			"queen",		// 126
			"zergling",		// 105
			"baneling",		// 9
			"roach",		// 110
			"ravager",		// 688
			"hydralisk",	// 107
			//"lurkermpburrowed",		// 911, burrowed 912
							// mp 502, mpburrowed 503
			// "infestor",		// 111
			//"swarmhostburrowedmp",	//mp 494, burrowedmp 493
			"ultralisk",	// 109

			/*** air **
			mutalisk
			corruptor
			viper
			broodlord
			overlord
			overseer
			*/
		});
	const static std::vector<std::string> unit_P({
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

			/*** air **
			//"observer",
			//"warpprism",
			phoenix
			voidray
			oracle
			tempest
			carrier
			mothership
			*/
		});
	// default = Random
	switch (GetRandomInteger(1, 3)) {
	case 1:
		candidate_units_name = &unit_T;
		break;
	case 2:
		candidate_units_name = &unit_Z;
		break;
	case 3:
	default:
		candidate_units_name = &unit_P;
		break;
	}
	switch (race) {
	case Terran:
		candidate_units_name = &unit_T;
		break;
	case Zerg:
		candidate_units_name = &unit_Z;
		break;
	case Protoss:
		candidate_units_name = &unit_P;
		break;
	default:
		// default = Random
		break;
	}
	for (const auto& unitname : *candidate_units_name) {
		UnitTypeID id = NameToUnitTypeID(unitname);
		candidate_units.push_back(id);
	}
	return get_squad_permutation(candidate_units, unit_permutations, probability);
}

UnitTypeID Combinator::NameToUnitTypeID(std::string name) {
	//std::cout << "hello i am NameToUnitTypeID" << std::endl;
	std::string lname;
	lname.resize(name.size());
	std::transform(name.begin(), name.end(), lname.begin(), ::tolower);

	std::string ldname;
	for (const UnitTypeData & data : unitdata)
	{
		ldname.resize(data.name.size());
		std::transform(data.name.begin(), data.name.end(), ldname.begin(), ::tolower);
		if (lname == ldname)
		{
			std::cout << "unit name " << name << " is found: " << data.unit_type_id << std::endl;
			return data.unit_type_id;
		}
	}
	std::cout << "unit name " << name << " not found!" << std::endl;
	return 0;
}

// and pick units with inclusion probability p and rearrange orders.
int32_t Combinator::get_squad_permutation(const std::vector<UnitTypeID>& candidate_units, std::vector<UnitTypeID>& unit_permutations, float probability) {
	unit_permutations.clear();
	if (probability <= 0.0f) {
		std::cout << "Fatal error : p = 0!" << std::endl;
		exit(1);
	}
	if (candidate_units.empty()) {
		std::cout << "Fatal error : no candidates!" << std::endl;
		exit(1);
	}
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	// pick each element with probability
	for (const auto& id : candidate_units) {
		if (distribution(generator) <= probability) {
			unit_permutations.push_back(id);
		}
	}
	// shuffle
	std::random_shuffle(unit_permutations.begin(), unit_permutations.end());
	return static_cast<int32_t>(unit_permutations.size());
}

// get max number of the given unit that we can buy with the given resource.
float Combinator::get_unit_affordable(UnitTypeID unittypeid, int32_t mineral, int32_t gas, int32_t food) {
	const UnitTypeData unittypedata = unitdata[unittypeid];

	// cost
	float c_m = static_cast<float>(unittypedata.mineral_cost);
	float c_g = static_cast<float>(unittypedata.vespene_cost);
	float c_f = static_cast<float>(unittypedata.food_required);	// due to zergling
	if (unittypeid == 1932) {
		c_m = 150;
		c_g = 75;
		c_f = 2;
	}
	// max number: use floor function.
	float n_m = c_m ? static_cast<float>(mineral / c_m) : std::numeric_limits<float>::max();
	float n_g = c_g ? static_cast<float>(gas / c_g) : std::numeric_limits<float>::max();
	float n_f = c_f ? static_cast<float>(food / c_f) : std::numeric_limits<float>::max();
	std::cout << c_m << " " << c_g << " " << c_f << std::endl;
	std::cout << n_m << " " << n_g << " " << n_f << std::endl;
	float n_min = std::min(std::min(n_m, n_g), n_f);

	return std::max(n_min, 0.0f);
}

// subtract resource with the amount of the cost of the unit.
void Combinator::subtract_unit_resource(UnitTypeID unittypeid, int32_t& mineral, int32_t& gas, int32_t& food, int32_t num_units) {
	//std::string unitname;
	//UnitTypeID unittypeid = NameToUnitTypeID(unitname);
	const UnitTypeData unittypedata = unitdata[unittypeid];

	mineral -= static_cast<int32_t>(unittypedata.mineral_cost * num_units);
	gas -= static_cast<int32_t>(unittypedata.vespene_cost * num_units);
	food -= static_cast<int32_t>(unittypedata.food_required * num_units);	// zergling
}

// buy some specific unit with some uniformly random number. 
int32_t Combinator::buy_unit_uniformly(UnitTypeID unittypeid, int32_t& mineral, int32_t& gas, int32_t& food) {
	//std::cout << "hello i am d_buy_unit_uniformly" << std::endl;
	float num_affordable = get_unit_affordable(unittypeid, mineral, gas, food);
	int32_t floor_affordable = static_cast<int32_t>(std::floor(num_affordable));
	if (floor_affordable) {
		std::uniform_int_distribution<int> distribution(1, floor_affordable);
		int32_t num_buy = distribution(generator);
		subtract_unit_resource(unittypeid, mineral, gas, food, num_buy);
		return num_buy;
	}
	else {
		return 0;
	}
}

// get the squad randomly (uniform-old).
bool Combinator::construct_squad_old(Race race, int32_t& mineral, int32_t& gas, int32_t& food, Json::Value& squad_json, float probability) {
	//std::cout << "hello i am d_construct_squad" << std::endl;
	squad_json.clear();
	std::vector<UnitTypeID> unit_permutations;
	bool success = false;
	// if failed 3 times in a row - do not try again
	for (int32_t trial = 0; !success && trial < 3; trial++) {
		get_squad_permutation(race, unit_permutations, probability);
		for (const auto& unittypeid : unit_permutations) {
			int32_t num_bought = buy_unit_uniformly(unittypeid, mineral, gas, food);
			// write
			if (num_bought) {
				Json::Value unit_pair;
				unit_pair["UnitTypeID"] = static_cast<int>(unittypeid);
				unit_pair["Quantity"] = static_cast<int>(num_bought);
				squad_json.append(unit_pair);
				success = true;
			}
		}
	}
	return success;
}

// get the squad randomly (dirichlet-simultaneous).
bool Combinator::construct_squad_d1(Race race, int32_t& mineral, int32_t& gas, int32_t& food, Json::Value& squad_json, float probability) {
	//std::cout << "hello i am d_construct_squad" << std::endl;
	squad_json.clear();
	std::vector<UnitTypeID> unit_permutations;
	bool success = false;
	int32_t variables = 0;
	while (!variables) {
		variables = get_squad_permutation(race, unit_permutations, probability);
	}
	std::vector<float> ratios;
	const int32_t lmineral = mineral;
	const int32_t lgas = gas;
	const int32_t lfood = food;
	dirichlet1(variables, ratios);
	for (const auto& unittypeid : unit_permutations) {
		float num_affordable = get_unit_affordable(unittypeid, lmineral, lgas, lfood);
		float ratio = ratios.back();
		//int32_t num_bought = static_cast<int32_t>(std::floor(num_affordable * ratio));
		int32_t num_bought = static_cast<int32_t>(std::round(num_affordable * ratio));
		subtract_unit_resource(unittypeid, mineral, gas, food, num_bought);
		ratios.pop_back();
		// write
		if (num_bought) {
			Json::Value unit_pair;
			unit_pair["UnitTypeID"] = static_cast<int>(unittypeid);
			unit_pair["Quantity"] = static_cast<int>(num_bought);
			squad_json.append(unit_pair);
			std::cout << UnitTypeToName(unittypeid.ToType()) << " " << num_bought << std::endl;
			success = true;
		}
	}
	std::cout << "mineral " << mineral << " gas " << gas << " food " << food << std::endl;
	return success;
}

// get the squad randomly (dirichlet-cutting rope).
bool Combinator::construct_squad(Race race, int32_t& mineral, int32_t& gas, int32_t& food, Json::Value& squad_json, float probability) {
	//std::cout << "hello i am d_construct_squad" << std::endl;
	squad_json.clear();
	std::vector<UnitTypeID> unit_permutations;
	bool success = false;
	int32_t variables = 0;
	while (!variables) {
		variables = get_squad_permutation(race, unit_permutations, probability);
	}
	std::vector<float> ratios;
	dirichlet1(variables, ratios);
	for (const auto& unittypeid : unit_permutations) {
		float num_affordable = get_unit_affordable(unittypeid, mineral, gas, food);
		float ratio = (variables == 1)? 1.0f : beta1k(variables - 1);
		int32_t num_bought = static_cast<int32_t>(std::floor(num_affordable * ratio));
		//int32_t num_bought = static_cast<int32_t>(std::round(num_affordable * ratio));
		subtract_unit_resource(unittypeid, mineral, gas, food, num_bought);
		variables--;
		// write
		if (num_bought) {
			Json::Value unit_pair;
			unit_pair["UnitTypeID"] = static_cast<int>(unittypeid);
			unit_pair["Quantity"] = static_cast<int>(num_bought);
			squad_json.append(unit_pair);
			std::cout << UnitTypeToName(unittypeid.ToType()) << " " << num_bought << std::endl;
			success = true;
		}
	}
	std::cout << "mineral " << mineral << " gas " << gas << " food " << food << std::endl;
	return success;
}
