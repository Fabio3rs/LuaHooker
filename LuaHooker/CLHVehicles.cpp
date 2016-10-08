#include "CLHVehicles.hpp"

CLHVehicles &CLHVehicles::s()
{
	static CLHVehicles inst;
	return inst;
}

static CLHVehicles &initVehInst = CLHVehicles::s();

int CLHVehicles::createVehicle(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	int model;
	float x, y, z;

	p >> model >> x >> y >> z;

	if (!p.fail())
	{
		if (ms_aInfoForModel[model].uiLoadStatus == 1)
		{


		}
	}

	return p.rtn();
}

void CLHVehicles::frameUpdate()
{




}

int CLHVehicles::registerFunctions(lua_State *L)
{


	return 0;
}

int CLHVehicles::registerGlobals(lua_State *L)
{


	return 0;
}

CLHVehicles::CLHVehicles()
{
	CLuaFunctions::f().registerLuaFuncsAPI(registerFunctions);
	CLuaFunctions::f().registerLuaFuncsAPI(registerGlobals);
	CLuaFunctions::f().registerFrameUpdateAPI(frameUpdate);

}

