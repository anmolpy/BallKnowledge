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
#include <conio.h>

int printMatchInfo(const MatchInfo& matchInfo, int& prev_home, int& prev_away) {
	std::string status = matchInfo.status;
    std::string home = matchInfo.homeTeam;
    std::string away = matchInfo.awayTeam;

    // printing match status
    if (status == "SCHEDULED") {
        std::cout << home << " vs. " << away << " has not started yet." << std::endl << std::endl;
        return 0;
    }
    if (status == "POSTPONED") {
        std::cout << home << " vs. " << away << " has been postponed." << std::endl << std::endl;
        return 0;
    }
    if (status == "CANCELED") {
        std::cout << home << " vs. " << away << " has been canceled." << std::endl << std::endl;
        return 0;
    }
    if (status == "SUSPENDED") {
        std::cout << home << " vs. " << away << " has been suspended." << std::endl << std::endl;
        return 0;
    }
    if (status == "TIMED") {
        std::cout << home << " vs. " << away << " has been timed." <<  << std::endl << std::endl;
        return 0;
    }
    if (status == "AWARDED") {
        std::cout << home << " vs. " << away << " has been awarded." << std::endl << std::endl;
        return 0;
    }

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

    if (homeScore != prev_home) {
        if (homeScore > prev_home) {
            notify();
            std::cout << home << " scored!" << std::endl;
        }

        prev_home = homeScore;
    }

    if (awayScore != prev_away) {
        if (awayScore > prev_away) {
            notify();
            std::cout << away << " scored!" << std::endl;
        }
        prev_away = awayScore;
    }
    std::cout << std::endl;
    return 0; 
    
}

void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

//nlohmann::json readJsonFromFile(const std::string& filePath) {
//    std::ifstream infile(filePath);
//    if (!infile.is_open()) {
//        throw std::runtime_error("Failed to open input file.");
//    }
//    std::string jsonString((std::istreambuf_iterator<char>(infile)),
//                           std::istreambuf_iterator<char>());
//    infile.close();
//    return nlohmann::json::parse(jsonString);
//}

static std::vector<int> showMatches(nlohmann::json response) {
    std::vector<int> matchNumber;
    int bullet = 1;
	int totalMatches = response["matches"].size();
	int trackedMatches = 0;
    std::cout << "Matches today (UTC-Time): " << std::endl;
    for (const auto& match : response["matches"]) {
        std::string home = match["homeTeam"]["name"];
        std::string away = match["awayTeam"]["name"];
        std::cout << bullet << ". " << home << " vs " << away << std::endl;
        bullet++;
    }

    // Loop until valid input is received
    bool addMatchNumber = true;
    while (true) {
        std::cout << "Enter the Match number you want to track: ";
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
		trackedMatches++;
        if(trackedMatches >= totalMatches){
            break;
		}

		// Ask if the user wants to add another match number
        std::cout << "Track another match? (y/n): ";
        char choice = _getch();
        std::cout << std::endl;
        if (choice == 'y' || choice == 'Y') {
            continue; // Continue the loop to add another match number
        }
        else if (choice == 'n' || choice == 'N') {
            break; // Exit the loop if the user doesn't want to add more match numbers
        }
        else break;
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


    try
    {
        // use when fetching data from api
        ApiClient api("773fad6d52334e4887abc932c8b10f28");
		std::string today = date_today();
        std::string endpoint = "/v4/competitions/WC/matches?dateFrom=" + today + "&dateTo=" + today;
        std::string response = api.Get(endpoint);
        using json = nlohmann::json;
        json initialj = json::parse(response);

		//use when fetching data from local file
        //nlohmann::json initialj = readJsonFromFile("D:/Code_projects/BallKnowledge/src/fixtures.json");

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

		int trackedMatchNumber;

        bool atLeastOneMatchisLive = false;
        
        while (true) {
            
			//use when fetching data from api
			response = api.Get(endpoint);
			json j = json::parse(response);

			//use when fetching data from local file
            //nlohmann::json j = readJsonFromFile("D:/Code_projects/BallKnowledge/src/fixtures.json");

            for (int i = 0; i < matches_to_track; i++) {
				std::cout << "Match " << i + 1 << ":" << std::endl;
                trackedMatchNumber = trackedMatchNumbers[i];

                MatchInfo MatchInfo = monitor(trackedMatchNumber, j);
				printMatchInfo(MatchInfo, prev_home[i], prev_away[i]);

                if (MatchInfo.islive == true) {
					atLeastOneMatchisLive = true;
                }                
            }
            if (!atLeastOneMatchisLive) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(6));
            clearConsole();
        }
		std::cout << "Press any key to exit...";
        char ch = _getch();
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
}