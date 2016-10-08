#include "CLHVehicles.hpp"

static auto generateVehicle = injector::cstd<CVehicle*(int modelId, int createdBy)>::call<0x00421440>;

CLHVehicles &CLHVehicles::s()
{
	static CLHVehicles inst;
	return inst;
}

static CLHVehicles &initVehInst = CLHVehicles::s();

CVehicle *CLHVehicles::spawnVehicle(const vehicleData &veh)
{
	CVehicle *result = nullptr;
	if (ms_aInfoForModel[veh.model].uiLoadStatus == 1)
	{
		result = generateVehicle(veh.model, 2);

		result->m_pCoords->at.x = veh.x;
		result->m_pCoords->at.y = veh.y;
		result->m_pCoords->at.z = veh.z;

	}

	return result;
}

int CLHVehicles::createVehicle(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	int model;
	float x, y, z;

	p >> model >> x >> y >> z;

	if (!p.fail())
	{
		vehicleData data;
		data.model = model;
		data.x = x;
		data.y = y;
		data.z = z;

		if (ms_aInfoForModel[model].uiLoadStatus == 1)
		{
			p << (unsigned int)s().spawnVehicle(data);
		}
		else
		{

		}
	}

	return p.rtn();
}

int CLHVehicles::getVehicleFuel(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);


	return p.rtn();
}

void CLHVehicles::frameUpdate()
{
	for (auto &vehList : s().vehiclesToBeCreate)
	{
		CVehicle *veh = generateVehicle(vehList.model, 2);


	}







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

