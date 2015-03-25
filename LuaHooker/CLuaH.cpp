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

	hFind = FindFirstFile("c:\\*.*", &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0
				&& extension_from_filename(data.cFileName) == "lua")
			{

			}
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}

}

CLuaH::CLuaH()
{
	inited = true;
}


