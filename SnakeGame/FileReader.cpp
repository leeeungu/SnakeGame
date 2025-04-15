#include "FileReader.h"
#include "DebugMessageManager.h"
#include <iostream>
#include <fstream>

bool FileReader::ReadFile(const char* strFile, std::vector<std::string>& arData)
{
	using namespace std;
	ifstream readFile;
	readFile.open(strFile);
	if (!readFile.is_open())
	{
		DebugMessageManager::PrintDebugMesasge_FileReader(strFile, "Can't Open / Add File");
		return false;
	}
	int nIndex{};
	int nSize = (int)arData.size();
	while (nIndex < nSize && getline(readFile, arData[nIndex]))
	{
		nIndex++;
	}

	bool bResult = (nIndex == nSize) && readFile.eof();
	if (!bResult)
	{
		DebugMessageManager::PrintDebugMesasge_FileReader(strFile,"Incorrect Data / Please follow the file manual");
	}
	readFile.close();
	return bResult;
}
