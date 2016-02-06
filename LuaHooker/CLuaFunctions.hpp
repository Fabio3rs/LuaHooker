#pragma once
#ifndef _LUA_HOOKER_CLUAFUNCTIONS_HPP_
#define _LUA_HOOKER_CLUAFUNCTIONS_HPP_

#include "CLuaH.hpp"
#include "GTA3RuntimeScript.h"
#include <map>
#include <string>
#include "fxt.hpp"

struct UpperHash
{
	std::hash<std::string> hash;
	uint32_t operator()(const char* key) const
	{
		//static_assert(sizeof(decltype(hash)::result_type) == sizeof(uint32_t), "");
		std::string s(key);
		std::transform(s.begin(), s.end(), s.begin(), ::toupper);
		return hash(s);
	}
};

class CLuaFunctions{
public:

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
	static int log_register(lua_State *L);
	static int setCheat(lua_State *L);
	static int makeHook(lua_State *L);
	static int createMenu(lua_State *L);
	static int setMenuCol(lua_State *L);
	static int newTextEntry(lua_State *L);

	injector::basic_fxt_manager<std::map<uint32_t, std::string>, UpperHash> manager;


	static void load_callback(int id);
	static void save_callback(int id);


	int thisSaveID;

	/*
	* Set a lua function as callback for a event
	*/
	static int setCallBackToEvent(lua_State *L);

	void registerFunctions(lua_State *L);
	void registerGlobals(lua_State *L);

	static CLuaFunctions &f();

private:
	CLuaFunctions();
};

#endif