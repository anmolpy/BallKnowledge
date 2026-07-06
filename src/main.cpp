#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include "apiClient.h"
#include "matchMonitor.h"
#include "Notification.h"
#include <chrono>
#include <thread>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <vector>

int printMatchInfo(const MatchInfo& matchInfo) {
	std::string status = matchInfo.status;

    // printing match status
    if (status == "SCHEDULED") {
        std::cout << "Match has not started yet." << std::endl;
        return 0;
    }
    if (status == "POSTPONED") {
        std::cout << "Match has been postponed." << std::endl;
        return 0;
    }
    if (status == "CANCELED") {
        std::cout << "Match has been canceled." << std::endl;
        return 0;
    }
    if (status == "SUSPENDED") {
        std::cout << "Match has been suspended." << std::endl;
        return 0;
    }
    if (status == "TIMED") {
        std::cout << "Match has been timed." << std::endl;
        return 0;
    }
    if (status == "AWARDED") {
        std::cout << "Match has been awarded." << std::endl;
        return 0;
    }


    std::string home = matchInfo.homeTeam;
    std::string away = matchInfo.awayTeam;
    int homeScore = matchInfo.homeScore;
    int awayScore = matchInfo.awayScore;
    int homePenalties = matchInfo.homePenalties;
    int awayPenalties = matchInfo.awayPenalties;
	bool isPenaltyShootout = matchInfo.isPenaltyShootout;

    //printing current score
    std::cout << home << "\t\t\t" << homeScore << " - " << awayScore << "\t\t\t" << away << std::endl;

    if (isPenaltyShootout) {
        std::cout << "Match went to penalty shootout." << std::endl;
        std::cout << "Penalty Shootout Score: " << std::endl;
        std::cout << home << "\t\t\t" << homePenalties << " - " << awayPenalties << "\t\t\t" << away << std::endl;
    }

    if (status == "FINISHED") {
        std::string winner = matchInfo.winner;
        if (winner == "DRAW") {
            std::cout << "Match ended in a draw." << std::endl;
        }
        else {
            std::cout << "Winner: " << winner << std::endl;
        }
	}


    


}
void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

nlohmann::json readJsonFromFile(const std::string& filePath) {
    std::ifstream infile(filePath);
    if (!infile.is_open()) {
        throw std::runtime_error("Failed to open input file.");
    }
    std::string jsonString((std::istreambuf_iterator<char>(infile)),
                           std::istreambuf_iterator<char>());
    infile.close();
    return nlohmann::json::parse(jsonString);
}

static std::vector<int> showMatches(nlohmann::json response) {
    std::vector<int> matchNumber;
    int bullet = 1;

    std::cout << "Matches today: " << std::endl;
    for (const auto& match : response["matches"]) {
        std::string home = match["homeTeam"]["name"];
        std::string away = match["awayTeam"]["name"];
        std::cout << bullet << ". " << home << " vs " << away << std::endl;
        bullet++;
    }

    // Loop until valid input is received
    bool addMatchNumber = true;
    bool invalidInput = true; // Flag to track invalid input. setting it to true initially to enter the loop
    while (true) {
        std::cout << "Enter the Match number you want to track: " << std::endl;
        int n;
        std::cin >> n;

        if (std::cin.fail())
        {
            std::cin.clear();                                          // reset error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard bad input
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (n < 1 || n > response["matches"].size()) {
            std::cout << "Invalid match number. Please enter a number between 1 and " << response["matches"].size() << "." << std::endl;
            continue;
        }

		matchNumber.push_back(n);

		// Ask if the user wants to add another match number
        std::cout << "Do you want to add another match number? (y/n): ";
        char choice;
        std::cin >> choice;
        if (choice == 'n' || choice == 'N') {
            break; // Exit the loop if the user doesn't want to add more match numbers
		}
        else if (choice == 'y' || choice == 'Y') {
			continue; // Continue the loop to add another match number
		}
    }
    
    return matchNumber;
}

std::string date_today(){
    using namespace std::chrono;
    auto now = system_clock::now();
    auto today = floor<days>(now); // truncate to midnight
	auto tomorrow = today + days{ 1 };
    year_month_day ymd{ today };


    std::ostringstream oss;
    oss << std::setfill('0')
        << static_cast<int>(ymd.year()) << "-"
        << std::setw(2) << static_cast<unsigned>(ymd.month()) << "-"
        << std::setw(2) << static_cast<unsigned>(ymd.day());

    return oss.str();
}


int main()
{
// use when fetching data from api

    try
    {
        /*ApiClient api("773fad6d52334e4887abc932c8b10f28");
		std::string today = date_today();
        std::string endpoint = "/v4/competitions/WC/matches?dateFrom=" + today + "&dateTo=" + today;
        std::string response = api.Get(endpoint);
        using json = nlohmann::json;
        json initialj = json::parse(response);*/

        nlohmann::json initialj = readJsonFromFile("D:/Code_projects/BallKnowledge/src/fixtures.json");

        std::vector<int> prev_home;
        std::vector<int> prev_away;
        std::vector<int> trackedMatchNumbers = showMatches(initialj);

		int matches_to_track = trackedMatchNumbers.size();

        for (int i = 0; i < matches_to_track; i++) {         
            MatchInfo matchInfo = monitor(trackedMatchNumbers[i], initialj);
			clearConsole();
			prev_away.push_back(matchInfo.awayScore);
			prev_home.push_back(matchInfo.homeScore);
		}
        std::cout << "You are tracking " << matches_to_track << " matches." << std::endl;

		int trackedMatchNumber;

        bool atLeastOneMatchisLive = false;
        
        while (true) {
            

			//response = api.Get(endpoint);
			//json j = json::parse(response);

            nlohmann::json j = readJsonFromFile("D:/Code_projects/BallKnowledge/src/fixtures.json");

            for (int i = 0; i < matches_to_track; i++) {
                trackedMatchNumber = trackedMatchNumbers[i];

                MatchInfo MatchInfo = monitor(trackedMatchNumber, j);
				printMatchInfo(MatchInfo);

                if (MatchInfo.islive == true) {
					atLeastOneMatchisLive = true;
                }

                if (MatchInfo.homeScore != prev_home[i]) {
                    if (MatchInfo.homeScore > prev_home[i]) {
                        notify();
                        std::cout << MatchInfo.homeTeam << " scored!" << std::endl;
                    }

                    prev_home[i] = MatchInfo.homeScore;
                }

                if (MatchInfo.awayScore != prev_away[i]) {
                    if (MatchInfo.awayScore > prev_away[i]) {
                        notify();
                        std::cout << MatchInfo.awayTeam << " scored!" << std::endl;
                    }
                    prev_away[i] = MatchInfo.awayScore;
                }


            }
            if (!atLeastOneMatchisLive) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(6));
            clearConsole();
        }

    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
}