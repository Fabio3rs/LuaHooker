#include "CLuaFunctions.hpp"
#include <injector\calling.hpp>
#include <injector\injector.hpp>
#include <injector\assembly.hpp>


static auto ShowTextBox = injector::cstd<char(const char *, char, char, char)>::call<0x00588BE0>;

CLuaFunctions &CLuaFunctions::LuaF()
{
	static CLuaFunctions LuaF;
	return LuaF;
}

CLuaFunctions::LuaParams::LuaParams(lua_State *state)
{
	L = state;
	num_params = lua_gettop(L);

	ret = 0;
	stck = 1;
	fail_bit = 0;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator<<(const std::string &param)
{
	lua_pushstring(L, param.c_str());
	++ret;

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator<<(double param)
{
	lua_pushnumber(L, param);
	++ret;

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator<<(int param)
{
	lua_pushinteger(L, param);
	++ret;

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator<<(size_t param)
{
	lua_pushinteger(L, param);
	++ret;

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator<<(bool param)
{
	lua_pushboolean(L, param);
	++ret;

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator>>(std::string &param)
{
	if (stck <= num_params){
		param = lua_tostring(L, stck);
		++stck;
	}
	else
	{
		fail_bit = 1;
	}

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator>>(double &param)
{
	if (stck <= num_params){
		param = lua_tonumber(L, stck);
		++stck;
	}
	else
	{
		fail_bit = 1;
	}

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator>>(int &param)
{
	if (stck <= num_params){
		param = lua_tointeger(L, stck);
		++stck;
	}
	else
	{
		fail_bit = 1;
	}

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator>>(void *&param)
{
	if (stck <= num_params){
		param = (void*)lua_tointeger(L, stck);
		++stck;
	}
	else
	{
		fail_bit = 1;
	}

	return *this;
}

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator>>(bool &param)
{
	if (stck <= num_params){
		param = lua_toboolean(L, stck);
		++stck;
	}
	else
	{
		fail_bit = 1;
	}

	return *this;
}

bool CLuaFunctions::LuaParams::fail()
{
	return fail_bit != 0;
}

int CLuaFunctions::LuaParams::rtn()
{
	return ret;
}

int CLuaFunctions::LuaParams::getNumParams()
{
	return num_params;
}

void CLuaFunctions::registerFunctions(lua_State *L)
{
	lua_register(L, "showMessageBox", showMessageBox);
	lua_register(L, "writeMemory", writeMemory);
	lua_register(L, "readMemory", readMemory);
	lua_register(L, "showTextBox", showTextBox);
	lua_register(L, "newGTA3Script", newGTA3Script);
	lua_register(L, "GTA3ScriptSize", GTA3ScriptSize);
	lua_register(L, "GTA3ScriptPushOpcode", GTA3ScriptPushOpcode);
}

void CLuaFunctions::registerGlobals(lua_State *L)
{

}

CLuaFunctions::CLuaFunctions()
{

}

int CLuaFunctions::newGTA3Script(lua_State *L)
{
	LuaParams p(L);
	std::string t;

	for (int i = 0, size = p.getNumParams() - 1; i < size; i++, p << &LuaF().GTA3Scripts[(p >> t, t)]);
	
	return p.rtn();
}

int CLuaFunctions::GTA3ScriptSize(lua_State *L)
{
	LuaParams p(L);
	std::string t;

	for (int i = 0, size = p.getNumParams() - 1; i < size; i++, p << LuaF().GTA3Scripts[(p >> t, t)].size());

	return p.rtn();
}

int CLuaFunctions::GTA3ScriptPushOpcode(lua_State *L)
{
	LuaParams p(L);

	/*
	script name,
	format {%d, %f, %s; -> x}
	*/

	return p.rtn();
}

int CLuaFunctions::runGTA3Script(lua_State *L)
{
	LuaParams p(L);
	std::string t;

	for (int i = 0, size = p.getNumParams() - 1; i < size; i++, LuaF().GTA3Scripts[(p >> t, t)].run());

	return p.rtn();
}

int CLuaFunctions::showMessageBox(lua_State *L)
{
	LuaParams p(L);

	std::string messageTitle, messageText;

	p >> messageText >> messageTitle;

	if (!p.fail()){
		p << MessageBoxA(0, messageText.c_str(), messageTitle.c_str(), 0);
	}
	else
	{
		p << false;
	}

	return p.rtn();
}

int CLuaFunctions::writeMemory(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() >= 3){
		injector::memory_pointer_raw mem;
		unsigned int data = 0;
		int size;

		p >> mem >> data >> size;

		if (p.fail())
		{
			p << false;
			return p.rtn();
		}

		bool vp = false;
		p >> vp;

		switch (size){
		case 1:
			injector::WriteMemory<uint8_t>(mem, data, vp);
			break;

		case 2:
			injector::WriteMemory<uint16_t>(mem, data, vp);
			break;

		case 4:
			injector::WriteMemory<int32_t>(mem, data, vp);
			break;

		default:
			p << false;
			break;
		}
		p << true;
	}
	else
	{
		p << false;
	}


	return p.rtn();
}

int CLuaFunctions::readMemory(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() >= 2){
		injector::memory_pointer_raw mem;
		unsigned int size = 0;
		p >> mem >> size;

		if (p.fail())
		{
			p << false;
			return p.rtn();
		}

		bool vp = false;
		p >> vp;

		switch (size){
		case 1:
			p << injector::ReadMemory<uint8_t>(mem, vp);
			break;

		case 2:
			p << injector::ReadMemory<uint16_t>(mem, vp);
			break;

		case 4:
			p << injector::ReadMemory<int32_t>(mem, vp);
			break;

		default:
			p << false;
			break;
		}
	}
	else
	{
		p << false;
	}

	return p.rtn();
}

int CLuaFunctions::showTextBox(lua_State *L){
	LuaParams p(L);

	std::string str;
	p >> str;
	ShowTextBox(str.c_str(), 0, 0, 0);

	return p.rtn();
}
