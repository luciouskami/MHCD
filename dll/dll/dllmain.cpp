// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

DWORD JGetHeroLevel = (DWORD)0x3c7950;
DWORD JGetPlayerName = (DWORD)0x3c19e0;
DWORD JGetPlayerNameAdd1 = (DWORD)0x3bdf50;
DWORD JGetPlayerNameAdd2 = (DWORD)0x40bb60;
DWORD JIsUnitVisible = (DWORD)0x3c8570;
DWORD JIsUnitSelected = (DWORD)0x3c8880;
DWORD JPlayer = (DWORD)0x3bc5b0;
DWORD JGamePoint = (DWORD)0xacd44c;
DWORD HookUnitAddress = (DWORD)0x42b0ed;
DWORD HookTlsSetAddress = (DWORD)0x6d9ff8;

HMODULE GameDllBase;

char HookUnitBytes[5] = {0xe9};
char HookTlsSetBytes[5] = {0xe9};
char HookSendBytes[5] = {0xe8};

bool InDotA = true;
bool State = 0;
bool hasgotplayer;
int ProcessId, ThreadId, ThreadTlsV, ThreadTlsT;

int s_unit[500];
int s_player[20];
char s_playername[20][100];
int s_selecttime[500][20];
int s_visibletime[500][20];
HWND htosend, hself;

void SendString(char * buf)
{
	int k;
	COPYDATASTRUCT Dta ;
	Dta.dwData = 0x33;
	Dta.cbData = strlen(buf) + 1;
	Dta.lpData = (LPVOID)buf;
	k = SendMessage(htosend, WM_COPYDATA, (WPARAM)hself,(LPARAM)&Dta);
}

void ConvertUTF8ToANSI(char * strUTF8, char * strANSI)
{
	int nLen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCTSTR)strUTF8, -1, NULL, 0);
	wchar_t * wszANSI = new wchar_t[nLen + 1];
	memset(wszANSI, 0, nLen * 2 + 2);
	nLen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCTSTR)strUTF8, -1, wszANSI, nLen);
	nLen = WideCharToMultiByte(CP_ACP, 0, wszANSI, -1, NULL, 0, NULL, NULL);
	char * szANSI = new char [nLen + 1];
	memset(szANSI, 0, nLen + 1);
	WideCharToMultiByte(CP_ACP, 0, wszANSI, -1, szANSI, nLen, NULL, NULL);
	memcpy(strANSI, szANSI, nLen + 1);
	delete [] wszANSI;
	delete [] szANSI;
}

int Check22(int tunit, int tplayer)
{
	int k = 0;
	char tmp[100];
	__try
	{

		__asm
		{
			pushad
			push tplayer
			push tunit
			mov esi, 0x6F95A193
			call dword ptr [JIsUnitVisible]
			pop ecx
			pop edx
			mov k, eax
			popad
		}
		
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		sprintf(tmp, "IsUnitVisible error : player : 0x%x unit : 0x%x", tplayer, tunit);
		OutputDebugString(tmp);
		ExitThread(0);
		return 0;
	}
	return k;
}



int Check21(int tunit, int tplayer)
{
	int k = 0;
	char tmp[100];
	__try
	{
		__asm
		{
			pushad
			push tplayer
			push tunit
			mov esi, 0x6F95A193
			call dword ptr [JIsUnitSelected]
			pop ecx
			pop edx
			mov k, eax
			popad
		}

		
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		sprintf(tmp, "IsUnitSelected error : player : 0x%x unit : 0x%x", tplayer, tunit);
		OutputDebugString(tmp);
		ExitThread(0);
		return 0;
	}
	return k;
}

int GetAllPlayer2(int k)
{
	int j = 0;
	__try
	{
		__asm
		{
			pushad
			push k
			call dword ptr [JPlayer]
			pop ecx
			mov j, eax
			popad
		}
			
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}
	if(j >= 0x100000)
		return j;
	return 0;
}

void GetAllPlayer()
{
	int i, j, k, OldTls;
	char tmp[100];
	memset(s_player, 0, sizeof(s_player));
	for(i = 0 ; i <= 11 ; i++)
	{
		OldTls = (int)TlsGetValue(ThreadTlsT);
		TlsSetValue(ThreadTlsT, (LPVOID)ThreadTlsV);
		k = GetAllPlayer2(i);
		TlsSetValue(ThreadTlsT, (LPVOID)OldTls);

		if(k > 0)
		{
			s_player[++s_player[0]] = k;
			//if(k <= 0x100000 || k >= 0x100020)s_player[s_player[0]] = 0;
			sprintf(tmp, "PlayerID %d", k);
			OutputDebugString(tmp);
		}

	}
}

void GetPlayerName(int tplayer, char * dst, int maxLen)
{
	char * res;
	__try
	{
		__asm
		{
			mov ecx, tplayer
			call dword ptr [JGetPlayerNameAdd1]
			test eax, eax
			jnz A
			jmp B

			A:
			push 1
			mov ecx, eax
			call dword ptr [JGetPlayerNameAdd2]

			B:
			mov res, eax
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		res = 0;
	}
	if(!res)
	{
		memset(dst, 0, maxLen);
		return;
	}
	memcpy(dst, res, strlen(res) + 1);
}


void GetAllPlayerName()
{
	int i;
	memset(s_playername, 0, sizeof(s_playername));
	for(i = 1 ; i <= 12 ; i++)
	{
		GetPlayerName(s_player[i], s_playername[i], 100);
		ConvertUTF8ToANSI(s_playername[i], s_playername[i]);
		OutputDebugString(s_playername[i]);
	}
}

int GetUnit2(int k)
{
	int a = 0;
	__try
	{
		__asm
		{
			pushad
			push k
			call dword ptr [JGetHeroLevel]
			mov a, eax
			pop eax
			popad
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		OutputDebugString("GetHeroLevel error");
		return -1;
	}
	if(a)return 1;
	return 0;
}

void GetUnit(int k)
{
	int i, a, OldTls;
	char tmp[100];
	
	if(k < 0x100000 || k > 0x200000)return;

	OldTls = (int)TlsGetValue(ThreadTlsT);
	TlsSetValue(ThreadTlsT, (LPVOID)ThreadTlsV);
		
	if(GetUnit2(k) == 1)
	{
		for(i = 1 ; i <= s_unit[0] ; i++)
			if(s_unit[i] == k) break;
		if(i > s_unit[0])
		{
			s_unit[++s_unit[0]] = k;
			sprintf(tmp, "得到英雄ID : %d", k);
			if(!hasgotplayer)
			{
				GetAllPlayer();
				GetAllPlayerName();
				hasgotplayer = true;
			}
			SendString(tmp);
			OutputDebugString(tmp);
		}
	}
	TlsSetValue(ThreadTlsT, (LPVOID)OldTls);
}


int WINAPI CheckThread(LPVOID xx)
{
	int i, j, tunit, tplayer, k, k1, k2, nowtime, OldTls;
	char tmp[100];
	int * x;

	x = (int *)JGamePoint;

		/*if(*x)
			OutputDebugString("InGame");
		else
			OutputDebugString("OutOfGame");*/

	/*if(!State && *x)
	{
		s_unit.empty();
		GetAllPlayer();
		State = true;
		return;
	}*/
	if(!*x)
	{
		State = false;
		return 0;
	}
	if(!State)return 0;
	
	TlsSetValue(ThreadTlsT, (LPVOID)ThreadTlsV);
	
	for(j = 1 ; j <= s_player[0]  ; j++)
	{
		for (i = 1 ; i <= s_unit[0]; i++)
		{
			k = 0;
			tunit = s_unit[i];
			tplayer = s_player[j];
			//OldTls = (int)TlsGetValue(ThreadTlsT);
			
			k1 = Check21(tunit, tplayer);
			k2 = Check22(tunit, tplayer);
			k = k1 && !k2;
			//TlsSetValue(ThreadTlsT, (LPVOID)OldTls);
			nowtime = GetTickCount();
			if(k && nowtime - s_selecttime[i][j] > 1000  && nowtime - s_visibletime[i][j] > 1000)
			{
				sprintf(tmp, "玩家编号：%d 玩家名：%s 选中不可见单位：%d 时间：%d", j, s_playername[j], tunit, nowtime);
				SendString(tmp);
				OutputDebugString(tmp);
			}
			if(k1)
				s_selecttime[i][j] = nowtime;
			if(k2)
				s_visibletime[i][j] = nowtime;
		}
	}
	return 0;
}

void Check()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CheckThread, (LPVOID)0, 0, 0);
}

void GetThreadTls(int j, int k)
{
	char tmp[100];
	if(GetCurrentThreadId() == ThreadId && k)
	{
		if(ThreadTlsV != k || ThreadTlsT != j)
		{
			ThreadTlsV = k;
			ThreadTlsT = j;
			sprintf(tmp, "GetMainThreadTls : 0x%x Type: 0x%x", k, j);
			OutputDebugString(tmp);
		}
	}
}

__declspec(naked) void HookUnit()
{
	__asm
	{
		pushad
		push edx
		call GetUnit
		pop edx
		popad
		pop esi
		retn 4
	}
}


__declspec(naked) void HookSend()
{
	__asm
	{
		pushad
		call Check
		popad

		sub edx, 1
		and eax, edx
		ret
		
	}
}

__declspec(naked) void HookTlsSet()
{
	__asm
	{
		pushad
		push edx
		push ecx
		call GetThreadTls
		pop eax
		pop eax
		popad

		ret
	}
}

int WINAPI Watcher(LPVOID k)
{
	int * x;
	while(1)
	{
		Sleep(100);
		x = (int *)JGamePoint;


		if(!State && *x)
		{
			memset(s_unit, 0, sizeof(s_unit));
			memset(s_selecttime, 0, sizeof(s_selecttime));
			memset(s_visibletime, 0, sizeof(s_visibletime));
//			GetAllPlayer();
			State = true;
			hasgotplayer = false;
			SendString("New Game");
			OutputDebugString("New Game");
			continue;
		}
		if(!*x)
		{
			State = false;
			continue;
		}
		Check();
		
	}
	return 0;
}

	

void DllLoad()
{
	char tm[100];

	hself = FindWindow(NULL, "Warcraft III");
	htosend = FindWindow(NULL, "MapHackClickDetector");
	GameDllBase = GetModuleHandle("game.dll");
	JGetHeroLevel += (DWORD)GameDllBase;
	JGetPlayerName += (DWORD)GameDllBase;
	JGetPlayerNameAdd1 += (DWORD)GameDllBase;
	JGetPlayerNameAdd2 += (DWORD)GameDllBase;
	JIsUnitVisible += (DWORD)GameDllBase;
	JIsUnitSelected += (DWORD)GameDllBase;
	JPlayer += (DWORD)GameDllBase;
	JGamePoint += (DWORD)GameDllBase;
	HookUnitAddress += (DWORD)GameDllBase;
	HookTlsSetAddress += (DWORD)GameDllBase;


	int tmp = (DWORD)HookUnit - (DWORD)HookUnitAddress - 5;
	memcpy(HookUnitBytes + 1, (void *)&tmp, 4);
	WriteProcessMemory((HANDLE)-1, (LPVOID)HookUnitAddress, (LPCVOID)HookUnitBytes, 5, 0);

	tmp = (DWORD)HookTlsSet - (DWORD)HookTlsSetAddress - 5;
	memcpy(HookTlsSetBytes + 1, (void *)&tmp, 4);
	WriteProcessMemory((HANDLE)-1, (LPVOID)HookTlsSetAddress, (LPCVOID)HookTlsSetBytes, 5, 0);

	ProcessId = GetCurrentProcessId();
	THREADENTRY32 te32 = {sizeof(te32)};
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
	if( Thread32First( hThreadSnap, &te32) )
	{
		do
		{
			if(ProcessId == te32.th32OwnerProcessID )
			{
				ThreadId = te32.th32ThreadID;
				break;
			}
		}while( Thread32Next( hThreadSnap, &te32) );
	}

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Watcher, (LPVOID)0, 0, 0);
	

	SendString("已启动");
	sprintf(tm, "MainThreadId : %d", ThreadId);
	OutputDebugString(tm);
	OutputDebugString("DllLoad");
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DllLoad();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

