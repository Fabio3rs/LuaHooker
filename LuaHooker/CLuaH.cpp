// LuaHooker.cpp : Defines the exported functions for the DLL application.
//

#include "includes.hpp"
#include "CLuaH.hpp"

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

CLuaH::CLuaH()
{
	if (L = luaL_newstate())
	{
		inited = loadFiles("LuaScripts");
	}
	else
	{
		inited = false;
	}
}


