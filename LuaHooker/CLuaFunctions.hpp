#pragma once
#ifndef _LUA_HOOKER_CLUAFUNCTIONS_HPP_
#define _LUA_HOOKER_CLUAFUNCTIONS_HPP_

#include "CLuaH.hpp"

class CLuaFunctions{
public:
	static CLuaFunctions &LuaF();

	class LuaParams
	{
		lua_State *L;
		int num_params, ret, stck, fail_bit;

	public:

		LuaParams &operator<<(const std::string &param);
		LuaParams &operator<<(double param);
		LuaParams &operator<<(int param);
		LuaParams &operator<<(bool param);

		LuaParams &operator>>(std::string &param);
		LuaParams &operator>>(double &param);
		LuaParams &operator>>(int &param);
		LuaParams &operator>>(bool &param);

		int rtn();

		bool fail();

		LuaParams(lua_State *state);
	};
	
	static int showMessageBox(lua_State *L);
	static int crashMyGame(lua_State *L);

	void registerFunctions(lua_State *L);

	CLuaFunctions();
};

#endif