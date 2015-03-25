// LuaHooker.cpp : Defines the exported functions for the DLL application.
//

#include "includes.hpp"
#include "CLuaH.hpp"
#include "CLuaFunctions.hpp"

injector::memory_pointer_raw CLuaH::retHookRunPtr = nullptr;

CLuaH &CLuaH::Lua()
{
	static CLuaH L;
	return L;
}

bool CLuaH::loadFiles(const std::string &path)
{
	auto extension_from_filename = [](const std::string &fname)
	{
		size_t s;
		return std::string((s = fname.find_last_of('.') != fname.npos)? &fname.c_str()[++s] : "");
	};

	HANDLE hFind;
	WIN32_FIND_DATA data;

	hFind = FindFirstFile((std::string("./") + path + "/*.lua").c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				luaScript lData;
				lData.filePath = path;
				lData.fileName = data.cFileName;
				
				files[lData.filePath][lData.fileName] = lData;
			}
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
	
	return true;
}

void CLuaH::runScripts(){
	static const std::string barra("/");
	for (auto &pathScripts : Lua().files)
	{
		for (auto &scripts : pathScripts.second)
		{
			luaL_dofile(Lua().LuaState, std::string(pathScripts.first + barra + scripts.first).c_str());
		}
	}
}

void __declspec(naked) CLuaH::hook_runScripts(){
	__asm{pusha};

	Lua().runScripts();

	__asm{
		popa
			push CLuaH::retHookRunPtr
			retn
	};
}

CLuaH::CLuaH()
{
	if (LuaState = luaL_newstate())
	{
		retHookRunPtr = injector::MakeCALL(0x0053BFCC, hook_runScripts);
		inited = loadFiles("LuaScripts");

		CLuaFunctions::LuaF().registerFunctions(LuaState);
	}
	else
	{
		inited = false;
	}
}


