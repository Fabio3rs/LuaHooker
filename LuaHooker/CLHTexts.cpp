#include "CLHTexts.hpp"

static auto ShowTextBox = injector::cstd<char(const char *, char, char, char)>::call<0x00588BE0>;
static auto textLowPriority = injector::cstd <void(const char *text, unsigned time, bool flag1, bool flag2)>::call<0x00580750>;
static auto textHighPriority = injector::cstd <void(const char *text, unsigned time, bool flag1, bool flag2)>::call<0x0069F0B0>;

CLHTexts &CLHTexts::s()
{
	static CLHTexts inst;
	return inst;
}

static CLHTexts &initInst = CLHTexts::s();

int CLHTexts::showTextBox(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	std::string str;
	p >> str;
	ShowTextBox(str.c_str(), 0, 0, 0);

	return p.rtn();
}

int CLHTexts::showLowPriorityText(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 2 && lua_isstring(L, 1) && lua_isnumber(L, 2))
	{
		std::string str;
		int time;
		p >> str >> time;
		strncpy(s().messageBuffer.get(), str.c_str(), s().messageBufferSize);
		textLowPriority(s().messageBuffer.get(), time, false, false);
	}

	return p.rtn();
}

int CLHTexts::showHighPriorityText(lua_State *L)
{
	CLuaFunctions::LuaParams p(L);

	if (p.getNumParams() == 2 && lua_isstring(L, 1) && lua_isnumber(L, 2))
	{
		std::string str;
		int time;
		p >> str >> time;
		strncpy(s().messageBuffer.get(), str.c_str(), s().messageBufferSize);
		textHighPriority(s().messageBuffer.get(), time, false, false);
	}

	return p.rtn();
}

int CLHTexts::registerFunctions(lua_State *L)
{
	lua_register(L, "showTextBox", showTextBox);
	lua_register(L, "showHighPriorityText", showHighPriorityText);
	lua_register(L, "showLowPriorityText", showLowPriorityText);


	return 0;
}

int CLHTexts::registerGlobals(lua_State *L)
{


	return 0;
}

CLHTexts::CLHTexts()
{
	messageBufferSize = 2048;
	messageBuffer = std::unique_ptr<char[]>(new char[messageBufferSize]);
	memset(messageBuffer.get(), 0, messageBufferSize);

	CLuaFunctions::f().registerLuaFuncsAPI(registerFunctions);
	CLuaFunctions::f().registerLuaFuncsAPI(registerGlobals);

}

