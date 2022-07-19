#include "player.h"

#include "util.h"

#include <iostream>
#include <vector>


void Player::SetBot(sc2::Agent* agent, playerid_t playerID) {
	_agent = agent;
	_playerID = playerID;
}

void Player::SetConfig(const PlayerConfig& config) {
	_config = config;
}

// get agent
sc2::Agent* const Player::Bot() const {
	return _agent;
}

Combinator& Player::combinator() {
	return _combinator;
}

const Combinator& Player::combinator() const {
	return _combinator;
}

void Player::GameInit() {
	const sc2::GameInfo game_info = Bot()->Observation()->GetGameInfo();
	_centerpos = (game_info.playable_max + game_info.playable_min) / 2.0f;
	_genpos = _offset + _centerpos;
	_combinator.set_unitdata(Bot()->Observation());

	if (_playerID<0) {
		_playerID = Bot()->Observation()->GetPlayerID();
	}
}

size_t Player::CountPlayerUnit(sc2::UnitTypeID unit_type, playerid_t playerID) const {
	if (playerID < 0) {
		playerID = _playerID;
	}
	return Bot()->Observation()->GetUnits(
		[unit_type, playerID](const sc2::Unit& unit) {
			return unit.owner == playerID && Util::IsUnit(unit_type)(unit); }
	).size();
}

size_t Player::CountPlayerUnit(playerid_t playerID) const {
	if (playerID < 0) {
		playerID = _playerID;
	}
	return Bot()->Observation()->GetUnits(
		[playerID](const sc2::Unit& unit) {return unit.owner == playerID; }
	).size();
}

std::tuple< std::vector<sc2::UnitTypeID>, std::vector<int> >
Player::GetSurvivedUnits(playerid_t playerID) const {
	if (playerID < 0) {
		playerID = _playerID;
	}
	sc2::Units myunits = Bot()->Observation()->GetUnits(
		[playerID](const sc2::Unit& unit) {return unit.owner == playerID; });
	// make a map
	std::map<sc2::UnitTypeID, int> unit_map;
	for (const auto& u : myunits) {
		sc2::UnitTypeID id = u->unit_type;
		if (!unit_map.count(id)) {
			unit_map[id] = 0;
		}
		unit_map[id] += 1;
	}
	size_t length = unit_map.size();
	std::vector<sc2::UnitTypeID> squad_unittypeid;
	std::vector<int> squad_quantity;
	squad_unittypeid.reserve(length);
	squad_quantity.reserve(length);
	for (const auto& e : unit_map) {
		squad_unittypeid.push_back(e.first);
		squad_quantity.push_back(e.second);
	}
	return std::make_tuple(squad_unittypeid, squad_quantity);
}

void Player::PlaceUnit(sc2::UnitTypeID unit, uint32_t numbers, sc2::Vector2D pos, playerid_t playerID) {
	sc2::DebugInterface* debug = Bot()->Debug();
	debug->DebugCreateUnit(unit, pos, playerID, numbers);
	//debug->SendDebug();
}

std::vector<int> Player::PlaceUnits(
	const std::vector<sc2::UnitTypeID>& squad_unittypeid,
	const std::vector<int>& squad_quantity,
	sc2::Vector2D pos,
	playerid_t playerID,
	bool shuffle
) {
	sc2::DebugInterface* debug = Bot()->Debug();
	size_t length = squad_unittypeid.size();
	assert(length == squad_quantity.size());
	std::vector<int> random_index(length, 0);
	for (int i = 0; i < length; i++) {
		random_index[i] = i;
	}
	if (shuffle) {
		_random.shuffle(random_index.begin(), random_index.end());
	}
	for (int i = 0; i < length; i++) {
		int index = random_index[i];
		int quantity = squad_quantity[index];
		if (!quantity) continue;
		sc2::UnitTypeID unittypeID = squad_unittypeid[index];
		debug->DebugCreateUnit(unittypeID, pos, playerID, quantity);
	}
	//debug->SendDebug();
	return random_index;
}

std::vector<int> Player::PlaceUnits(
	const std::vector<sc2::UnitTypeID>& squad_unittypeid,
	const std::vector<int>& squad_quantity,
	bool shuffle) {
	return PlaceUnits(squad_unittypeid, squad_quantity, _centerpos + _config.offset, _playerID, shuffle);
}

void Player::KillPlayerUnit(playerid_t playerID) {
	if (playerID < 0) {
		playerID = _playerID;
	}
	sc2::DebugInterface* debug = Bot()->Debug();
	const sc2::ObservationInterface* observation = Bot()->Observation();
	sc2::Units units = observation->GetUnits(
		[playerID](const sc2::Unit& unit) {return unit.owner == playerID; });
	for (const auto& unit : units) {
		debug->DebugKillUnit(unit);
	}
	//debug->SendDebug();
}

void Player::KillUnit(sc2::Filter f) {
	sc2::DebugInterface* debug = Bot()->Debug();
	const sc2::ObservationInterface* observation = Bot()->Observation();
	sc2::Units units = observation->GetUnits(f);
	for (const auto& unit : units) {
		debug->DebugKillUnit(unit);
	}
	//debug->SendDebug();
}

void Player::ShowMap() {
	sc2::DebugInterface* debug = Bot()->Debug();
	debug->DebugShowMap();
	//debug->SendDebug();
}

void Player::SendDebug() {
	sc2::DebugInterface* debug = Bot()->Debug();
	debug->SendDebug();
}

void Player::LeaveGame() {
	//DebugInterface* debug = Debug();
	//debug->DebugEndGame();
	//debug->SendDebug();
	Bot()->Control()->RequestLeaveGame();
}

void Player::MoveCamera() {
	const sc2::ObservationInterface* observation = Bot()->Observation();
	playerid_t playerID = _playerID;

	sc2::Units myunits = observation->GetUnits(
		[playerID](const sc2::Unit& unit) {return unit.owner == playerID; });
	sc2::Point2D mycenter = _centerpos + _config.offset;
	if (myunits.size()) {
		for (const auto& unit : myunits) {
			mycenter += sc2::Point2D(unit->pos);
		}
		mycenter /= static_cast<float>(myunits.size());
	}

	Bot()->Debug()->DebugMoveCamera(mycenter);
	Bot()->Debug()->SendDebug();
}

void Player::ScreenCapture(const std::string& filename) {
	const SC2APIProtocol::Observation* observation = Bot()->Observation()->GetRawObservation();
	const SC2APIProtocol::ObservationRender& render = observation->render_data();

	//const SC2APIProtocol::ImageData& minimapraw = render.minimap();
	const SC2APIProtocol::ImageData& mapraw = render.map();

	//BRG, 24bit(3 channels)
	std::string map = mapraw.data();
	//std::string minimap = minimapraw.data();

	int32_t width = mapraw.size().x();
	int32_t height = mapraw.size().y();
	int32_t length = map.length();
	
	if (length == 0) {
		std::cerr << "Image size is 0. Check if rendering is enabled!" << std::endl;
	}
	if (length != width * height * 3) {
		std::cerr << "Image should be 24bpp!" << std::endl;
	}

	Util::save_BGR_image(filename, map.data(), width, height);
}
