#pragma once
#ifndef _LUA_HOOKER_CLUAH_HPP_
#define _LUA_HOOKER_CLUAH_HPP_

#include "includes.hpp"
#include "Lua/lua.hpp"

class CLuaH
{
	bool inited;
	lua_State *L;

public:
	struct luaScript
	{
		std::map < std::string, std::string >				savedValues;
		std::string											filePath;
		std::string											fileName;
	};

	std::map < std::string, std::map<std::string, luaScript> > files; /* std::map<pathForScripts, std::map<scriptName, scriptData>> */

	static CLuaH				&Lua();

	inline bool					initSuccess(){ return inited; }
	bool						loadFiles(const std::string &path);

	CLuaH();
};


#endif