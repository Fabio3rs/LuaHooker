#pragma once
#ifndef _LUA_HOOKER_CLHVEHICLES_HPP_
#define _LUA_HOOKER_CLHVEHICLES_HPP_
#include "CLuaFunctions.hpp"
#include <memory>
#include <utility>
#include <functional>

class CLHVehicles
{
	CLHVehicles();
	CLHVehicles(CLHVehicles&) = delete;
	~CLHVehicles() = default;
public:
	static CLHVehicles &s();

	static int createVehicle(lua_State *L);
	
protected:
	static int registerFunctions(lua_State *L);
	static int registerGlobals(lua_State *L);
	static void frameUpdate();
};

#endif