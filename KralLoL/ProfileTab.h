#pragma once

#include "Definitions.h"
#include "Includes.h"
#include "HTTP.h"
#include "Auth.h"

class ProfileTab
{
public:
	static void Render()
	{
		if (ImGui::BeginTabItem("Profil"))
		{
			static char statusText[1024 * 16];
			ImGui::Text("Durum:");
			ImGui::InputTextMultiline("##inputStatus", (statusText), IM_ARRAYSIZE(statusText), ImVec2(400, 100), ImGuiInputTextFlags_AllowTabInput);
			if (ImGui::Button("Durumu Gonder"))
			{
				std::string body = "{\"statusMessage\":\"" + std::string(statusText) + "\"}";

				size_t nPos = 0;
				while (nPos != std::string::npos)
				{
					nPos = body.find("\n", nPos);
					if (nPos != std::string::npos)
					{
						body.erase(body.begin() + nPos);
						body.insert(nPos, "\\n");
					}
				}
				std::string result = http->Request("PUT", "https://127.0.0.1/lol-chat/v1/me", body, auth->leagueHeader, "", "", auth->leaguePort);
				if (result.find("errorCode") != std::string::npos)
					MessageBoxA(0, result.c_str(), 0, 0);
			}

			ImGui::SameLine();
			static int availability = 0;
			static int lastAvailability = 0;
			ImGui::RadioButton("Cevrimici", &availability, 0); ImGui::SameLine();
			ImGui::RadioButton("Mobil", &availability, 1); ImGui::SameLine();
			ImGui::RadioButton("Disarida", &availability, 2); ImGui::SameLine();
			ImGui::RadioButton("Cevrimdisi", &availability, 3);

			if (availability != lastAvailability)
			{
				lastAvailability = availability;
				std::string body = R"({"availability":")";
				switch (availability)
				{
				case 0:
					body += "online";
					break;
				case 1:
					body += "mobile";
					break;
				case 2:
					body += "away";
					break;
				case 3:
					body += "offline";
					break;
				}
				body += "\"}";
				http->Request("PUT", "https://127.0.0.1/lol-chat/v1/me", body, auth->leagueHeader, "", "", auth->leaguePort);
			}

			ImGui::Separator();

			ImGui::Text("Rutbe:");
			static int rank = 0;
			ImGui::RadioButton("Demir", &rank, 0); ImGui::SameLine();
			ImGui::RadioButton("Bronz", &rank, 1); ImGui::SameLine();
			ImGui::RadioButton("Gumus", &rank, 2); ImGui::SameLine();
			ImGui::RadioButton("Altin", &rank, 3); ImGui::SameLine();
			ImGui::RadioButton("Patin", &rank, 4); ImGui::SameLine();
			ImGui::RadioButton("Elmas", &rank, 5); ImGui::SameLine();
			ImGui::RadioButton("Usta", &rank, 6); ImGui::SameLine();
			ImGui::RadioButton("BuyukUsta", &rank, 7); ImGui::SameLine();
			ImGui::RadioButton("Sampiyon", &rank, 8);

			static int tier = 0;
			ImGui::RadioButton("I", &tier, 0); ImGui::SameLine();
			ImGui::RadioButton("II", &tier, 1); ImGui::SameLine();
			ImGui::RadioButton("III", &tier, 2); ImGui::SameLine();
			ImGui::RadioButton("IV", &tier, 3); ImGui::SameLine();
			ImGui::RadioButton("Yok", &tier, 4);

			static int queue = 0;
			ImGui::RadioButton("Tek/Cift", &queue, 0); ImGui::SameLine();
			ImGui::RadioButton("Esnek 5v5", &queue, 1); ImGui::SameLine();
			ImGui::RadioButton("Esnek 3v3", &queue, 2); ImGui::SameLine();
			ImGui::RadioButton("TFT", &queue, 3); ImGui::SameLine();
			ImGui::RadioButton("()", &queue, 4);
			ImGui::RadioButton("Kral", &queue, 5);

			if (ImGui::Button("Gonder##submitRank"))
			{
				std::string body = R"({"lol":{"rankedLeagueQueue":")";
				switch (queue)
				{
				case 0:
					body += "RANKED_SOLO_5x5";
					break;
				case 1:
					body += "RANKED_FLEX_SR";
					break;
				case 2:
					body += "RANKED_FLEX_TT";
					break;
				case 3:
					body += "RANKED_TFT";
					break;
				case 4:
					body += "";
					break;
				}

				body += R"(","rankedLeagueTier":")";

				switch (rank)
				{
				case 0:
					body += "IRON";
					break;
				case 1:
					body += "BRONZE";
					break;
				case 2:
					body += "SILVER";
					break;
				case 3:
					body += "GOLD";
					break;
				case 4:
					body += "PLATINUM";
					break;
				case 5:
					body += "DIAMOND";
					break;
				case 6:
					body += "MASTER";
					break;
				case 7:
					body += "GRANDMASTER";
					break;
				case 8:
					body += "CHALLENGER";
					break;
				}

				body += R"(","rankedLeagueDivision":")";

				switch (tier)
				{
				case 0:
					body += "I";
					break;
				case 1:
					body += "II";
					break;
				case 2:
					body += "III";
					break;
				case 3:
					body += "IV";
					break;
				case 4:
					body += "";
					break;
				}

				body += R"("}})";

				http->Request("PUT", "https://127.0.0.1/lol-chat/v1/me", body, auth->leagueHeader, "", "", auth->leaguePort);
			}

			ImGui::SameLine();
			if (ImGui::Button("Sifirla##emptyRank"))
			{
				http->Request("PUT", "https://127.0.0.1/lol-chat/v1/me", R"({"lol":{"rankedLeagueQueue":"","rankedLeagueTier":"","rankedLeagueDivision":""}})", auth->leagueHeader, "", "", auth->leaguePort);
			}

			ImGui::Separator();

			static int iconID;
			ImGui::Text("Simge:");
			ImGui::InputInt("##inputIcon:", &iconID, 1, 100);
			ImGui::SameLine();
			if (ImGui::Button("Gonder##submitIcon"))
			{
				std::string body = R"({"profileIconId":)" + std::to_string(iconID) + "}";
				std::string result = http->Request("PUT", "https://127.0.0.1/lol-summoner/v1/current-summoner/icon", body, auth->leagueHeader, "", "", auth->leaguePort);
				if (result.find("errorCode") != std::string::npos)
				{
					MessageBoxA(0, result.c_str(), 0, 0);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Gonder 2##submitIcon2"))
			{
				std::string body = R"({"icon":)" + std::to_string(iconID) + "}";
				std::string result = http->Request("PUT", "https://127.0.0.1/lol-chat/v1/me/", body, auth->leagueHeader, "", "", auth->leaguePort);
				if (result.find("errorCode") != std::string::npos)
				{
					MessageBoxA(0, result.c_str(), 0, 0);
				}
			}

			static int backgroundID;
			ImGui::Text("Arka Plan:");

			ImGui::InputInt("##inputBackground", &backgroundID, 1, 100);
			ImGui::SameLine();
			if (ImGui::Button("Gonder##submitBackground"))
			{
				std::string body = R"({"key":"backgroundSkinId","value":)" + std::to_string(backgroundID) + "}";
				std::string result = http->Request("POST", "https://127.0.0.1/lol-summoner/v1/current-summoner/summoner-profile/", body, auth->leagueHeader, "", "", auth->leaguePort);
			}

			if (ImGui::CollapsingHeader("Arka plan"))
			{
				if (champSkins.empty())
				{
					ImGui::Text("Cilt verileri hala indiriliyor");
				}
				else
				{
					for (auto &c : champSkins)
					{
						if (ImGui::TreeNode(c.name.c_str()))
						{
							for (auto &s : c.skins)
							{
								if (ImGui::Button(s.second.c_str()))
								{
									std::string body = R"({"key":"backgroundSkinId","value":)" + s.first + "}";
									std::string result = http->Request("POST", "https://127.0.0.1/lol-summoner/v1/current-summoner/summoner-profile/", body, auth->leagueHeader, "", "", auth->leaguePort);
								}
							}
							ImGui::TreePop();
						}
					}
				}
			}

			ImGui::EndTabItem();
		}
	}
};