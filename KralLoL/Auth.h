#pragma once

#include <string>
#include "base64.h"

class Auth
{
public:
	int riotPort = 0;
	std::string riotToken;
	std::string riotHeader;

	int leaguePort = 0;
	std::string leagueToken;
	std::string leagueHeader;

	Base64 base64;

	// baþarýyla baðlantý noktasý ve belirteç alýndýðýnda 1 döndürür
	bool GetRiotClientInfo();

	void MakeRiotHeader();

	// baþarýyla baðlantý noktasý ve belirteç alýndýðýnda 1 döndürür
	bool GetLeagueClientInfo();

	void MakeLeagueHeader();
private:
	std::wstring GetProcessCommandLine(std::string sProcessName);
};

extern Auth* auth;
