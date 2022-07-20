#pragma once

#include "combinator.h"
#include "configurator.h"
#include "randomsample.h"

typedef int32_t playerid_t;

class Player {
private:
	sc2::Agent* _agent;
	playerid_t _playerID;
	std::string _playername;
	sc2::Vector2D _offset;
	sc2::Point2D _genpos;
	sc2::Point2D _centerpos;
	PlayerConfig _config;
	Combinator _combinator;
	RandomSample _random;

public:

	// This agent must see all of #playerid's units
	void SetBot(sc2::Agent* agent, playerid_t playerID=-1);

	void SetConfig(const PlayerConfig& config);

	// get agent
	sc2::Agent* const Bot() const;

	Combinator& combinator();
	const Combinator& combinator() const;

	void GameInit();

	// Count number of units with 
	size_t CountPlayerUnit(sc2::UnitTypeID unit_type, playerid_t playerID=-1) const;

	size_t CountPlayerUnit(playerid_t playerID=-1) const;

	// Get list of units (seen) owned by player
	std::tuple< std::vector<sc2::UnitTypeID>, std::vector<int> > GetSurvivedUnits(playerid_t playerID=-1) const;

	// Put unit owned by player (need to send debug)
	void PlaceUnit(sc2::UnitTypeID unit, uint32_t numbers, sc2::Vector2D pos, playerid_t playerID);

	// Put list of units owned by player (need to send debug)
	std::vector<int> PlaceUnits(
		const std::vector<sc2::UnitTypeID>& squad_unittypeid,
		const std::vector<int>& squad_quantity, 
		sc2::Vector2D pos,
		playerid_t playerID,
		bool shuffle=true
	);

	// Put list of units as predefined way.
	std::vector<int> PlaceUnits(
		const std::vector<sc2::UnitTypeID>& squad_unittypeid,
		const std::vector<int>& squad_quantity, 
		bool shuffle=true);

	// Kill all (seen) units owned by player (need to send debug)
	void KillPlayerUnit(playerid_t playerID=-1);

	// Kill all (seen) units captured by filter (need to send debug)
	void KillUnit(sc2::Filter f);

	// Show map (need to send debug)
	void ShowMap();

	// Send debug
	void SendDebug();

	// End game
	void LeaveGame();
	
	// Move Camera to Center of units
	void MoveCamera();

	// Rendering must be enabled!!!
	void ScreenCapture(const std::string& filename);
};
