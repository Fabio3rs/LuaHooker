// dllmain.cpp : Defines the entry point for the DLL application.
#include "includes.hpp"
#include "CLuaH.hpp"
#include "CLuaFunctions.hpp"
#include "CLog.h"

void luaInit()
{
	CLog::log() << "Initializing LuaHooker";
	CLuaFunctions::f();
	CLog::log() << (CLuaH::Lua().initSuccess()? "LuaHooker init success" : "LuaHooker failed to init");
}

void *retptr;

void __declspec(naked) hook()
{
	_asm
	{
		pushad
		call luaInit
		popad

		push retptr
		ret
	}
}


bool init()
{
	retptr = injector::MakeCALL(0x008246EC, hook).get();
	return retptr != nullptr;
}

void disableDEP()
{
	HMODULE kernel = GetModuleHandleA("kernel32.dll");

	typedef BOOL(__stdcall *SetProcessDEPPolicy_t)(DWORD dwFlags);
	SetProcessDEPPolicy_t SetProcessDEPPolicy = (SetProcessDEPPolicy_t)GetProcAddress(kernel, "SetProcessDEPPolicy");

	if (SetProcessDEPPolicy) SetProcessDEPPolicy(0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (false) disableDEP();
		return init();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

