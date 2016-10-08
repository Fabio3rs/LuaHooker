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
	struct vehicleData
	{
		int model;
		float x, y, z, rx, ry, rz;
		bool alarm, doorLock;

		vehicleData()
		{
			model = 0;
			x = y = z = rx = ry = rz = 0.0;
			alarm = doorLock = false;
		}
	};

	CVehicle *spawnVehicle(const vehicleData &veh);

	static CLHVehicles &s();

	static int createVehicle(lua_State *L);
	static int getVehicleFuel(lua_State *L);
	
private:
	std::vector < vehicleData > vehiclesToBeCreate;

	static int registerFunctions(lua_State *L);
	static int registerGlobals(lua_State *L);
	static void frameUpdate();
};

#endif