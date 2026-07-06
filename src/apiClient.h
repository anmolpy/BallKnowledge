#pragma once

#include <string>

class ApiClient
{
public:
	ApiClient(const std::string& apiKey);
	std::string Get(const std::string& endpoint);

private:
	std::string m_apiKey;
	const std::string BASE_URL = "https://api.football-data.org";
};