// plugins.cpp
// x64dbg plugin

#include <Windows.h>

#include "pluginsdk\_plugins.h"

#ifdef _WIN64
#pragma comment(lib, "pluginsdk\\x64dbg.lib")
#pragma comment(lib, "pluginsdk\\x64bridge.lib")
#else
#pragma comment(lib, "pluginsdk\\x32dbg.lib")
#pragma comment(lib, "pluginsdk\\x32bridge.lib")
#endif	//_WIN64



//-------------------------------------------------------------------
#define plugin_name "YCoreDump"
#define plugin_version 1

// �� �÷����� ��ư
#define MENU_Bar 0
#define MENU_DISASM_Run_Bp 1
#define MENU_DUMP 2
#define MENU_STACK 3

// Handle�� ��������
int g_iPluginHandle;

HWND g_hwndDlg;
int g_hMenu;
int g_hMenuDisasm;
int g_hMenuDump;
int g_hMenuStack;

//-------------------------------------------------------------------
/*
//�÷����� ȣ�� �⺻ ����
extern "C" __declspec(dllexport) bool pluginit(PLUG_INITSTRUCT* initStruct);
extern "C" __declspec(dllexport) void plugsetup(PLUG_SETUPSTRUCT* setupStruct);
extern "C" __declspec(dllexport) bool plugstop(void);

//�޴�â�� ���� ����
extern "C" __declspec(dllexport) void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info);
*/
//-------------------------------------------------------------------
extern "C" __declspec(dllexport) bool pluginit(PLUG_INITSTRUCT* initStruct)
{
	initStruct->sdkVersion = PLUG_SDKVERSION;
	initStruct->pluginVersion = plugin_version;
	strcpy_s(initStruct->pluginName, 256, plugin_name);
	g_iPluginHandle = initStruct->pluginHandle;

	_plugin_logprintf("[YCoreDump] Start!\n pluginHandle : %d\n", g_iPluginHandle);

	return true;
}

extern "C" __declspec(dllexport) void plugsetup(PLUG_SETUPSTRUCT* setupStruct)
{
	g_hwndDlg = setupStruct->hwndDlg;
	g_hMenu = setupStruct->hMenu;
	g_hMenuDisasm = setupStruct->hMenuDisasm;
	g_hMenuDump = setupStruct->hMenuDump;
	g_hMenuStack = setupStruct->hMenuStack;

	// ���� �޴�
	_plugin_menuaddentry(g_hMenu, MENU_Bar, "Menu bar");

	// �𽺾���� â �޴�
	_plugin_menuaddentry(g_hMenuDisasm, MENU_DISASM_Run_Bp, "YCoreDumper");


	// ���� â �޴�
	_plugin_menuaddentry(g_hMenuDump, MENU_DUMP, "Menu Dump Test 1");


	// ���� â �޴�
	_plugin_menuaddentry(g_hMenuStack, MENU_STACK, "Menu Stack Test 1");

}
////////////////////////////////////////////////////
static int traceCount = 0;

// Debug�� ���۵Ǿ��� ��� traceCount�� 0���� �ʱ�ȭ
extern "C" __declspec(dllexport) void CBINITDEBUG(CBTYPE cbType, PLUG_CB_INITDEBUG* info)
{
	traceCount = 0;
	_plugin_logprintf("[YCoreDump] debugging of file %s started!\n", (const char*)info->szFileName);
}

//Called when a debugging session ends
extern "C" __declspec(dllexport) void CBSTOPDEBUG(CBTYPE cbType, PLUG_CB_STOPDEBUG* info)
{
	_plugin_logputs("[YCoreDump] debugging stopped!");
}

// Trace�� ���۵� ��� 20000�� �ݺ����� ��� �����ϰ� traceCount�� �ٽ� 0���� �ʱ�ȭ
extern "C" __declspec(dllexport) void CBTRACEEXECUTE(CBTYPE cbType, PLUG_CB_TRACEEXECUTE* info)
{
	if (++traceCount == 5)
	{
		info->stop = true, traceCount = 0;
		MessageBox(g_hwndDlg, L"[YCoreDump] CBTRACEEXECUTE ", L"YCoreDump", MB_ICONINFORMATION | MB_OK);
	}
}
//////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) bool plugstop(void)
{
	_plugin_menuclear(g_hMenu);
	_plugin_menuclear(g_hMenuDisasm);
	_plugin_menuclear(g_hMenuDump);
	_plugin_menuclear(g_hMenuStack);

	return true;
}

//-------------------------------------------------------------------
extern "C" __declspec(dllexport) void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
	SELECTIONDATA sel;
	BASIC_INSTRUCTION_INFO basicinfo;

	duint uiAddr = 0;

	switch (info->hEntry)
	{
	case MENU_Bar:
		while (1) {
			if (CB_PAUSEDEBUG) {// Dbg running üũ
				_plugin_logprintf("[YCoreDump] Dbg is Stop!\n");
				MessageBox(g_hwndDlg, L"[YCoreDump] Dbg is Stop! ", L"YCoreDump", MB_ICONINFORMATION | MB_OK);
				break;
			}
			else
			{
				_plugin_logprintf("[YCoreDump] Wait. Dbg is running.\n");
				Sleep(500);
				break;
			}
		}
		break;

	case MENU_DISASM_Run_Bp:
		GuiSelectionGet(GUI_DISASSEMBLY, &sel);
		_plugin_logprintf("[YCoreDump] Disasm [ START : 0x%p - END : 0x%p ]\n", sel.start, sel.end);
		DbgCmdExec("bp VirtualProtect");

		if (!DbgCmdExec("run"))
		{
			MessageBox(g_hwndDlg, L"[YCoreDump] command run. ", L"YCoreDump", MB_ICONINFORMATION | MB_OK);
		}
		else // run�� �� ���
		{
			_plugin_logprintf("[YCoreDump] Running!\n");
			while (1) {
				if (CB_PAUSEDEBUG) {// Dbg running üũ
									//DbgCmdExec("pause");
					MessageBox(g_hwndDlg, L"[YCoreDump] Dbg is Stop! ", L"YCoreDump", MB_ICONINFORMATION | MB_OK);
					break;
				}
				else {
					_plugin_logprintf("[YCoreDump] Wait. Dbg is running.\n");
					Sleep(500);
				}
			}
		}

		break;

	case MENU_DUMP:
		GuiSelectionGet(GUI_DUMP, &sel);
		_plugin_logprintf("[YCoreDump] Dump [ START : 0x%p - END : 0x%p ]\n", sel.start, sel.end);

		break;

	case MENU_STACK:
		GuiSelectionGet(GUI_STACK, &sel);
		_plugin_logprintf("[YCoreDump] Stack [ START : 0x%p - END : 0x%p ]\n", sel.start, sel.end);

		break;
		}
	}

	//-------------------------------------------------------------------
	// DllMain
	//-------------------------------------------------------------------
	BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpReserved)
	{
		return TRUE;
	}