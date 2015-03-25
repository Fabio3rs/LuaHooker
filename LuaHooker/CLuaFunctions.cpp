#include "CLuaFunctions.hpp"

CLuaFunctions &CLuaFunctions::LuaF()
{
	static CLuaFunctions LuaF;
	return LuaF;
}

void CLuaFunctions::registerFunctions(lua_State *L)
{
	lua_register(L, "showMessageBox", showMessageBox);
}

CLuaFunctions::CLuaFunctions()
{

}

int CLuaFunctions::showMessageBox(lua_State *L)
{
	int params = lua_gettop(L);

	if (params == 2){
		MessageBoxA(0, lua_tostring(L, 1), lua_tostring(L, 2), 0);
	}

	return 0;
}

int CLuaFunctions::crashMyGame(lua_State *L)
{

}
