#pragma once

#include "combinator.h"
#include "configurator.h"
#include "randomsample.h"

#include "json/json-forwards.h"

class Player {
private:
	sc2::Agent* agent_;
	uint32_t playerID;
	bool default_playerID;
	std::string playername;
	sc2::Vector2D offset;
	sc2::Point2D genpos;
	sc2::Point2D centerpos;
	PlayerConfig config;
	Combinator combinator_;
	RandomSample random;

public:

	// This agent must see all of #playerid's units
	void setBot(sc2::Agent* agent);
	void setBot(sc2::Agent* agent, int32_t playerID);

	void setConfig(const PlayerConfig& config);

	// get agent
	sc2::Agent* const Bot() const;

	Combinator& combinator();
	const Combinator& combinator() const;

	void GameInit();

	// Count number of units with 
	size_t CountPlayerUnit(sc2::UnitTypeID unit_type, uint32_t playerID) const;

	size_t CountPlayerUnit(uint32_t playerID) const;

	size_t CountPlayerUnit() const;

	std::tuple< std::vector<sc2::UnitTypeID>, std::vector<int> > GetPlacedUnit() const;

	// Get list of units (seen) owned by player
	std::tuple< std::vector<sc2::UnitTypeID>, std::vector<int> > GetPlacedUnit(uint32_t playerID) const;

	// Put unit owned by player (need to send debug)
	void DeployUnit(sc2::UnitTypeID unit, uint32_t numbers, sc2::Vector2D pos, uint32_t playerID);

	// Put list of units owned by player (need to send debug)
	void DeployUnit(
		const std::vector<sc2::UnitTypeID>& squad_unittypeid,
		const std::vector<int>& squad_quantity, 
		sc2::Vector2D pos,
		uint32_t playerID,
		bool shuffle=true
	);

	// Put list of units as predefined way.
	void DeployUnit(bool shuffle=true);

	// Kill all (seen) units owned by player (need to send debug)
	void KillPlayerUnit(uint32_t playerID);

	void KillPlayerUnit();

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
