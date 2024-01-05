#include <mq/Plugin.h>

#include <iostream>

PreSetup("MQ2WinTitle");
PLUGIN_VERSION(1.0);

#define OTHERSTRING " - ${MacroQuest.Server}" 
#define INGAMESTRING " - ${Me.Name} ${Me.Surname} (${MacroQuest.Server}) - ${Zone.Name}"
#define ZONINGSTRING " - ${Me.Name} ${Me.Surname} (${MacroQuest.Server}) - Zoning..."
#define TELLSTRING " - * [${MacroQuest.LastTell}] ${Me.Name} ${Me.Surname} (${MacroQuest.Server}) - ${Zone.Name}"

bool WTDEBUG = false, PETDEBUG = false, TellRecv = false, IsBackGround = false, Zoning = false,
	Init = false, ShowHotKey = true, KeepOriginalTitle = false, bBeenInGame = false;
char szLoginName[MAX_STRING] = { 0 }, zCharName[MAX_STRING] = { 0 }, szServerName[MAX_STRING] = { 0 }, szWindowTitle[MAX_STRING] = { 0 },
	g_ZoningString[MAX_STRING] = { 0 }, g_OtherString[MAX_STRING] = { 0 }, g_InGameString[MAX_STRING] = { 0 }, g_TellString[MAX_STRING] = { 0 },
	g_HotKey[MAX_STRING] = { 0 }, g_OriginalTitle[MAX_STRING] = { 0 }, g_LastTitle[MAX_STRING] = { 0 };
ULONGLONG TimeStamp = 0LL, InGameTimeStamp = 0LL;
HWND EQWnd = nullptr;

void WTDebug(const char* Cmd)
{
	char zParm[MAX_STRING];
	GetArg(zParm, Cmd, 1);
	if (zParm[0] == 0)
		WTDEBUG = !WTDEBUG;
	else if (!_strnicmp(zParm, "on", 2))
		WTDEBUG = true;
	else if (!_strnicmp(zParm, "off", 2))
		WTDEBUG = false;
	else
		WTDEBUG = !WTDEBUG;
	WriteChatf("\ar%s\ax::\amDEBUGGING is now %s\ax.", mqplugin::PluginName, WTDEBUG ? "\aoON" : "\agOFF");
	WritePrivateProfileString("Settings", "Debug", WTDEBUG ? "on" : "off", INIFileName);
}

void PetDebug(const char* Cmd)
{
	char zParm[MAX_STRING];
	GetArg(zParm, Cmd, 1);
	if (zParm[0] == 0)
		PETDEBUG = !PETDEBUG;
	else if (!_strnicmp(zParm, "on", 2))
		PETDEBUG = true;
	else if (!_strnicmp(zParm, "off", 2))
		PETDEBUG = false;
	else
		PETDEBUG = !PETDEBUG;
	WriteChatf("\ar%s\ax::\amOnIncomingChat pet tell debugging is now %s\ax.", mqplugin::PluginName, PETDEBUG ? "\aoON" : "\agOFF");
	WritePrivateProfileString("Settings", "PetDebug", PETDEBUG ? "on" : "off", INIFileName);
}

void SetWinTitle(int GameState)
{
	char szTemp[MAX_STRING] = { 0 };
	PlayerClient* pPlayer = pLocalPlayer;

	if (GameState != 2000)
	{
		if (!Init) 
		{
			if (!GetPrivateProfileString("Settings", "InGame", NULL, g_InGameString, MAX_STRING, INIFileName))
				WritePrivateProfileString("Settings", "InGame", INGAMESTRING, INIFileName);
			if (!GetPrivateProfileString("Settings", "Tell", NULL, g_TellString, MAX_STRING, INIFileName))
				WritePrivateProfileString("Settings", "Tell", TELLSTRING, INIFileName);
			if (!GetPrivateProfileString("Settings", "Zoning", NULL, g_ZoningString, MAX_STRING, INIFileName))
				WritePrivateProfileString("Settings", "Zoning", ZONINGSTRING, INIFileName);
			if (!GetPrivateProfileString("Settings", "Other", NULL, g_OtherString, MAX_STRING, INIFileName))
				WritePrivateProfileString("Settings", "Other", OTHERSTRING, INIFileName);
			if (!GetPrivateProfileString("Settings", "ShowHotKey", NULL, szTemp, MAX_STRING, INIFileName))
				WritePrivateProfileString("Settings", "ShowHotKey", "on", INIFileName);
			GetPrivateProfileString("Settings", "InGame", INGAMESTRING, g_InGameString, MAX_STRING, INIFileName);
			GetPrivateProfileString("Settings", "Tell", TELLSTRING, g_TellString, MAX_STRING, INIFileName);
			GetPrivateProfileString("Settings", "Zoning", ZONINGSTRING, g_ZoningString, MAX_STRING, INIFileName);
			GetPrivateProfileString("Settings", "Other", OTHERSTRING, g_OtherString, MAX_STRING, INIFileName);
			GetPrivateProfileString("Settings", "ShowHotKey", "on", szTemp, MAX_STRING, INIFileName);
			if (!_stricmp(szTemp, "on"))
				ShowHotKey = true;
			else
				ShowHotKey = false;
		}
	}

	HWND hwnd = EQWnd ? EQWnd : GetEQWindowHandle();

	if (!hwnd) 
	{
		if (WTDEBUG) 
		{
			DebugSpewAlways("%s::!hwnd", mqplugin::PluginName);
			WriteChatf("%s::!hwnd", mqplugin::PluginName);
		}
		return;
	}
	static bool ShownOnce = false, InitialSetupDone = false, InitialHKDone = false;
	if (!InitialSetupDone && GameState == GAMESTATE_INGAME) 
	{
		::GetWindowTextA(hwnd, g_OriginalTitle, MAX_STRING - 1);
		if (g_OriginalTitle[0]) 
		{
			InitialSetupDone = true;
			int pos = ci_find_substr(g_OriginalTitle, " (hotkey:");
			if (pos != -1)
			{
				pos++;
				const int pos2 = ci_find_substr(&g_OriginalTitle[pos], " (hotkey:");
				if (pos2 != -1)
				{
					for (unsigned int x = static_cast<int>(strlen(g_OriginalTitle)) - 1; x > 0; x--)
					{
						if (g_OriginalTitle[x] == '(' && g_OriginalTitle[x - 1] == ' ')
						{
							g_OriginalTitle[x - 1] = 0;
							break;
						}
					}
				}
			}
		}
		else if (WTDEBUG) 
		{
			DebugSpewAlways("%s::GetWindowText() failed.", mqplugin::PluginName);
			WriteChatf("%s::GetWindowText() failed.", mqplugin::PluginName);
		}
		if (WTDEBUG && !ShownOnce) 
		{
			ShownOnce = true;
			DebugSpewAlways("%s::g_OriginalTitle='%s'", mqplugin::PluginName, g_OriginalTitle);
			WriteChatf("%s::g_OriginalTitle='%s'", mqplugin::PluginName, g_OriginalTitle);
		}
	}
	// Verify hotkey is set, might not want to verify g_HotKey[0], and just let it update, but for now, will check it - htw
	if (ShowHotKey && hwnd && !g_HotKey[0]) 
	{
		char szTempTitle[MAX_STRING] = { 0 };
		::GetWindowTextA(hwnd, szTempTitle, MAX_STRING - 1);
		if (szTempTitle[0])
		{
			const int pos = ci_find_substr(szTempTitle, "(Hotkey: ");
			if (pos != -1)
			{
				strcpy_s(g_HotKey, &szTempTitle[pos]);
				for (unsigned int x = 0; x < strlen(g_HotKey); x++)
				{
					if (g_HotKey[x] == ')' && g_HotKey[x + 1])
					{
						g_HotKey[x + 1] = 0;
						break;
					}
				}
			}
		}
	}
	if (!InitialHKDone && GameState == GAMESTATE_INGAME) 
	{
		if (strlen(g_OriginalTitle) > 1 && ci_find_substr(g_OriginalTitle, "(Hotkey: ") != -1 && ci_find_substr(g_OriginalTitle, "+") != -1 && ci_find_substr(g_OriginalTitle, ")") != -1) 
		{
			InitialHKDone = true;
			if (ShowHotKey) 
			{
				const int pos = ci_find_substr(g_OriginalTitle, "(Hotkey: ");
				if (pos != -1)
				{
					strcpy_s(g_HotKey, &g_OriginalTitle[pos]);
					for (unsigned int x = 0; x < strlen(g_HotKey); x++)
					{
						if (g_HotKey[x] == ')' && g_HotKey[x + 1])
						{
							g_HotKey[x + 1] = 0;
							break;
						}
					}
				}
				if (WTDEBUG) 
				{
					DebugSpewAlways("%s::g_HotKey='%s'", mqplugin::PluginName, g_HotKey);
					WriteChatf("%s::g_HotKey='%s'", mqplugin::PluginName, g_HotKey);
				}
			}
		}
	}
	if (!InitialSetupDone)
		return;
	if (GameState == GAMESTATE_INGAME) 
	{
		if (Zoning) 
		{
			if (KeepOriginalTitle)
				sprintf_s(szWindowTitle, "%s %s", g_OriginalTitle, g_ZoningString);
			else
				sprintf_s(szWindowTitle, "%s", g_ZoningString);
			if (pPlayer)
				ParseMacroParameter(szWindowTitle);
			else
				strcpy_s(szWindowTitle, g_OriginalTitle);
		}
		else 
		{
			if (TellRecv && IsBackGround && pEverQuestInfo->LastTellFromList[0][0]) 
			{
				if (KeepOriginalTitle)
					sprintf_s(szWindowTitle, "%s %s", g_OriginalTitle, g_TellString);
				else
					sprintf_s(szWindowTitle, "%s", g_TellString);
				if (pPlayer)
					ParseMacroParameter(szWindowTitle);
				else
					strcpy_s(szWindowTitle, g_OriginalTitle);
			}
			else 
			{
				TellRecv = false;  // reset TellRecv if toggled to foreground, or in foreground
				if (KeepOriginalTitle)
					sprintf_s(szWindowTitle, "%s %s", g_OriginalTitle, g_InGameString);
				else
					sprintf_s(szWindowTitle, "%s", g_InGameString);
				if (pPlayer)
					ParseMacroParameter(szWindowTitle);
				else
					strcpy_s(szWindowTitle, g_OriginalTitle);
			}
		}
	}
	else if (GameState == GAMESTATE_CHARSELECT) 
	{
		if (Zoning) 
		{
			if (KeepOriginalTitle)
				sprintf_s(szWindowTitle, "%s %s", g_OriginalTitle, g_ZoningString);
			else
				sprintf_s(szWindowTitle, "%s", g_ZoningString);
			if (pPlayer)
				ParseMacroParameter(szWindowTitle);
			else
				strcpy_s(szWindowTitle, g_OriginalTitle);
		}
		else 
		{
			if (KeepOriginalTitle)
				sprintf_s(szWindowTitle, "%s %s", g_OriginalTitle, g_OtherString);
			else
				sprintf_s(szWindowTitle, "%s", g_OtherString);
			if (pPlayer)
				ParseMacroParameter(szWindowTitle);
			else
				strcpy_s(szWindowTitle, g_OriginalTitle);
		}
	}
	else if (GameState == 2000) 
	{
		if (g_HotKey[0]) 
		{
			char szTmp1[MAX_STRING] = { 0 };
			for (unsigned int x = 0; x < strlen(g_OriginalTitle); x++)
			{
				if (g_OriginalTitle[x] == ')' && g_OriginalTitle[x + 1])
				{
					g_OriginalTitle[x + 1] = 0;
					break;
				}
			}
			if (ci_find_substr(g_OriginalTitle, "(Hotkey:") == -1)
				sprintf_s(szTmp1, "%s %s", g_OriginalTitle, g_HotKey);
			else
				sprintf_s(szTmp1, "%s", g_OriginalTitle);
			::SetWindowTextA(hwnd, szTmp1);
		}
		else
		{
			for (unsigned int x = 0; x < strlen(g_OriginalTitle); x++)
			{
				if (g_OriginalTitle[x] == ')' && g_OriginalTitle[x + 1])
				{
					g_OriginalTitle[x + 1] = 0;
					break;
				}
			}
			::SetWindowTextA(hwnd, g_OriginalTitle);
		}
		return;
	}
	else if (g_OriginalTitle[0]) 
	{
		const char* szLogin = GetLoginName();
		if (!szLogin) 
		{
			strcpy_s(szLoginName, "");
			if (KeepOriginalTitle)
				sprintf_s(szWindowTitle, "%s   EverQuest", g_OriginalTitle);
			else
				sprintf_s(szWindowTitle, "EverQuest");
		}
		else 
		{
			strcpy_s(szLoginName, szLogin);
			if (KeepOriginalTitle)
				sprintf_s(szWindowTitle, "%s   EverQuest (%s)", g_OriginalTitle, szLoginName);
			else
				sprintf_s(szWindowTitle, "EverQuest (%s)", szLoginName);
		}
	}
	if (ShowHotKey && g_HotKey[0])
		sprintf_s(szTemp, "%s %s", szWindowTitle, g_HotKey);
	else
		strcpy_s(szTemp, szWindowTitle);
	strcpy_s(g_LastTitle, szTemp);
	::SetWindowTextA(hwnd, szTemp);
	// this will spam, since we're constantly calling this function & not skipping due to wineq2's bs
	/*
	if(WTDEBUG) 
	{
		DebugSpewAlways("%s::SetWindowText(0x%X, \"%s\")", mqplugin::PluginName, hwnd, szTemp);
		WriteChatf("%s::SetWindowText(0x%X, \"%s\")", mqplugin::PluginName, hwnd, szTemp);
	}
	*/
}

void ReloadConfig()
{
	char szTemp[MAX_STRING] = { 0 };
	GetPrivateProfileString("Settings", "InGame", INGAMESTRING, g_InGameString, MAX_STRING, INIFileName);
	GetPrivateProfileString("Settings", "Tell", TELLSTRING, g_TellString, MAX_STRING, INIFileName);
	GetPrivateProfileString("Settings", "Zoning", ZONINGSTRING, g_ZoningString, MAX_STRING, INIFileName);
	GetPrivateProfileString("Settings", "Other", OTHERSTRING, g_OtherString, MAX_STRING, INIFileName);
	GetPrivateProfileString("Settings", "ShowHotKey", "on", szTemp, MAX_STRING, INIFileName);
	if (!_stricmp(szTemp, "on"))
		ShowHotKey = true;
	else
		ShowHotKey = false;
	WriteChatf("\at%s \ay(\agv%1.2f\ay): \atBrought to you by \arhtw \ax- \agConfiguration reloaded.", mqplugin::PluginName, MQ2Version);
	if (GetGameState() == GAMESTATE_CHARSELECT || GetGameState() == GAMESTATE_INGAME)
		SetWinTitle(GetGameState());
	else
		SetWinTitle(1000);
}

void FixTitleBar(const char* szState)
{
	char zParm[MAX_STRING];
	GetArg(zParm, szState, 1);
	SetWinTitle(GetIntFromString(zParm, 0));
}

void WTHelp()
{
	WriteChatf("");
	WriteChatf("\ayMQ2WinTitle Commands:");
	WriteChatf("");
	WriteChatf("\ay/wintitle reload\ax: \agReload configuration from ini file");
	WriteChatf("\ay/wintitle fixtitle [state]\ax: \agCall function to 'fix' title based on game state");
	WriteChatf("\ay/wintitle debug [off|on]\ax: \agToggle displaying debugging messages to chat, or force on/off");
	WriteChatf("\ay/wintitle petdebug [off|on]\ax: \agToggle displaying pet debugging messages to chat, or force on/off");
	WriteChatf("\ay/wintitle [help] \ax: \agThis help");
	WriteChatf("");
}

void WinTitleCmd(SPAWNINFO* pSpawn, const char* szLine)
{
	char szArg1[MAX_STRING], szArg2[MAX_STRING];
	GetArg(szArg1, szLine, 1);
	if (!_stricmp(szArg1, "load") || !_stricmp(szArg1, "reload")) 
	{
		ReloadConfig();
	}
	else if (!_stricmp(szArg1, "fix") || !_stricmp(szArg1, "fixtitle")) 
	{
		GetArg(szArg2, szLine, 2);
		FixTitleBar(szArg2);
	}
	else if (!_stricmp(szArg1, "debug")) 
	{
		GetArg(szArg2, szLine, 2);
		WTDebug(szArg2);
	}
	else if (!_stricmp(szArg1, "petdebug")) 
	{
		GetArg(szArg2, szLine, 2);
		PetDebug(szArg2);
	}
	else 
	{
		WTHelp();
	}
}

PLUGIN_API void InitializePlugin()
{
	char szTemp[MAX_STRING] = { 0 };
	DebugSpewAlways("Initializing %s", mqplugin::PluginName);

	AddCommand("/wintitle", WinTitleCmd);

	TellRecv = false;
	IsBackGround = false;
	Zoning = false;
	Init = false;
	if (GetGameState() == GAMESTATE_INGAME)
		bBeenInGame = true;
	else
		bBeenInGame = false;
	if (bBeenInGame)
	{
		if (GetGameState() == GAMESTATE_CHARSELECT || GetGameState() == GAMESTATE_INGAME)
			SetWinTitle(GetGameState());
		else
			SetWinTitle(1000);
	}
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down %s", mqplugin::PluginName);
	RemoveCommand("/wintitle");
	if (bBeenInGame && !gbUnload)
	{
		SetWinTitle(2000);
	}
}

PLUGIN_API void OnPulse()
{
	if (!EQWnd)
		return;
	if (GetTickCount64() - TimeStamp < 50LL)
		return;
	TimeStamp = GetTickCount64();
	if (GetForegroundWindow() != EQWnd) 
	{
		IsBackGround = true;
		if (bBeenInGame)
			SetWinTitle(GetGameState());
	}
	else 
	{
		if (bBeenInGame)
			SetWinTitle(GetGameState());
		IsBackGround = false;
	}
}

PLUGIN_API void OnBeginZone()
{
	Zoning = true;
	if (bBeenInGame)
		SetWinTitle(GetGameState());
}

PLUGIN_API void SetGameState(int GameState)
{
	if (GetGameState() == GAMESTATE_CHARSELECT || GetGameState() == GAMESTATE_INGAME)
	{
		SetWinTitle(GetGameState());
		if (GetGameState() == GAMESTATE_INGAME)
			bBeenInGame = true;
	}
	else
	{
		SetWinTitle(1000);
		bBeenInGame = false;
	}
}

PLUGIN_API void OnEndZone()
{
	DebugSpewAlways("%s::OnEndZone()", mqplugin::PluginName);
	Zoning = false;
	char szCmd[MAX_STRING] = { 0 };
	sprintf_s(szCmd, "/timed 20 /squelch /fixtitlebar %d", GetGameState());
	EzCommand(szCmd);
	SetWinTitle(GetGameState());
}

PLUGIN_API bool OnIncomingChat(const char* tLine, DWORD Color)
{
	if (!IsBackGround)
		return false;

	if (strstr(tLine, "tells you, ") || strstr(tLine, "told you, "))
	{
		char Line[MAX_STRING] = { 0 };
		PCHARINFO pCh = GetCharInfo();
		if (pCh->pSpawn && pCh->pSpawn->PetID)
		{
			const size_t L = strlen(tLine);
			unsigned int NewPos = 0;
			unsigned int OldPos = 0;
			bool Initial = true;
			do 
			{
				if (tLine[OldPos] != 0x12) 
				{
					Line[NewPos] = tLine[OldPos];
					NewPos++;
					OldPos++;
				}
				else 
				{
					if (Initial) 
					{
						OldPos += 2;
						Initial = false;
					}
					else
						OldPos++;
				}
			} while (OldPos < L);
			Line[NewPos] = 0;
			char* nt;
			char* Sender = strtok_s(Line, " ", &nt);
			SPAWNINFO* MyPet = GetSpawnByID(pCh->pSpawn->PetID);
			if (PETDEBUG && MyPet)
				WriteChatf("\ar%s\ax::\amReceived: '%s'   Found: '%s'", mqplugin::PluginName, Sender, MyPet->DisplayedName);
			if (MyPet && (!strcmp(Sender, MyPet->DisplayedName) || !_strnicmp(pCh->Name, Sender, strlen(pCh->Name))))
			{
				return false;
			}
		}
		TellRecv = true;
		if (bBeenInGame)
			SetWinTitle(GetGameState());
	}
	return false;
}
