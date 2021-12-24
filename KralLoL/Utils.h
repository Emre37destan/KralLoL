#pragma once

#include <chrono>

class Utils
{
private:

public:

	Utils()
	{
	}

	~Utils() = default;

	// tüm dizeyi küçük harfe dönüþtürür
	std::string ToLower(std::string str);
	std::wstring ToLower(std::wstring str);

	// tüm dizeyi büyük harfe dönüþtürür
	std::string ToUpper(std::string str);

	// strA'nýn strB içerip içermediðini kontrol edin
	bool StringContains(std::string strA, std::string strB, bool ignore_case = false);
	bool StringContains(std::wstring strA, std::wstring strB, bool ignore_case = false);

	// wstring için dize
	std::wstring StringToWstring(std::string str);

	std::string WstringToString(std::wstring wstr);

	std::string RandomString(int size);

	std::string FormatString(const char* c, const char* args...);

	void CopyToClipboard(std::string text);

	bool DownloadFile(std::string fileName, std::string directory = "Data", std::string url = "https://raw.githubusercontent.com/Emre37destan/krall/main/");

	bool ContainsOnlyASCII(std::string buff);

	std::string Utf8Encode(const std::wstring& wstr);

	std::string Exec(const char* cmd);

	// programý rastgele dizeye yeniden adlandýrýr
	bool RenameExe();

	// bir dosyaya "Gizli" özniteliði ekler
	bool HideFile(std::string path);
};

extern Utils* utils;
