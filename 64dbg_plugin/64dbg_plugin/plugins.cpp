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

#define MENU_Bar 0
#define MENU_DISASM_Run_Bp 1
#define MENU_DUMP 2
#define MENU_STACK 3

int g_iPluginHandle;

HWND g_hwndDlg;
int g_hMenu;
int g_hMenuDisasm;
int g_hMenuDump;
int g_hMenuStack;

//-------------------------------------------------------------------
//플러그인 호출 기본 선언
extern "C" __declspec(dllexport) bool pluginit(PLUG_INITSTRUCT* initStruct);
extern "C" __declspec(dllexport) void plugsetup(PLUG_SETUPSTRUCT* setupStruct);
extern "C" __declspec(dllexport) bool plugstop(void);

//메뉴창을 위한 선언
extern "C" __declspec(dllexport) void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info);

//-------------------------------------------------------------------
__declspec(dllexport) bool pluginit(PLUG_INITSTRUCT* initStruct)
{
	initStruct->sdkVersion = PLUG_SDKVERSION;
	initStruct->pluginVersion = plugin_version;
	strcpy_s(initStruct->pluginName, 256, plugin_name);
	g_iPluginHandle = initStruct->pluginHandle;

	_plugin_logprintf("[BasicPlugin] pluginHandle : %d\n", g_iPluginHandle);

	return true;
}

__declspec(dllexport) void plugsetup(PLUG_SETUPSTRUCT* setupStruct)
{
	g_hwndDlg = setupStruct->hwndDlg;
	g_hMenu = setupStruct->hMenu;
	g_hMenuDisasm = setupStruct->hMenuDisasm;
	g_hMenuDump = setupStruct->hMenuDump;
	g_hMenuStack = setupStruct->hMenuStack;

	// 메인 메뉴
	_plugin_menuaddentry(g_hMenu, MENU_Bar, "Menu bar");

	// 디스어셈블 창 메뉴
	_plugin_menuaddentry(g_hMenuDisasm, MENU_DISASM_Run_Bp, "Menu Disasm Test 1");
	

	// 덤프 창 메뉴
	_plugin_menuaddentry(g_hMenuDump, MENU_DUMP, "Menu Dump Test 1");
	

	// 스택 창 메뉴
	_plugin_menuaddentry(g_hMenuStack, MENU_STACK, "Menu Stack Test 1");
	
}

__declspec(dllexport) bool plugstop(void)
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
		MessageBox(g_hwndDlg, L"YCoreDump!!", L"YCoreDump", MB_ICONINFORMATION | MB_OK);

		break;

	case MENU_DISASM_Run_Bp:
		GuiSelectionGet(GUI_DISASSEMBLY, &sel);
		_plugin_logprintf("[YCoreDump] Disasm [ START : 0x%p - END : 0x%p ]\n", sel.start, sel.end);
		DbgCmdExec("bp VirtualProtect");
		DbgCmdExec("run /go/r/g");

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