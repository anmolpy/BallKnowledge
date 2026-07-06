#include <iostream>
#include <nlohmann/json.hpp>
#include "matchMonitor.h"


MatchInfo monitor(int gameid, nlohmann::json response) {
	

	MatchInfo result;
	auto& match = response["matches"][gameid - 1];
	std::string status = match["status"];
	result.islive = false; // default to false, will be set to true if match is live
	result.status = status;

	

	std::string home = match["homeTeam"]["name"];
	result.homeTeam = home;
	std::string away = match["awayTeam"]["name"];
	result.awayTeam = away;

	int homeScore = match["score"]["fullTime"]["home"].is_null() ? 0 : match["score"]["fullTime"]["home"].get<int>();
	result.homeScore = homeScore;
	int awayScore = match["score"]["fullTime"]["away"].is_null() ? 0 : match["score"]["fullTime"]["away"].get<int>();
	result.awayScore = awayScore;

	int homePenalties;
	int awayPenalties;
	if (match["score"]["duration"] == "PENALTY_SHOOTOUT") {
		result.isPenaltyShootout = true;
		result.homePenalties = match["score"]["penalties"]["home"].is_null() ? 0 : match["score"]["penalties"]["home"].get<int>();
		result.awayPenalties = match["score"]["penalties"]["away"].is_null() ? 0 : match["score"]["penalties"]["away"].get<int>();
		result.homeScore = match["score"]["regularTime"]["home"];
		result.awayScore = match["score"]["regularTime"]["away"];
	}

	

	
	
	if (status == "FINISHED") {
		std::string winner = match["score"]["winner"];
		if(winner == "HOME_TEAM") {
			result.winner = home;
		} else if(winner == "AWAY_TEAM") {
			result.winner = away;
		} else {
			result.winner = "DRAW";
		}
		return result;
	}

	

	if (status == "IN_PLAY" || status == "PAUSED" || status == "EXTRA_TIME" || status == "PENALTY_SHOOTOUT") {
		result.islive = true;
		return result;
	}

	return result;
}