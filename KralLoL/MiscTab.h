#pragma once

#include "Definitions.h"
#include "Includes.h"
#include "HTTP.h"
#include "Auth.h"
#include "Misc.h"
#include "Settings.h"

class MiscTab
{
public:

	static std::string LevenshteinDistance(std::vector<std::string>vec, std::string str2)
	{
		int max = 999;
		std::string bestMatch;

		for (std::string str1 : vec)
		{
			int l_string_length1 = str1.length();
			int l_string_length2 = str2.length();
			int d[50 + 1][50 + 1]{};

			int i;
			int j;
			int l_cost;

			for (i = 0; i <= l_string_length1; i++)
			{
				d[i][0] = i;
			}
			for (j = 0; j <= l_string_length2; j++)
			{
				d[0][j] = j;
			}
			for (i = 1; i <= l_string_length1; i++)
			{
				for (j = 1; j <= l_string_length2; j++)
				{
					if (str1[static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(i) - 1] == str2[static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(j) - 1])
					{
						l_cost = 0;
					}
					else
					{
						l_cost = 1;
					}
					d[i][j] = (std::min)(
						d[i - 1][j] + 1,                  // Sil
						(std::min)(d[i][j - 1] + 1,         // Sok
							d[i - 1][j - 1] + l_cost)           // Ýkame
						);
					if ((i > 1) &&
						(j > 1) &&
						(str1[static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(i) - 1] == str2[static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(j) - 2]) &&
						(str1[static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(i) - 2] == str2[static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(j) - 1])
						)
					{
						d[i][j] = (std::min)(
							d[i][j],
							d[i - 2][j - 2] + l_cost   // Aktar
							);
					}
				}
			}

			if (d[l_string_length1][l_string_length2] <= max)
			{
				max = d[l_string_length1][l_string_length2];
				bestMatch = str1;
			}
		}
		return bestMatch;
	}

	static void Render()
	{
		if (ImGui::BeginTabItem("Cesit"))
		{
			static std::string result;

			ImGui::Columns(2, 0, false);

			if (ImGui::Button("Baska bir istemci baslat"))
			{
				if (!std::filesystem::exists(S.leaguePath))
				{
					result = "Gecersiz yol, bunu Ayarlar sekmesinde degistirin";
				}
				else
					ShellExecuteA(NULL, NULL, std::format("{}LeagueClient.exe", S.leaguePath).c_str(), "--allow-multiple-clients", NULL, SW_SHOWNORMAL);
			}

			if (ImGui::Button("UX yeniden baslat"))
			{
				result = http->Request("POST", "https://127.0.0.1/riotclient/kill-and-restart-ux", "", auth->leagueHeader, "", "", auth->leaguePort);
				if (result.find("failed") != std::string::npos)
				{
					if (auth->GetLeagueClientInfo())
						result = "Yeni lig istemcisine yeniden baglandi";
				}
			}

			ImGui::NextColumn();

			if (ImGui::Button("Eski istemciyi baslat"))
			{
				if (!std::filesystem::exists(S.leaguePath))
				{
					result = "Gecersiz yol, bunu Ayarlar sekmesinde degistirin";
				}
				else
				{
					Misc::LaunchLegacyClient();
				}
			}

			if (ImGui::Button("Istemciyi kapat"))
				result = http->Request("POST", "https://127.0.0.1/process-control/v1/process/quit", "", auth->leagueHeader, "", "", auth->leaguePort);

			ImGui::SameLine();
			if (ImGui::Button("Istemciyi kapatmaya zorla"))
				Misc::TaskKillLeague();

			ImGui::Columns(1);

			ImGui::Separator();

			ImGui::Columns(2, 0, false);

			if (ImGui::Button("Tum arkadaslik isteklerini kabul et"))
			{
				if (MessageBoxA(0, "Emin misin?", 0, MB_OKCANCEL) == IDOK)
				{
					std::string getFriends = http->Request("GET", "https://127.0.0.1/lol-chat/v1/friend-requests", "", auth->leagueHeader, "", "", auth->leaguePort);

					Json::CharReaderBuilder builder;
					const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
					JSONCPP_STRING err;
					Json::Value root;
					if (!reader->parse(getFriends.c_str(), getFriends.c_str() + static_cast<int>(getFriends.length()), &root, &err))
					{
						result = "JSON ayristirilamadi";
					}
					else
					{
						if (root.isArray())
						{
							for (Json::Value::ArrayIndex i = 0; i < root.size(); i++)
							{
								std::string req = "https://127.0.0.1/lol-chat/v1/friend-requests/" + root[i]["pid"].asString();
								http->Request("PUT", req, R"({"direction":"both"})", auth->leagueHeader, "", "", auth->leaguePort);
							}
							result = std::to_string(root.size()) + " arkadaslik istegi kabul edildi";
						}
					}
				}
			}

			ImGui::NextColumn();

			if (ImGui::Button("Tum arkadaslik isteklerini sil"))
			{
				if (MessageBoxA(0, "Emin misin?", 0, MB_OKCANCEL) == IDOK)
				{
					std::string getFriends = http->Request("GET", "https://127.0.0.1/lol-chat/v1/friend-requests", "", auth->leagueHeader, "", "", auth->leaguePort);

					Json::CharReaderBuilder builder;
					const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
					JSONCPP_STRING err;
					Json::Value root;
					if (!reader->parse(getFriends.c_str(), getFriends.c_str() + static_cast<int>(getFriends.length()), &root, &err))
					{
						result = "JSON ayristirilamadi";
					}
					else
					{
						if (root.isArray())
						{
							for (Json::Value::ArrayIndex i = 0; i < root.size(); i++)
							{
								std::string req = "https://127.0.0.1/lol-chat/v1/friend-requests/" + root[i]["pid"].asString();
								http->Request("DELETE", req, "", auth->leagueHeader, "", "", auth->leaguePort);
							}
							result = std::to_string(root.size()) + " arkadaslik istegi silindi";
						}
					}
				}
			}

			ImGui::Columns(1);

			static std::vector<std::pair<std::string, int>>items;
			static size_t item_current_idx = 0; // Burada seçim verilerimizi bir indeks olarak saklýyoruz.
			const char* combo_label = "Varsayilan";
			if (!items.empty())
				combo_label = items[item_current_idx].first.c_str();

			if (ImGui::Button("Tum arkadaslari kaldir"))
			{
				if (MessageBoxA(0, "Emin misin?", 0, MB_OKCANCEL) == IDOK)
				{
					std::string getFriends = http->Request("GET", "https://127.0.0.1/lol-chat/v1/friends", "", auth->leagueHeader, "", "", auth->leaguePort);

					Json::CharReaderBuilder builder;
					const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
					JSONCPP_STRING err;
					Json::Value root;
					if (!reader->parse(getFriends.c_str(), getFriends.c_str() + static_cast<int>(getFriends.length()), &root, &err))
					{
						result = "JSON ayristirilamadi";
					}
					else
					{
						if (root.isArray())
						{
							int iDeleted = 0;
							for (Json::Value::ArrayIndex i = 0; i < root.size(); ++i)
							{
								if (root[i]["groupId"].asUInt() == item_current_idx)
								{
									std::string req = "https://127.0.0.1/lol-chat/v1/friends/" + root[i]["pid"].asString();
									http->Request("DELETE", req, "", auth->leagueHeader, "", "", auth->leaguePort);
									iDeleted++;
								}
							}
							result = std::to_string(iDeleted) + " arkadas silindi";
						}
					}
				}
			}
			ImGui::SameLine();
			ImGui::Text(" Klasorden: ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			if (ImGui::BeginCombo("##comboGroups", combo_label, 0))
			{
				std::string getGroups = http->Request("GET", "https://127.0.0.1/lol-chat/v1/friend-groups", "", auth->leagueHeader, "", "", auth->leaguePort);
				Json::CharReaderBuilder builder;
				const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
				JSONCPP_STRING err;
				Json::Value root;
				if (reader->parse(getGroups.c_str(), getGroups.c_str() + static_cast<int>(getGroups.length()), &root, &err))
				{
					if (root.isArray())
					{
						items.clear();
						for (Json::Value::ArrayIndex i = 0; i < root.size(); i++)
						{
							std::pair<std::string, int > temp = { root[i]["name"].asString(), root[i]["id"].asInt() };
							items.emplace_back(temp);
						}
						std::sort(items.begin(), items.end(), [](std::pair<std::string, int > a, std::pair<std::string, int >b) {return a.second < b.second; });
					}
				}

				for (size_t n = 0; n < items.size(); n++)
				{
					const bool is_selected = (item_current_idx == n);
					if (ImGui::Selectable(items[n].first.c_str(), is_selected))
						item_current_idx = n;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Egiticiyi Atla"))
			{
				http->Request("POST", "https://127.0.0.1/telemetry/v1/events/new_player_experience", R"({"eventName":"hide_screen","plugin":"rcp-fe-lol-new-player-experience","screenName":"npe_tutorial_modules"})", auth->leagueHeader, "", "", auth->leaguePort);
				http->Request("PUT", "https://127.0.0.1/lol-npe-tutorial-path/v1/settings", R"({"hasSeenTutorialPath":true,"hasSkippedTutorialPath":true,"shouldSeeNewPlayerExperience":true})", auth->leagueHeader, "", "", auth->leaguePort);
				//DELETE https://127.0.0.1:63027/lol-statstones/v1/vignette-notifications HTTP/1.1
				// ?
			}

			// Patched :(
			if (ImGui::Button("Beles Tristana + Riot Kizi Kostumu"))
				result = http->Request("POST", "https://127.0.0.1/lol-login/v1/session/invoke?destination=inventoryService&method=giftFacebookFan&args=[]", "", auth->leagueHeader, "", "", auth->leaguePort);

			ImGui::SameLine();
			Misc::HelpMarker("Dugmeye bastiktan sonra yeniden oturum acin");
			

			ImGui::Separator();

			static std::vector<std::pair<std::string, std::string>>itemsDisenchant = {
	{"Sampiyon parcalari","CHAMPION_RENTAL"}, {"Sampiyon takma adlari","CHAMPION"},
	{"Kostum parcalari","CHAMPION_SKIN_RENTAL"}, {"Kostum takma adlari", "CHAMPION_SKIN"},
	{"Simgeler","STATSTONE_SHARD"},{"Totemler","WARDSKIN_RENTAL"}
			};
			static size_t itemIndexDisenchant = 0;
			const char* comboDisenchant = itemsDisenchant[itemIndexDisenchant].first.c_str();

			if (ImGui::Button("Tumunun buyusunu boz: "))
			{
				Json::Value root;
				Json::CharReaderBuilder builder;
				const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
				JSONCPP_STRING err;
				std::string getLoot = http->Request("GET", "https://127.0.0.1/lol-loot/v1/player-loot-map", "", auth->leagueHeader, "", "", auth->leaguePort);

				if (reader->parse(getLoot.c_str(), getLoot.c_str() + static_cast<int>(getLoot.length()), &root, &err))
				{
					if (MessageBoxA(0, "Emin misin?", 0, MB_OKCANCEL) == IDOK)
					{
						int i = 0;

						for (std::string name : root.getMemberNames())
						{
							std::regex regexStr("^" + itemsDisenchant[itemIndexDisenchant].second + "_[\\d]+");

							if (std::regex_match(name, regexStr))
							{
								std::string disenchantCase = itemsDisenchant[itemIndexDisenchant].second == "STATSTONE_SHARD" ? "DISENCHANT" : "disenchant";

								std::string disenchantName = itemsDisenchant[itemIndexDisenchant].second;
								if (itemsDisenchant[itemIndexDisenchant].second == "CHAMPION_SKIN_RENTAL")
									disenchantName = "SKIN_RENTAL";

								std::string disenchantUrl = std::format("https://127.0.0.1/lol-loot/v1/recipes/{0}_{1}/craft?repeat=1", disenchantName, disenchantCase);
								std::string disenchantBody = std::format(R"(["{}"])", name).c_str();
								http->Request("POST", disenchantUrl, disenchantBody, auth->leagueHeader, "", "", auth->leaguePort);
								i++;
							}
						}
						result = std::format("Buyusu bozuldu {0} {1}", std::to_string(i), itemsDisenchant[itemIndexDisenchant].first);
					}
				}
				else
					result = "Ganimet bulunamadi";
			}

			ImGui::SameLine();

			ImGui::SetNextItemWidth(static_cast<float>(S.Window.width / 3));
			if (ImGui::BeginCombo("##comboDisenchant", comboDisenchant, 0))
			{
				for (size_t n = 0; n < itemsDisenchant.size(); n++)
				{
					const bool is_selected = (itemIndexDisenchant == n);
					if (ImGui::Selectable(itemsDisenchant[n].first.c_str(), is_selected))
						itemIndexDisenchant = n;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			// Levenshtein mesafe algoritmasý ile en yakýn þampiyon adýný alma ve kimliðini alma
			ImGui::Text("ID sampiyon adi");
			static std::vector<std::string>champNames;
			if (!champSkins.empty() && champNames.empty())
			{
				for (auto &champ : champSkins)
					champNames.emplace_back(champ.name);
			}

			static char bufChampionName[50];
			static size_t lastSize = 0;
			static std::string closestChampion;
			static std::string closestId;
			ImGui::SetNextItemWidth(static_cast<float>(S.Window.width / 3));
			ImGui::InputText("##inputChampionName", bufChampionName, IM_ARRAYSIZE(bufChampionName));
			if (strlen(bufChampionName) < 1)
			{
				closestChampion = "";
				closestId = "";
				lastSize = 0;
			}
			else if (lastSize != strlen(bufChampionName))
			{
				lastSize = strlen(bufChampionName);
				closestChampion = LevenshteinDistance(champNames, bufChampionName);

				for (auto &champ : champSkins)
				{
					if (closestChampion == champ.name)
					{
						closestId = std::to_string(champ.key);
						break;
					}
				}
			}
			ImGui::SameLine();
			ImGui::TextWrapped("%s ID: %s", closestChampion.c_str(), closestId.c_str());

			if (ImGui::Button("Hesabin e-postasini kontrol edin"))
				result = http->Request("GET", "https://127.0.0.1/lol-email-verification/v1/email", "", auth->leagueHeader, "", "", auth->leaguePort);

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
				ImGui::InputTextMultiline("##miscResult", cResultJson, sResultJson.size() + 1, ImVec2(600, 200));
			}

			ImGui::EndTabItem();
		}
	}
};