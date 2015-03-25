#pragma once
#ifndef _LUA_HOOKER_CLUAH_HPP_
#define _LUA_HOOKER_CLUAH_HPP_

#include "includes.hpp"
#include "Lua/lua.hpp"

class CLuaH
{
	bool inited;
	lua_State *luaState;
	static injector::memory_pointer_raw retHookRunPtr;

public:
	struct luaScript
	{
		lua_State											*luaState;
		std::map < std::string, std::string >				savedValues;
		std::string											filePath;
		std::string											fileName;
	};

	std::map < std::string, std::map<std::string, luaScript> > files; /* std::map<pathForScripts, std::map<scriptName, scriptData>> */

	static CLuaH				&Lua();

	inline bool					initSuccess(){ return inited; }
	bool						loadFiles(const std::string &path);
	void						runScripts();

	/* hook functions */
	static void					hook_runScripts();
	
	CLuaH();
};


#endif