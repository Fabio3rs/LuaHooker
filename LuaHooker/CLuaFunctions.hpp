#pragma once
#ifndef _LUA_HOOKER_CLUAFUNCTIONS_HPP_
#define _LUA_HOOKER_CLUAFUNCTIONS_HPP_

#include "CLuaH.hpp"
#include "GTA3RuntimeScript.h"
#include <map>
#include <string>

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
		LuaParams &operator<<(size_t param);

		LuaParams &operator>>(std::string &param);
		LuaParams &operator>>(double &param);
		LuaParams &operator>>(int &param);
		LuaParams &operator>>(void *&param);
		LuaParams &operator>>(bool &param);

		template<class T>
		inline LuaParams &operator>>(T &param){
			if (stck <= num_params){
				param = (T)(lua_tointeger(L, stck));
				++stck;
			}
			else
			{
				fail_bit = 1;
			}

			return *this;
		}

		int getNumParams();
		int rtn();

		bool fail();

		LuaParams(lua_State *state);
	};

	std::map<std::string, GTA3RuntimeScript> GTA3Scripts;
	
	static int showMessageBox(lua_State *L);
	static int writeMemory(lua_State *L);
	static int readMemory(lua_State *L);
	static int showTextBox(lua_State *L);
	static int newGTA3Script(lua_State *L);
	static int GTA3ScriptSize(lua_State *L);
	static int GTA3ScriptPushOpcode(lua_State *L);
	static int runGTA3Script(lua_State *L);

	void registerFunctions(lua_State *L);
	void registerGlobals(lua_State *L);

	CLuaFunctions();
};

#endif