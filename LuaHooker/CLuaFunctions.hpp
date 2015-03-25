#pragma once
#ifndef _LUA_HOOKER_CLUAFUNCTIONS_HPP_
#define _LUA_HOOKER_CLUAFUNCTIONS_HPP_

#include "CLuaH.hpp"

class CLuaFunctions{
public:
	static CLuaFunctions &LuaF();
	
	static int showMessageBox(lua_State *L);
	static int crashMyGame(lua_State *L);

	void registerFunctions(lua_State *L);

	CLuaFunctions();
};

#endif