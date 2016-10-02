#include "CLHCamera.hpp"

static auto ShakeCamera = injector::cstd<void(int, float)>::call<0x0050A970>;

CLHCamera &CLHCamera::s()
{
	static CLHCamera inst;
	return inst;
}

static CLHCamera &initInst = CLHCamera::s();

int CLHCamera::shakeCamera(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);
	
	int time;

	p >> time;
	if (!p.fail()){
		ShakeCamera(0x00B6F028, time * 0.001);
	}

	return p.rtn();
}

int CLHCamera::registerFunctions(lua_State *L)
{
	lua_register(L, "shakeCamera", shakeCamera);
	return 0;
}

int CLHCamera::registerGlobals(lua_State *L)
{
	return 0;
}

CLHCamera::CLHCamera()
{
	CLuaFunctions::f().registerLuaFuncsAPI(registerFunctions);
	CLuaFunctions::f().registerLuaFuncsAPI(registerGlobals);
}

