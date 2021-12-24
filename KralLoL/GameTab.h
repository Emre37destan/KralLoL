#pragma once

#include "Definitions.h"
#include "Includes.h"
#include "HTTP.h"
#include "Auth.h"
#include "Utils.h"
#include "Misc.h"

class GameTab
{
public:

	static void Render()
	{
		if (ImGui::BeginTabItem("Oyun"))
		{
			static std::string result;
			static std::string custom;
			static int gameID = 0;

			ImGui::Text("Modlar:");
			ImGui::Columns(4, 0, false);

			if (ImGui::Button("Kapali Secim"))
				gameID = BlindPick;

			if (ImGui::Button("Sirali Secim"))
				gameID = DraftPick;

			if (ImGui::Button("Tek/Cift"))
				gameID = SoloDuo;

			if (ImGui::Button("Esnek"))
				gameID = Flex;

			ImGui::NextColumn();

			if (ImGui::Button("ARAM"))
				gameID = ARAM;

			if (ImGui::Button("ARURF"))
				gameID = ARURF;

			if (ImGui::Button("Nexus Akini"))
				gameID = NexusBlitz;

			if (ImGui::Button("Ulti Buyu Kitabi"))
				gameID = UltimateSpellBook;

			ImGui::NextColumn();

			if (ImGui::Button("TFT Normal"))
				gameID = TFTNormal;

			if (ImGui::Button("TFT Derece"))
				gameID = TFTRanked;

			if (ImGui::Button("TFT Hiper Yenileme"))
				gameID = TFTHyperRoll;

			if (ImGui::Button("TFT Egitim"))
				gameID = TFTTutorial;

			ImGui::NextColumn();

			if (ImGui::Button("Alistirma Araci"))
			{
				custom = R"({"customGameLobby":{"configuration":{"gameMode":"PRACTICETOOL","gameMutator":"","gameServerRegion":"","mapId":11,"mutators":{"id":1},"spectatorPolicy":"AllAllowed","teamSize":1},"lobbyName":"KralLoL","lobbyPassword":null},"isCustom":true})";
			}

			if (ImGui::Button("Alistirma Araci 5v5"))
			{
				custom = R"({"customGameLobby":{"configuration":{"gameMode":"PRACTICETOOL","gameMutator":"","gameServerRegion":"","mapId":11,"mutators":{"id":1},"spectatorPolicy":"AllAllowed","teamSize":5},"lobbyName":"KralLoL","lobbyPassword":null},"isCustom":true})";
			}

			if (ImGui::Button("Carpisma"))
				gameID = Clash;

			ImGui::Columns(1);

			ImGui::Separator();

			ImGui::Columns(4, 0, false);

			if (ImGui::Button("Egitim 1"))
				gameID = Tutorial1;

			if (ImGui::Button("Egitim 2"))
				gameID = Tutorial2;

			if (ImGui::Button("Egitim 3"))
				gameID = Tutorial3;

			ImGui::NextColumn();

			if (ImGui::Button("Yapay Giris"))
				gameID = IntroBots;

			if (ImGui::Button("Yapay Baslangic"))
				gameID = BeginnerBots;

			if (ImGui::Button("Yapay Orta"))
				gameID = IntermediateBots;

			ImGui::NextColumn();

			if (ImGui::Button("Sihirdar Vadisi"))
				custom = R"({"customGameLobby":{"configuration":{"gameMode":"CLASSIC","gameMutator":"","gameServerRegion":"","mapId":11,"mutators":{"id":1},"spectatorPolicy":"AllAllowed","teamSize":5},"lobbyName":"KralLoL","lobbyPassword":null},"isCustom":true})";

			if (ImGui::Button("Sonsuz Ucurum"))
				custom = R"({"customGameLobby":{"configuration":{"gameMode":"ARAM","gameMutator":"","gameServerRegion":"","mapId":12,"mutators":{"id":1},"spectatorPolicy":"AllAllowed","teamSize":5},"lobbyName":"KralLoL","lobbyPassword":null},"isCustom":true})";

			//"id" 1- blind 2- draft -4 all random 6- tournament draft

			ImGui::NextColumn();

			static std::vector<std::pair<int, std::string>>botChamps;
			static size_t indexBots = 0; // Burada seçim verilerimizi bir indeks olarak saklýyoruz.
			const char* labelBots = "Bot";
			if (!botChamps.empty())
				labelBots = botChamps[indexBots].second.c_str();
			if (ImGui::BeginCombo("##comboBots", labelBots, 0))
			{
				if (botChamps.empty())
				{
					std::string getBots = http->Request("GET", "https://127.0.0.1/lol-lobby/v2/lobby/custom/available-bots", "", auth->leagueHeader, "", "", auth->leaguePort);
					Json::CharReaderBuilder builder;
					const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
					JSONCPP_STRING err;
					Json::Value root;
					if (reader->parse(getBots.c_str(), getBots.c_str() + static_cast<int>(getBots.length()), &root, &err))
					{
						if (root.isArray())
						{
							for (Json::Value::ArrayIndex i = 0; i < root.size(); i++)
							{
								std::pair<int, std::string>temp = { root[i]["id"].asInt(),root[i]["name"].asString() };
								botChamps.emplace_back(temp);
							}
							std::sort(botChamps.begin(), botChamps.end(), [](std::pair<int, std::string> a, std::pair<int, std::string >b) {return a.second < b.second; });
						}
					}
				}

				for (size_t n = 0; n < botChamps.size(); n++)
				{
					const bool is_selected = (indexBots == n);
					if (ImGui::Selectable(botChamps[n].second.c_str(), is_selected))
						indexBots = n;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			std::vector<std::string>difficulties = { "BASLANGIC", "ORTA" };
			static size_t indexDifficulty = 0; // Burada seçim verilerimizi bir indeks olarak saklýyoruz.
			const char* labelDifficulty = difficulties[indexDifficulty].c_str();

			if (ImGui::BeginCombo("##comboDifficulty", labelDifficulty, 0))
			{
				for (size_t n = 0; n < difficulties.size(); n++)
				{
					const bool is_selected = (indexDifficulty == n);
					if (ImGui::Selectable(difficulties[n].c_str(), is_selected))
						indexDifficulty = n;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			static int botTeam = 0;

			if (ImGui::Button("Bot Ekle##addBot"))
			{
				std::string team = botTeam ? R"(,"teamId":"200"})" : R"(,"teamId":"100"})";
				std::string body = R"({"botDifficulty":")" + difficulties[indexDifficulty] + R"(","championId":)" + std::to_string(botChamps[indexBots].first) + team;
				result = http->Request("POST", "https://127.0.0.1/lol-lobby/v1/lobby/custom/bots", body, auth->leagueHeader, "", "", auth->leaguePort);
			}
			ImGui::SameLine();
			ImGui::RadioButton("Mavi", &botTeam, 0); ImGui::SameLine();
			ImGui::RadioButton("Kirmizi", &botTeam, 1);

			ImGui::Columns(1);

			//ImGui::Separator();
			//static int inputGameID = 0;
			//ImGui::InputInt("##inputGameID:", &inputGameID, 1, 100);
			//ImGui::SameLine();
			//if (ImGui::Button("Submit##gameID"))
			//{
			//	gameID = inputGameID;
			//}

			// if pressed any button, gameID or custom changed
			if (gameID != 0 || !custom.empty())
			{
				std::string body;
				if (custom.empty())
				{
					body = R"({"queueId":)" + std::to_string(gameID) + "}";
				}
				else
				{
					body = custom;
					custom = "";
				}
				result = http->Request("POST", "https://127.0.0.1/lol-lobby/v2/lobby", body, auth->leagueHeader, "", "", auth->leaguePort);

				/*	{
						for (int i = 0; i < 10001; i++)
						{
							std::string res = R"({"customGameLobby":{"configuration":{"gameMode":"CLASSIC","gameMutator":"","gameServerRegion":"","mapId":11,"mutators":{"id":)" + std::to_string(i) + R"(},"spectatorPolicy":"AllAllowed","teamSize":5},"lobbyName":"KBot","lobbyPassword":null},"isCustom":true})";
							std::string xdd= http.Request("POST", "https://127.0.0.1/lol-lobby/v2/lobby", (res), auth->leagueHeader, "", "", clientPort);
							if (xdd.find("errorCode") == std::string::npos)
								std::cout << i << std::endl;
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
						}
					}*/

				gameID = 0;
			}

			ImGui::Separator();

			ImGui::Columns(3, 0, false);
			if (ImGui::Button("Karsilasma Baslat"))
			{
				result = http->Request("POST", "https://127.0.0.1/lol-lobby/v2/lobby/matchmaking/search", "", auth->leagueHeader, "", "", auth->leaguePort);
			}
			ImGui::NextColumn();

			// sýra arama sýrasýnda buna basarsanýz, sýrayý tekrar baþlatamazsýnýz.
			// lobiye tekrar girmezseniz :)
			if (ImGui::Button("Atlat"))
			{
				result = http->Request("POST", R"(https://127.0.0.1/lol-login/v1/session/invoke?destination=lcdsServiceProxy&method=call&args=["","teambuilder-draft","quitV2",""])", "", auth->leagueHeader, "", "", auth->leaguePort);
			}
			ImGui::SameLine();
			Misc::HelpMarker("Lobiyi aninda atlatir, yine de LP kaybedersiniz, ancak istemciyi yeniden baslatmaniz gerekmez");
			ImGui::NextColumn();
			if (ImGui::Button("Coklu OP.GG"))
			{
				std::string names;
				std::string champSelect = http->Request("GET", "https://127.0.0.1/lol-champ-select/v1/session", "", auth->leagueHeader, "", "", auth->leaguePort);
				if (!champSelect.empty() && champSelect.find("RPC_ERROR") == std::string::npos)
				{
					Json::CharReaderBuilder builder;
					const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
					JSONCPP_STRING err;
					Json::Value rootLocale;
					Json::Value rootCSelect;
					Json::Value rootSummoner;

					std::string regionLocale = http->Request("GET", "https://127.0.0.1/riotclient/get_region_locale", "", auth->leagueHeader, "", "", auth->leaguePort);
					if (reader->parse(regionLocale.c_str(), regionLocale.c_str() + static_cast<int>(regionLocale.length()), &rootLocale, &err))
					{
						std::wstring region = utils->StringToWstring(rootLocale["webRegion"].asString());
						if (reader->parse(champSelect.c_str(), champSelect.c_str() + static_cast<int>(champSelect.length()), &rootCSelect, &err))
						{
							std::wstring url = L"https://" + region + L".op.gg/multi/query=";
							auto &teamArr = rootCSelect["myTeam"];
							if (teamArr.isArray())
							{
								for (Json::Value::ArrayIndex i = 0; i < teamArr.size(); ++i)
								{
									std::string summId = teamArr[i]["summonerId"].asString();
									if (summId != "0")
									{
										std::string summoner = http->Request("GET", "https://127.0.0.1/lol-summoner/v1/summoners/" + summId, "", auth->leagueHeader, "", "", auth->leaguePort);
										if (reader->parse(summoner.c_str(), summoner.c_str() + static_cast<int>(summoner.length()), &rootSummoner, &err))
										{
											std::wstring summName = utils->StringToWstring(rootSummoner["internalName"].asString());
											url += summName + L",";
										}
									}
								}
								ShellExecuteW(0, 0, url.c_str(), 0, 0, SW_SHOW);
								result = utils->WstringToString(url);
							}
						}
					}
				}
				else
					result = "Sampiyon secimi bulunamadi";
			}

			ImGui::Columns(1);

			ImGui::Separator();

			ImGui::Columns(3, 0, false);
			ImGui::Checkbox("Oto Sec", &S.gameTab.autoAcceptEnabled);

			ImGui::NextColumn();
			if (ImGui::Button("Herkesi lobiye davet et"))
			{
				std::string getFriends = http->Request("GET", "https://127.0.0.1/lol-chat/v1/friends", "", auth->leagueHeader, "", "", auth->leaguePort);

				Json::CharReaderBuilder builder;
				const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
				JSONCPP_STRING err;
				Json::Value root;
				if (reader->parse(getFriends.c_str(), getFriends.c_str() + static_cast<int>(getFriends.length()), &root, &err))
				{
					if (root.isArray())
					{
						for (Json::Value::ArrayIndex i = 0; i < root.size(); ++i)
						{
							std::string friendSummId = root[i]["summonerId"].asString();
							std::string inviteBody = "[{\"toSummonerId\":" + friendSummId + "}]";
							http->Request("POST", "https://127.0.0.1/lol-lobby/v2/lobby/invitations", inviteBody,
								auth->leagueHeader, "", "", auth->leaguePort);
						}
						result = "Arkadaslari lobiye davet etti";
					}
				}
			}
			ImGui::Columns(1);

			ImGui::Columns(2, 0, false);

			ImGui::Text("Aninda mesaj: ");
			ImGui::SameLine();
			static char bufInstantMessage[500];
			std::copy(S.gameTab.instantMessage.begin(), S.gameTab.instantMessage.end(), bufInstantMessage);
			//ImGui::SetNextItemWidth(S.Window.width / 3);
			ImGui::InputText("##inputInstantMessage", bufInstantMessage, IM_ARRAYSIZE(bufInstantMessage));
			S.gameTab.instantMessage = bufInstantMessage;

			ImGui::NextColumn();
			ImGui::SliderInt("Gecikme##sliderInstantMessageDelay", &S.gameTab.instantMessageDelay, 0, 10000, "%d ms");

			ImGui::Columns(1);

			ImGui::Columns(3, 0, false);

			ImGui::Checkbox("HemenAl", &S.gameTab.instalockEnabled);
			ImGui::NextColumn();

			ImGui::SliderInt("Gecikme##sliderInstalockDelay", &S.gameTab.instalockDelay, 0, 10000, "%d ms");

			ImGui::NextColumn();

			ImGui::Checkbox("Sampiyon yasagindan kacin", &S.gameTab.dodgeOnBan);

			ImGui::SameLine();
			Misc::HelpMarker("Yedekleme secimini yok sayar");

			ImGui::Columns(1);

			static bool isStillDownloading = true;
			if (!champSkins.empty())
				isStillDownloading = false;

			static std::string chosenInstalock = "HemenAl \t\tSecili: " + Misc::ChampIdToName(S.gameTab.instalockId) + "###AnimatedInstalock";
			static int lastInstalockId = 0;
			if ((lastInstalockId != S.gameTab.instalockId) && !isStillDownloading)
			{
				lastInstalockId = S.gameTab.instalockId;
				chosenInstalock = "HemenAl \t\tSecili: " + Misc::ChampIdToName(S.gameTab.instalockId) + "###AnimatedInstalock";
			}
			if (ImGui::CollapsingHeader(chosenInstalock.c_str()))
			{
				std::vector<std::pair<int, std::string>>instalockChamps = GetInstalockChamps();
				for (auto &champ : instalockChamps)
				{
					char bufchamp[128];
					sprintf_s(bufchamp, "##Select %s", champ.second.c_str());
					ImGui::Text("%s", champ.second.c_str());
					ImGui::SameLine();
					ImGui::RadioButton(bufchamp, &S.gameTab.instalockId, champ.first);
				}
			}

			static std::string chosenBackup = "Yedek secimi \t\t\tSecili: " + Misc::ChampIdToName(S.gameTab.backupId) + "###AnimatedBackup";
			static int lastBackupId = 0;
			if ((lastBackupId != S.gameTab.backupId) && !isStillDownloading)
			{
				lastBackupId = S.gameTab.backupId;
				chosenBackup = "Yedek secimi \t\t\tSecili: " + Misc::ChampIdToName(S.gameTab.backupId) + "###AnimatedBackup";
			}
			if (ImGui::CollapsingHeader(chosenBackup.c_str()))
			{
				ImGui::Text("Hicbiri");
				ImGui::SameLine();
				ImGui::RadioButton("##noneBackupPick", &S.gameTab.backupId, 0);
				std::vector<std::pair<int, std::string>>instalockChamps = GetInstalockChamps();
				for (auto &champ : instalockChamps)
				{
					char bufchamp[128];
					sprintf_s(bufchamp, "##Select %s", champ.second.c_str());
					ImGui::Text("%s", champ.second.c_str());
					ImGui::SameLine();
					ImGui::RadioButton(bufchamp, &S.gameTab.backupId, champ.first);
				}
			}

			static std::string chosenAutoban = "Oto ban\t\t\t\tSecili: " + Misc::ChampIdToName(S.gameTab.autoBanId) + "###AnimatedAutoban";
			static int lastAutoban = 0;
			if ((lastAutoban != S.gameTab.autoBanId) && !isStillDownloading)
			{
				lastAutoban = S.gameTab.autoBanId;
				chosenAutoban = "Oto ban\t\t\t\tSecili: " + Misc::ChampIdToName(S.gameTab.autoBanId) + "###AnimatedAutoban";
			}
			if (ImGui::CollapsingHeader(chosenAutoban.c_str()))
			{
				if (champSkins.empty())
				{
					ImGui::Text("Sampiyon verileri hala indiriliyor");
				}
				else
				{
					ImGui::Text("Hicbiri");
					ImGui::SameLine();
					ImGui::RadioButton("##nonechamp", &S.gameTab.autoBanId, 0);
					for (auto &c : champSkins)
					{
						char bufchamp[128];
						sprintf_s(bufchamp, "##Select %s", c.name.c_str());
						ImGui::Text("%s", c.name.c_str());
						ImGui::SameLine();
						ImGui::RadioButton(bufchamp, &S.gameTab.autoBanId, c.key);
					}
				}
			}

		//	ImGui::Separator();

		//	 Patched :(
		/*	if (ImGui::Button("Beles ARAM/ARURF boost"))
			{
				std::string wallet = http->Request("GET", "https://127.0.0.1/lol-inventory/v1/wallet/RP", "", auth->leagueHeader, "", "", auth->leaguePort);

				Json::CharReaderBuilder builder;
				const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
				JSONCPP_STRING err;
				Json::Value root;
				if (!reader->parse(wallet.c_str(), wallet.c_str() + static_cast<int>(wallet.length()), &root, &err))
					result = wallet;
				else
				{
					unsigned RP = root["RP"].asUInt();
					if (RP < 95)
					{
						result = http->Request("POST", R"(https://127.0.0.1/lol-login/v1/session/invoke?destination=lcdsServiceProxy&method=call&args=["","teambuilder-draft","activateBattleBoostV1",""])", "", auth->leagueHeader, "", "", auth->leaguePort);
					}
					else
					{
						MessageBoxA(0, "Yeterli RP'niz var", "Size ucretsiz bir cilt takviyesi vermek mumkun degil", 0);
					}
				}
			}
			ImGui::SameLine();
			Misc::HelpMarker("Yalnýzca destek icin yeterli RP'niz olmadiginda calisir");*/

			static Json::StreamWriterBuilder wBuilder;
			static std::string sResultJson;
			static char* cResultJson;

			if (!result.empty())
			{
				Json::CharReaderBuilder builder;
				const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
				JSONCPP_STRING err;
				Json::Value root;
				if (!reader->parse(result.c_str(), result.c_str() + static_cast<int>(result.length()), &root, &err))
					sResultJson = result;
				else
				{
					sResultJson = Json::writeString(wBuilder, root);
				}
				result = "";
			}

			if (!sResultJson.empty())
			{
				cResultJson = &sResultJson[0];
				ImGui::InputTextMultiline("##gameResult", cResultJson, sResultJson.size() + 1, ImVec2(600, 300));
			}

			ImGui::EndTabItem();
		}
	}

	static std::vector<std::pair<int, std::string>> GetInstalockChamps()
	{
		std::vector<std::pair<int, std::string>>temp;

		std::string result = http->Request("GET", "https://127.0.0.1/lol-champions/v1/owned-champions-minimal", "", auth->leagueHeader, "", "", auth->leaguePort);
		Json::CharReaderBuilder builder;
		const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
		JSONCPP_STRING err;
		Json::Value root;
		if (reader->parse(result.c_str(), result.c_str() + static_cast<int>(result.length()), &root, &err))
		{
			if (root.isArray())
			{
				for (Json::Value::ArrayIndex i = 0; i < root.size(); i++)
				{
					if (root[i]["freeToPlay"].asBool() == true || root[i]["ownership"]["owned"].asBool() == true)
					{
						std::pair<int, std::string > champ = { root[i]["id"].asInt() , root[i]["alias"].asString() };
						temp.emplace_back(champ);
					}
				}
			}
		}
		std::sort(temp.begin(), temp.end(), [](std::pair<int, std::string > a, std::pair<int, std::string >b) {return a.second < b.second; });
		return temp;
	}

	static void InstantMessage()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(S.gameTab.instantMessageDelay));
		Json::Value root;
		Json::CharReaderBuilder builder;
		const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
		JSONCPP_STRING err;
		std::string getChat = http->Request("GET", "https://127.0.0.1/lol-chat/v1/conversations", "", auth->leagueHeader, "", "", auth->leaguePort);
		if (reader->parse(getChat.c_str(), getChat.c_str() + static_cast<int>(getChat.length()), &root, &err))
		{
			if (root.isArray())
			{
				for (Json::Value::ArrayIndex i = 0; i < root.size(); i++)
				{
					if (root[i]["type"].asString() != "championSelect")
						continue;
					std::string lobbyID = root[i]["id"].asString();
					std::string request = "https://127.0.0.1/lol-chat/v1/conversations/" + lobbyID + "/messages";
					std::string error = "errorCode";
					while (error.find("errorCode") != std::string::npos)
					{
						error = http->Request("POST", request, R"({"type":"chat", "body":")" + std::string(S.gameTab.instantMessage) + R"("})", auth->leagueHeader, "", "", auth->leaguePort);
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					break;
				}
			}
		}
	}

	static void AutoAccept()
	{
		while (true)
		{
			if (!FindWindowA("RiotWindowClass", "League of Legends (TM) Client"))
			{
				if (::FindWindowA("RCLIENT", "League of Legends"))
				{
					if (S.gameTab.autoAcceptEnabled || S.gameTab.autoBanId || (S.gameTab.dodgeOnBan && S.gameTab.instalockEnabled) || (S.gameTab.instalockEnabled && S.gameTab.instalockId))
					{
						Json::Value rootSearch;
						Json::Value rootChampSelect;
						Json::Value rootSession;
						Json::CharReaderBuilder builder;
						const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
						JSONCPP_STRING err;
						std::string getSearchState = http->Request("GET", "https://127.0.0.1/lol-lobby/v2/lobby/matchmaking/search-state", "", auth->leagueHeader, "", "", auth->leaguePort);
						if (reader->parse(getSearchState.c_str(), getSearchState.c_str() + static_cast<int>(getSearchState.length()), &rootSearch, &err))
						{
							static bool foundCell = false;
							static bool sendMessage = true;
							static int useBackupId = 0;
							std::string searchState = rootSearch["searchState"].asString();
							if (searchState == "Found")
							{
								std::string getChampSelect = http->Request("GET", "https://127.0.0.1/lol-champ-select/v1/session", "", auth->leagueHeader, "", "", auth->leaguePort);
								if (getChampSelect.find("RPC_ERROR") != std::string::npos)
								{
									foundCell = false;
									sendMessage = true;
									useBackupId = 0;
									if (S.gameTab.autoAcceptEnabled)
									{
										http->Request("POST", "https://127.0.0.1/lol-matchmaking/v1/ready-check/accept", "", auth->leagueHeader, "", "", auth->leaguePort);
									}
									std::this_thread::sleep_for(std::chrono::milliseconds(100));
								}
								else
								{
									if (reader->parse(getChampSelect.c_str(), getChampSelect.c_str() + static_cast<int>(getChampSelect.length()), &rootChampSelect, &err))
									{
										//if (S.gameTab.dodgeOnBan && S.gameTab.instalockId && S.gameTab.instalockEnabled)
										//{
										//	// empty on draft? look within actions
										//	auto myTeamBans = rootChampSelect["bans"]["myTeamBans"];
										//	if (myTeamBans.isArray())
										//	{
										//		for (Json::Value::ArrayIndex i = 0; i < myTeamBans.size(); i++)
										//		{
										//			if (myTeamBans[i].asInt() == S.gameTab.instalockId)
										//			{
										//				http->Request("POST", R"(https://127.0.0.1/lol-login/v1/session/invoke?destination=lcdsServiceProxy&method=call&args=["","teambuilder-draft","quitV2",""])", "", auth->leagueHeader, "", "", auth->leaguePort);
										//			}
										//		}
										//	}
										//	auto theirTeamBans = rootChampSelect["bans"]["theirTeamBans"];
										//	if (theirTeamBans.isArray())
										//	{
										//		for (Json::Value::ArrayIndex i = 0; i < theirTeamBans.size(); i++)
										//		{
										//			if (theirTeamBans[i].asInt() == S.gameTab.instalockId)
										//			{
										//				http->Request("POST", R"(https://127.0.0.1/lol-login/v1/session/invoke?destination=lcdsServiceProxy&method=call&args=["","teambuilder-draft","quitV2",""])", "", auth->leagueHeader, "", "", auth->leaguePort);
										//			}
										//		}
										//	}
										//}
										if (sendMessage && !S.gameTab.instantMessage.empty())
										{
											sendMessage = false;
											std::thread instantMessageThread(&GameTab::InstantMessage);
											instantMessageThread.detach();
										}
										if (S.gameTab.instalockEnabled || S.gameTab.autoBanId)
										{
											// Kendi zirvesine sahip ol
											std::string getSession = http->Request("GET", "https://127.0.0.1/lol-login/v1/session", "", auth->leagueHeader, "", "", auth->leaguePort);
											if (reader->parse(getSession.c_str(), getSession.c_str() + static_cast<int>(getSession.length()), &rootSession, &err))
											{
												static int cellId = 0;

												if (!foundCell)
												{
													auto &myTeam = rootChampSelect["myTeam"];
													std::string summId = rootSession["summonerId"].asString();
													if (myTeam.isArray())
													{
														// Kendi hücre kimliðini al
														for (Json::Value::ArrayIndex i = 0; i < myTeam.size(); i++)
														{
															if (myTeam[i]["summonerId"].asString() == summId)
															{
																cellId = myTeam[i]["cellId"].asInt();
																foundCell = true;
																break;
															}
														}
													}
												}
												else
												{
													for (Json::Value::ArrayIndex j = 0; j < rootChampSelect["actions"].size(); j++)
													{
														auto &actions = rootChampSelect["actions"][j];
														if (actions.isArray())
														{
															for (Json::Value::ArrayIndex i = 0; i < actions.size(); i++)
															{
																// Kendi eylemlerini ara
																if (actions[i]["actorCellId"].asInt() == cellId)
																{
																	std::string actionType = actions[i]["type"].asString();
																	if (actionType == "pick" && S.gameTab.instalockId && S.gameTab.instalockEnabled)
																	{
																		// Henüz seçmediysen
																		if (actions[i]["completed"].asBool() == false)
																		{
																			std::this_thread::sleep_for(std::chrono::milliseconds(S.gameTab.instalockDelay));

																			int currentPick = S.gameTab.instalockId;
																			if (useBackupId)
																				currentPick = useBackupId;

																			http->Request("PATCH", "https://127.0.0.1/lol-champ-select/v1/session/actions/" + actions[i]["id"].asString(),
																				R"({"completed":true,"championId":)" + std::to_string(currentPick) + "}", auth->leagueHeader, "", "", auth->leaguePort);
																		}
																		//else
																		//{
																		//	// Zaten seçtik yapacak bir þey yok o kadar uyu
																		//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
																		//	break;
																		//}
																	}
																	else if (actionType == "ban" && S.gameTab.autoBanId)
																	{
																		if (actions[i]["completed"].asBool() == false)
																		{
																			http->Request("PATCH", "https://127.0.0.1/lol-champ-select/v1/session/actions/" + actions[i]["id"].asString(),
																				R"({"completed":true,"championId":)" + std::to_string(S.gameTab.autoBanId) + "}", auth->leagueHeader, "", "", auth->leaguePort);
																		}
																	}
																	//else break;
																}
																// Yasaklama etkinleþtirilmiþse veya yedek seçimden kaçýnýlýrsa
																if ((S.gameTab.dodgeOnBan || S.gameTab.backupId) && S.gameTab.instalockEnabled && S.gameTab.instalockId)
																{
																	if (actions[i]["type"].asString() == "ban" && actions[i]["completed"].asBool() == true)
																	{
																		if (actions[i]["championId"].asInt() == S.gameTab.instalockId)
																		{
																			if (S.gameTab.dodgeOnBan)
																			{
																				http->Request("POST", R"(https://127.0.0.1/lol-login/v1/session/invoke?destination=lcdsServiceProxy&method=call&args=["","teambuilder-draft","quitV2",""])", "", auth->leagueHeader, "", "", auth->leaguePort);
																			}
																			else if (S.gameTab.backupId)
																			{
																				useBackupId = S.gameTab.backupId;
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
										else
										{
											std::this_thread::sleep_for(std::chrono::milliseconds(1000));
										}
									}
								}
							}
							else
							{
								foundCell = false;
								sendMessage = true;
								useBackupId = 0;
								std::this_thread::sleep_for(std::chrono::milliseconds(1000));
							}
						}
					}
				}
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
};