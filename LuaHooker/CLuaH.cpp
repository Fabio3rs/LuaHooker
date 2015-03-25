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

}

CLuaH::CLuaH()
{
	inited = true;
}


