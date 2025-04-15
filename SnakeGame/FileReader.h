#pragma once

#include <vector>
#include <string>


class FileReader
{
private:
	FileReader() = delete;
public:
	static bool ReadFile(const char* strFile, std::vector<std::string>& arData);
};