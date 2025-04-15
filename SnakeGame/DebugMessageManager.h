#pragma once

class DebugMessageManager
{
private:
	DebugMessageManager() = delete;
public:
	static void PrintDebugMesasge_Net(const char* strMessage);
	static void PrintDebugMesasge(const char* strMessage);
	static void PrintDebugMesasge_Sprite(const char* strFile, const char* strMessage);
	static void PrintDebugMesasge_FileReader(const char* strFile, const char* strMessage);
};