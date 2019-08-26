#pragma once

#include <sc2api/sc2_api.h>

class Camerabot : public sc2::Agent {
private:

	std::string botname;
	sc2::Point2D centerpos;
	sc2::Point2D campos;
	sc2::Point2D camoffset;

public:

	Camerabot(std::string botname, sc2::Point2D camoffset = sc2::Point2D(0, 0)) 
		: botname(botname), camoffset(camoffset) {}

	void OnGameStart() final override;
	void OnStep() final override;
};

class Simbot : public sc2::Agent {
private:
	
	std::string botname;
	sc2::Point2D centerpos;
	sc2::Point2D campos;
	sc2::Point2D camoffset;

public:

	Simbot(std::string botname, sc2::Point2D camoffset= sc2::Point2D(0,0) ) 
		: botname(botname), camoffset(camoffset) {}

	void OnGameStart() final override;
	void OnUnitIdle(const sc2::Unit* unit) final override;
	void OnStep() final override;
};
