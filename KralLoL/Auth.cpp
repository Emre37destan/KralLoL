#include "Auth.h"
#include "Utils.h"
#include "NtQueryInfoProc.h"

bool Auth::GetRiotClientInfo()
{
	std::string sAuth = utils->WstringToString(GetProcessCommandLine("RiotClientUx.exe"));
	if (sAuth.empty())
	{
		//MessageBoxA(0, "Istemci bulunamadi", 0, 0);
		return 0;
	}

	std::string appPort = "--app-port=";
	size_t nPos = sAuth.find(appPort);
	if (nPos != std::string::npos)
		riotPort = std::stoi(sAuth.substr(nPos + appPort.size(), 5)); // baðlantý noktasý her zaman 5 sayý uzunluðundadýr

	std::string remotingAuth = "--remoting-auth-token=";
	nPos = sAuth.find(remotingAuth) + strlen(remotingAuth.c_str());
	if (nPos != std::string::npos)
	{
		std::string token = "riot:" + sAuth.substr(nPos, 22); // belirteç her zaman 22 karakter uzunluðundadýr
		unsigned char m_Test[50]{};
		strncpy((char*)m_Test, token.c_str(), sizeof(m_Test));
		riotToken = base64.Encode(m_Test, token.size()).c_str();
	}
	else
	{
		MessageBoxA(0, "Istemciye baglanilamadi", 0, 0);

		return 0;
	}

	MakeRiotHeader();

	return 1;
}

void Auth::MakeRiotHeader()
{
	riotHeader = "Host: 127.0.0.1:" + std::to_string(riotPort) + "\r\n" +
		"Connection: keep-alive" + "\r\n" +
		"Authorization: Basic " + riotToken + "\r\n" +
		"Accept: application/json" + "\r\n" +
		"Access-Control-Allow-Credentials: true" + "\r\n" +
		"Access-Control-Allow-Origin: 127.0.0.1" + "\r\n" +
		"Content-Type: application/json" + "\r\n" +
		"Origin: https://127.0.0.1:" + std::to_string(riotPort) + "\r\n" +
		"User-Agent: Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) RiotClient/42.0.0 (CEF 74) Safari/537.36" + "\r\n" +
		"Referer: https://127.0.0.1:" + std::to_string(riotPort) + "/index.html" + "\r\n" +
		"Accept-Encoding: gzip, deflate, br" + "\r\n" +
		"Accept-Language: tr-TR,tr;q=0.8";
}

bool  Auth::GetLeagueClientInfo()
{
	// Ýstemci baðlantý noktasýný ve yetkilendirme kodunu komut satýrýndan alýn
	std::string sAuth = utils->WstringToString(GetProcessCommandLine("LeagueClientUx.exe"));
	if (sAuth.empty())
	{
		//MessageBoxA(0, "Istemci bulunamadi", 0, 0);
		return 0;
	}

	std::string appPort = "\"--app-port=";
	size_t nPos = sAuth.find(appPort);
	if (nPos != std::string::npos)
		leaguePort = std::stoi(sAuth.substr(nPos + appPort.size(), 5));

	std::string remotingAuth = "--remoting-auth-token=";
	nPos = sAuth.find(remotingAuth) + strlen(remotingAuth.c_str());
	if (nPos != std::string::npos)
	{
		std::string token = "riot:" + sAuth.substr(nPos, 22);
		unsigned char m_Test[50]{};
		strncpy((char*)m_Test, token.c_str(), sizeof(m_Test));
		leagueToken = base64.Encode(m_Test, token.size()).c_str();
	}
	else
	{
		MessageBoxA(0, "Istemciye baglanilamadi", 0, 0);

		return 0;
	}

	MakeLeagueHeader();

	return 1;
}

void Auth::MakeLeagueHeader()
{
	leagueHeader = "Host: 127.0.0.1:" + std::to_string(leaguePort) + "\r\n" +
		"Connection: keep-alive" + "\r\n" +
		"Authorization: Basic " + leagueToken + "\r\n" +
		"Accept: application/json" + "\r\n" +
		"Content-Type: application/json" + "\r\n" +
		"Origin: https://127.0.0.1:" + std::to_string(leaguePort) + "\r\n" +
		"User-Agent: Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) LeagueOfLegendsClient/11.24.413.2485 (CEF 74) Safari/537.36" + "\r\n" +
		"X-Riot-Source: rcp-fe-lol-social" + "\r\n" +
		"Referer: https://127.0.0.1:" + std::to_string(leaguePort) + "/index.html" + "\r\n" +
		"Accept-Encoding: gzip, deflate, br" + "\r\n" +
		"Accept-Language: Müþteri bulunamadý;q=0.8";
}

std::wstring Auth::GetProcessCommandLine(std::string sProcessName)
{
	std::wstring wstrResult;
	HANDLE Handle;
	DWORD ProcessID = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (snapshot != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 entry{};
		entry.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(snapshot, &entry))
		{
			do
			{
				char temp[260];
				sprintf(temp, "%ws", entry.szExeFile);
				if (!stricmp(temp, sProcessName.c_str()))
				{
					ProcessID = entry.th32ProcessID;
					Handle = OpenProcess(PROCESS_ALL_ACCESS, 0, entry.th32ProcessID);

					PROCESS_BASIC_INFORMATION pbi{};
					PEB peb = { 0 };
					tNtQueryInformationProcess NtQueryInformationProcess =
						(tNtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
					NTSTATUS status = NtQueryInformationProcess(Handle, ProcessBasicInformation, &pbi, sizeof(pbi), 0);

					if (NT_SUCCESS(status))
					{
						ReadProcessMemory(Handle, pbi.PebBaseAddress, &peb, sizeof(peb), 0);
					}
					PRTL_USER_PROCESS_PARAMETERS pRtlProcParam = peb.ProcessParameters;
					PRTL_USER_PROCESS_PARAMETERS pRtlProcParamCopy =
						(PRTL_USER_PROCESS_PARAMETERS)malloc(sizeof(RTL_USER_PROCESS_PARAMETERS));

					bool result = ReadProcessMemory(Handle,
						pRtlProcParam,
						pRtlProcParamCopy,
						sizeof(RTL_USER_PROCESS_PARAMETERS),
						NULL);
					PWSTR wBuffer = pRtlProcParamCopy->CommandLine.Buffer;
					USHORT len = pRtlProcParamCopy->CommandLine.Length;
					PWSTR wBufferCopy = (PWSTR)malloc(len);
					result = ReadProcessMemory(Handle,
						wBuffer,
						wBufferCopy,
						len,
						NULL);

					wstrResult = std::wstring(wBufferCopy);

					CloseHandle(Handle);
					break;
				}
			} while (Process32Next(snapshot, &entry));
		}
	}
	CloseHandle(snapshot);
	return wstrResult;
}

Auth* auth = new Auth();