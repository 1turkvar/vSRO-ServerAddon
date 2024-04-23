#include "AppManager.h"
// Console stuffs
#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
// Utils
#include "Utils/IO/SimpleIni.h"
#include "Utils/Memory/Process.h"
#include "Utils/Memory/hook.h"
#pragma warning(disable:4244) // Bitwise operations warnings
// ASM injection
#include "AsmEdition.h"

/// Static stuffs
bool AppManager::m_IsInitialized;
std::string AppManager::m_IP;

void AppManager::Initialize()
{
	if (!m_IsInitialized)
	{
		m_IsInitialized = true;
		InitConfigFile();
		InitDebugConsole();
		InitPatchValues();
		InitHooks();
	}
}
void AppManager::InitConfigFile()
{
	CSimpleIniA ini;
	// Try to load it or create a new one
	if (ini.LoadFile("vSRO-ShardManager.ini") != SI_Error::SI_OK)
	{
		ini.SetSpaces(false);
		// Memory
		ini.SetValue("Server", "SERVER_IP", "127.0.0.1", "; IP Spoof");
		ini.SetLongValue("Account", "CHARACTERS_MAX", 4, "; Maximum characters per account");
		ini.SetLongValue("Guild", "UNION_LIMIT", 8, "; Union participants limit");
		ini.SetLongValue("Event", "CTF_PARTICIPANS_MIN", 8, "; Minimum participants required to start Capture The Flag");
		ini.SetLongValue("Event", "BA_PARTICIPANS_MIN", 8, "; Minimum participants required to start Battle Arena");
		ini.SetBoolValue("Fix", "PARTY_MATCH_1HOUR_DC", true, "; Fix disconnect when party takes more than 1 hour on party match");
		ini.SetBoolValue("Fix", "GUILD_POINTS", true, "; Prevents negative values on guild points");
		ini.SetBoolValue("Fix", "TREEJOB", true, "; Tree Job Update");
		// App
		ini.SetBoolValue("App", "DEBUG_CONSOLE", true, "; Attach debug console");
		// Save it
		ini.SaveFile("vSRO-ShardManager.ini");
	}
}
void AppManager::InitDebugConsole()
{
	// Load file
	CSimpleIniA ini;
	ini.LoadFile("vSRO-ShardManager.ini");

	// Check if console has been deactivated
	if (ini.GetBoolValue("App", "DEBUG_CONSOLE", true))
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		freopen("CONIN$", "r", stdin);
	}
}
void AppManager::InitHooks()
{
	std::cout << " * Initializing hooks..." << std::endl;

	// Load file
	CSimpleIniA ini;
	ini.LoadFile("vSRO-GameServer.ini");

	// Fix
	if (ini.GetBoolValue("Fix", "GUILD_POINTS", true))
	{
		printf(" - FIX_GUILD_POINTS\r\n");
		// Redirect code flow to DLL
		if (placeHook(0x004364EE, addr_from_this(&AsmEdition::OnDonateGuildPoints)))
		{
			std::cout << "   - OnDonateGuildPoints" << std::endl;
		}
		if (placeHook(0x00438B68, addr_from_this(&AsmEdition::OnDonateGuildPointsErrorCode)))
		{
			std::cout << "   - OnDonateGuildPointsErrorCode" << std::endl;
		}
		if (placeHook(0x0043A9F6, addr_from_this(&AsmEdition::OnDonateGuildPointsErrorMsg)))
		{
			std::cout << "   - OnDonateGuildPointsErrorMsg" << std::endl;
		}

	}
	if (ini.GetBoolValue("Fix", "TREEJOB", true))
	{
		printf(" - TREEJOB\r\n");
		for (int i = 0; i < 197; i++)
			WriteMemoryValue<uint8_t>(0x00459A05 + i, 0x90); // NOP
		if (placeHook(0x00459A00, addr_from_this(&AsmEdition::ASM_TreeJob)))
		{
			std::cout << "   - Tree Job Ranking" << std::endl;
		}
	}


}
void AppManager::InitPatchValues()
{
	std::cout << " * Initializing patches..." << std::endl;

	// Load file
	CSimpleIniA ini;
	ini.LoadFile("vSRO-ShardManager.ini");

	// buffers
	uint8_t byteValue;

	//Server
	auto currentValueIP = ReadMemoryString(0x00686349 + 1);
	if (!currentValueIP.empty())
	{
		m_IP = ini.GetValue("Server", "SERVER_IP", "127.0.0.7");
		auto newValueLen = m_IP.size();
		// Check value it's not empty 
		if (newValueLen != 0)
		{
			printf(" - IP (%s) -> (%s)\r\n", currentValueIP.c_str(), m_IP.c_str());
			replaceOffsetEdit(0x00686349, (uint32_t)m_IP.c_str());
			replaceOffsetEdit(0x0068639D, (uint32_t)m_IP.c_str());
		}
	}

	// Account
	if (ReadMemoryValue<uint8_t>(0x0040F47C + 2, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Account", "CHARACTERS_MAX", 4);
		printf(" - ACCOUNT_CHARACTERS_MAX (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x0040F47C + 2, newValue);
		WriteMemoryValue<uint8_t>(0x00429B83 + 2, newValue);
	}

	// Guild
	if (ReadMemoryValue<uint8_t>(0x00434311 + 1, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Guild", "UNION_LIMIT", 8);
		printf(" - GUILD_UNION_LIMIT (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x00434311 + 1, newValue);
	}

	// Event
	if (ReadMemoryValue<uint8_t>(0x00672891 + 4, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Event", "CTF_PARTICIPANS_MIN", 8);
		printf(" - EVENT_CTF_PARTICIPANS_MIN (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x00672891 + 4, newValue);
	}
	if (ReadMemoryValue<uint8_t>(0x0066A1B0 + 4, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Event", "BA_PARTICIPANS_MIN", 8);
		printf(" - EVENT_BA_PARTICIPANS_MIN (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x0066A1B0 + 4, newValue);
	}

	// Fix
	if (ini.GetBoolValue("Fix", "PARTY_MATCH_1HOUR_DC", true))
	{
		printf(" - FIX_PARTY_MATCH_1HOUR_DC\r\n");
		// Disable/Jump timeout condition
		WriteMemoryValue<uint16_t>(0x0045055C, 0x30EB); // jmp,+30
		for(int i = 0; i < 3; i++)
			WriteMemoryValue<uint8_t>(0x0045055C + 2 + i, 0x90); // nop
	}
}
