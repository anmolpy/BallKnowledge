#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <sstream>
#include <chrono>
#include "apiClient.h"

/// use this fiile when fetching data from api and writing to file. remove from cmake file when done to avoid unnecessary api calls and rate limit issues.

std::string date_today() {
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
        ApiClient api("773fad6d52334e4887abc932c8b10f28");
        std::string today = date_today();
        std::string endpoint = "/v4/competitions/WC/matches?dateFrom=2026-07-01&dateTo=" + today;
        using json = nlohmann::json;
        std::string response = api.Get(endpoint);

        json j = json::parse(response);

        // Open the file and write the pretty-printed JSON (4 spaces indent)
        std::ofstream outfile("D:/Code_projects/BallKnowledge/src/fixtures.json");
        if (outfile.is_open())
        {
            outfile << j.dump(4);
            outfile.close();
            std::cout << "Successfully wrote output to src/fixtures.txt" << std::endl;
        }
        else
        {
            std::cerr << "Failed to open output file." << std::endl;
        }

       

    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
}