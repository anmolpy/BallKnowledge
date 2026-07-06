#pragma once
#include <nlohmann/json.hpp>

struct MatchInfo {
	std::string status;
	int homeScore;
	int awayScore;
	std::string homeTeam;
	std::string awayTeam;
	bool islive;
	bool isPenaltyShootout = false;
	int homePenalties;
	int awayPenalties;
	std::string winner;
};

MatchInfo monitor(int gameid, nlohmann::json response);