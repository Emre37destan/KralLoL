#pragma once

#include "Definitions.h"
#include "Includes.h"
#include "HTTP.h"
#include "Utils.h"
#include "Auth.h"
#include "Misc.h"
#include "Settings.h"

class SettingsTab
{
public:

	static void ShowFontSelector(const char* label)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* font_current = ImGui::GetFont();
		if (ImGui::BeginCombo(label, font_current->GetDebugName()))
		{
			for (int n = 0; n < io.Fonts->Fonts.Size; n++)
			{
				ImFont* font = io.Fonts->Fonts[n];
				ImGui::PushID((void*)font);
				if (ImGui::Selectable(font->GetDebugName(), font == font_current))
				{
					io.FontDefault = font;
					S.selectedFont = n;
					CSettings::Save();
				}
				ImGui::PopID();
			}
			ImGui::EndCombo();
		}
		//ImGui::SameLine();
	/*	Misc::HelpMarker(
			"- Load additional fonts with io.Fonts->AddFontFromFileTTF().\n"
			"- The font atlas is built when calling io.Fonts->GetTexDataAsXXXX() or io.Fonts->Build().\n"
			"- Read FAQ and docs/FONTS.md for more details.\n"
			"- If you need to add/remove fonts at runtime (e.g. for DPI change), do it before calling NewFrame().");*/
	}

	static void Render()
	{
		static bool once = true;
		if (ImGui::BeginTabItem("Ayarlar"))
		{
			if (once)
			{
				once = false;
				HKEY hkResult;
				if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\LeagueClientUx.exe", 0, KEY_QUERY_VALUE, &hkResult) == ERROR_SUCCESS)
				{
					char buffer[MAX_PATH]{};
					DWORD dwLen = 0;
					LSTATUS regQuery = RegGetValueA(hkResult, 0, "debugger", RRF_RT_REG_SZ, 0, (PVOID)&buffer, &dwLen);
					if (regQuery == ERROR_SUCCESS)
					{
						S.currentDebugger = std::string(buffer, dwLen);
					}
					else if (regQuery == ERROR_FILE_NOT_FOUND)
					{
						S.currentDebugger = "Hicbir Sey";
					}
					else
					{
						S.currentDebugger = "Basarisiz oldu, hata kodu " + regQuery;
					}
					RegCloseKey(hkResult);
				}
				else
					S.currentDebugger = "Hata";
			}
			static std::string result;

			ImGui::Checkbox("Oto-Ad", &S.autoRename);
			ImGui::SameLine();
			Misc::HelpMarker("Baslatildiginda programi otomatik olarak yeniden adlandirir");

			ImGui::Checkbox("Akis Kaniti", &S.streamProof);
			ImGui::SameLine();
			Misc::HelpMarker("Programi kayitlarda ve ekran goruntulerinde gizler");

			ImGui::Checkbox("Hata ayiklayiciyi kaydet IFEO", &S.debugger);
			ImGui::SameLine();
			ImGui::Text(" | Bagli: %s", S.currentDebugger.c_str());

			// Ligle ilgili tüm süreçleri sonlandýrýn,
			// salt okunur ve gizli özellikleri 
			// dosyalardan kaldýrýn ve silin
			if (ImGui::Button("Gunlukleri temizle"))
			{
				result = Misc::ClearLogs();
			}

			static char bufLeaguePath[MAX_PATH];
			std::copy(S.leaguePath.begin(), S.leaguePath.end(), bufLeaguePath);
			ImGui::Text("League Yolu:");
			ImGui::InputText("##leaguePath", bufLeaguePath, MAX_PATH);
			S.leaguePath = bufLeaguePath;

			/*	if (ImGui::Button("Ayarlari Kayit Et"))
				{
					CSettings::Save();
					result = "Kayit Edildi!";
				}*/

			ImGui::Separator();

			ImGui::Text("Yazi tipi:");
			ImGui::SameLine();
			Misc::HelpMarker("Bu programin yazi tipi, Lig'in degil");

			ShowFontSelector("##fontSelector");

			static char buffAddFot[MAX_PATH];
			std::string tempFont = "C:/Windows/Fonts/";
			std::copy(tempFont.begin(), tempFont.end(), buffAddFot);
			ImGui::Text("Yazi tipi yolu:");

			ImGui::InputText("##inputFont", buffAddFot, MAX_PATH);
			ImGui::SameLine();
			if (ImGui::Button("Yazi tipi ekle"))
			{
				std::string temp = buffAddFot;
				if (std::filesystem::exists(temp) && temp.find(".") != std::string::npos)
				{
					if (std::find(S.vFonts.begin(), S.vFonts.end(), temp) == S.vFonts.end())
					{
						S.vFonts.emplace_back(temp);
						S.bAddFont = true;
						CSettings::Save();
						result = temp + " yazi tipi eklendi, secmek icin programi yeniden baslatin";
					}
					else
						result = "Yazi tipi zaten eklendi";
				}
				else
					result = "Yazi tipi bulunamadi";
			}

			if (ImGui::Button("Pencere boyutunu sifirla"))
			{
				S.Window.width = 1207;
				S.Window.height = 633;
				S.Window.resize = true;
				CSettings::Save();
			}
			ImGui::SameLine();
			ImGui::Text(std::format("{0}x{1}", S.Window.width, S.Window.height).c_str());

			ImGui::Separator();
			ImGui::Text("GitHub depo:");
			Misc::TextURL("BANA TIKLA!", "https://github.com/Emre37destan/KralLoL");

			ImGui::Separator();
			ImGui::Text("GitHub Versions:");
			Misc::TextURL("BANA TIKLA!", "https://github.com/Emre37destan/KralLoL/tags");

			ImGui::TextWrapped(result.c_str());

			ImGui::EndTabItem();
		}
		else
			once = true;
	}
};