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


}

CLuaH::CLuaH()
{
	inited = true;
}


