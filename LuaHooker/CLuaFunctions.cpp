#include "CLuaFunctions.hpp"
#include <injector\calling.hpp>
#include <injector\injector.hpp>
#include <injector\assembly.hpp>
#include "saving.hpp"
#include "CLog.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>

static auto ShowTextBox = injector::cstd<char(const char *, char, char, char)>::call<0x00588BE0>;

template<class T> void setLuaGlobal(lua_State *L, const std::string &name, const T &value)
{
	CLuaH::customParam(value).pushToLuaStack(L);
	lua_setglobal(L, name.c_str());
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

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator<<(int64_t param)
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

CLuaFunctions::LuaParams &CLuaFunctions::LuaParams::operator>>(uintptr_t &param)
{
	if (stck <= num_params){
		param = (uintptr_t)lua_tointeger(L, stck);
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

namespace inject_asm
{
	injector::reg_pack* registers = nullptr;

	// uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;

	uint32_t &retregs(const std::string &r)
	{
		auto &v = *registers;
		auto l = [](const std::string &s, uint32_t &p){
			return std::pair<std::string, uint32_t*>(s, &p);
		};
		auto m = [](const std::string &s, void* &p){
			return std::pair<std::string, uint32_t*>(s, (uint32_t*)&p);
		};

		const std::map <std::string, uint32_t*> data = {
			l("edi", v.edi),
			l("esi", v.esi),
			l("ebp", v.ebp),
			l("esp", v.esp),
			l("ebx", v.ebx),
			l("edx", v.edx),
			l("ecx", v.ecx),
			l("eax", v.eax),
			m("retn", v.retn) };

		return *data.at(r);
	}


	void defaultfun(injector::reg_pack &pack, uintptr_t address)
	{
		registers = &pack;
		CLuaH::Lua().runHookEvent(address);
		registers = nullptr;
	}
};


int CLuaFunctions::getTextEntry(lua_State *L)
{
	LuaParams p(L);

	std::string key;
	p >> key;

	if (!p.fail())
	{
		p << std::string(f().manager.get(injector::raw_ptr(0x00C1B340).get(), key.c_str()));
	}

	return p.rtn();
}

int CLuaFunctions::clearCheatBuffer(lua_State *L)
{
	LuaParams p(L);

	char *buffer = (char*)0x00969110;
	*buffer = 0;

	return p.rtn();
}

int CLuaFunctions::getCheatBuffer(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() > 0)
	{
		std::string buffer = (char*)0x00969110;

		int chars = 30;
		bool reverse = true;
		p >> chars;
		p >> reverse;

		if (buffer.size() > chars)
		{
			buffer.resize(chars);
		}

		std::reverse(buffer.begin(), buffer.end());

		p << buffer;
	}
	else{
		p << std::string((char*)0x00969110);
	}

	return p.rtn();
}

int CLuaFunctions::getMakeHookReg(lua_State *L)
{
	LuaParams p(L);

	if (inject_asm::registers && p.getNumParams() > 0)
	{
		for (int i = 0, size = p.getNumParams(); i < size; i++)
		{
			std::string rn;

			p >> rn;
			p << inject_asm::retregs(rn);
		}
	}

	return p.rtn();
}


int CLuaFunctions::readString(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() > 0)
	{
		int ptr, size = -1;
		p >> ptr;
		p >> size;

		if (size >= 0){
			char *sbeg = (char*)ptr, *send = (char*)ptr + size;

			std::string str(sbeg, send);

			p << str;
		}
		else{
			std::string str((char*)ptr);

			p << str;
		}
	}

	return p.rtn();
}

int CLuaFunctions::setMakeHookReg(lua_State *L)
{
	LuaParams p(L);

	if (inject_asm::registers && p.getNumParams() == 2)
	{
		std::string rn;

		p >> rn;

		if (lua_isinteger(L, 2))
		{
			int j;
			p >> j;
			p << (inject_asm::retregs(rn) = j);
		}
		else
		{
			float j;
			p >> j;
			p << (inject_asm::retregs(rn) = j);
		}
	}

	return p.rtn();
}

int CLuaFunctions::forceFloat(lua_State *L)
{
	LuaParams p(L);

	while (true)
	{
		int i;
		p >> i;

		if (p.fail())
			break;

		p << *(float*)&i;
	}

	return p.rtn();
}

int CLuaFunctions::callf(lua_State *L)
{
	LuaParams p(L);

	static void *memptr = 0;
	static int funptr = 0;
	static void *paramsPtr = 0;
	static int paramssize = 0;
	static int popargs = 0;
	static int retn = 0;

	try{
		if (p.getNumParams() >= 1)
		{
			std::vector <int32_t> args;
			std::deque <std::string> strs;
			std::string tmpstr;

			p >> funptr;

			popargs = 0;
			p >> popargs;
			popargs *= 4;

			for (int i = 3, size = p.getNumParams(); i <= size; ++i)
			{
				switch (lua_type(L, i))
				{
				case LUA_TNIL:
					break;

				case LUA_TNUMBER:
					if (lua_isinteger(L, i))
					{
						int c;
						p >> c;
						args.push_back(c);
					}
					else{
						float c;
						p >> c;
						args.push_back(*(int*)&c);
					}
					break;

				case LUA_TBOOLEAN:
					bool c;
					p >> c;
					args.push_back(c);
					break;

				case LUA_TSTRING:
					p >> tmpstr;
					strs.push_back(tmpstr);
					args.push_back((int)&(strs.back()[0]));
					break;

				case LUA_TTABLE:
				case LUA_TFUNCTION:
				case LUA_TUSERDATA:
				case LUA_TTHREAD:
				case LUA_TLIGHTUSERDATA:
				default:
					break;
				}
			}


			paramsPtr = &args[0];
			paramssize = 4 * args.size();

			_asm
			{
				sub esp, paramssize
					mov memptr, esp

					pushad
					mov ecx, paramssize
					mov esi, paramsPtr
					mov edi, memptr
					rep movsb
					popad

					call funptr

					add esp, popargs

					mov retn, eax
			}

			p << retn;
		}
	}
	catch (const std::exception &e){
		CLog::log() << std::string("callf error ") + e.what();
	}
	catch (...){

		CLog::log() << "callf unknow error";
	}

	return p.rtn();
}

int CLuaFunctions::iniWrite(lua_State *L)
{
	LuaParams p(L);

	std::string fname, section, key, data;
	p >> fname >> section >> key >> data;

	fname = ".\\" + fname;

	if (!p.fail())
	{
		p << WritePrivateProfileStringA(section.c_str(), key.c_str(), data.c_str(), fname.c_str());
	}

	return p.rtn();
}

int CLuaFunctions::iniRead(lua_State *L)
{
	LuaParams p(L);

	std::string fname, section, key, defaultval;
	p >> fname >> section >> key;

	fname = ".\\" + fname;

	if (!p.fail())
	{
		char r[4096];
		p >> defaultval;

		GetPrivateProfileStringA(section.c_str(), key.c_str(), defaultval.c_str(), r, sizeof(r), fname.c_str());

		p << std::string(r);
	}


	return p.rtn();
}

int CLuaFunctions::keyPressed(lua_State *L)
{
	LuaParams p(L);

	while (true)
	{
		int key;
		p >> key;

		if (p.fail())
			break;

		p << (HIBYTE(GetKeyState(key)) == 0xFF);
	}

	return p.rtn();
}

int CLuaFunctions::callThiscall(lua_State *L)
{
	LuaParams p(L);

	static void *memptr = 0;
	static int funptr = 0;
	static int ecxptr = 0;
	static void *paramsPtr = 0;
	static int paramssize = 0;
	static int popargs = 0;
	static int retn = 0;

	try{
		if (p.getNumParams() >= 2)
		{
			std::vector <int32_t> args;
			std::deque <std::string> strs;
			std::string tmpstr;

			p >> funptr;
			p >> ecxptr;

			popargs = 0;
			p >> popargs;
			popargs *= 4;

			for (int i = 4, size = p.getNumParams(); i <= size; ++i)
			{
				switch (lua_type(L, i))
				{
				case LUA_TNIL:
					break;

				case LUA_TNUMBER:
					if (lua_isinteger(L, i))
					{
						int c;
						p >> c;
						args.push_back(c);
					}
					else{
						float c;
						p >> c;
						args.push_back(*(int*)&c);
					}
					break;

				case LUA_TBOOLEAN:
					bool c;
					p >> c;
					args.push_back(c);
					break;

				case LUA_TSTRING:
					p >> tmpstr;
					strs.push_back(tmpstr);
					args.push_back((int)&(strs.back()[0]));
					break;

				case LUA_TTABLE:
				case LUA_TFUNCTION:
				case LUA_TUSERDATA:
				case LUA_TTHREAD:
				case LUA_TLIGHTUSERDATA:
				default:
					break;
				}
			}


			paramsPtr = &args[0];
			paramssize = 4 * args.size();

			int ecxbackup;

			{
				_asm
				{
					sub esp, paramssize
						mov memptr, esp

						pushad
						mov ecx, paramssize
						mov esi, paramsPtr
						mov edi, memptr
						rep movsb
						popad

						mov ecxbackup, ecx

						mov ecx, ecxptr
						call funptr

						mov ecx, ecxbackup

						add esp, popargs

						mov retn, eax
				}
			}

			p << retn;
		}
	}
	catch (const std::exception &e){
		CLog::log() << std::string("callThiscall error ") + e.what();
	}
	catch (...){

		CLog::log() << "callThiscall unknow error";
	}

	return p.rtn();
}

int CLuaFunctions::makeHook(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() == 2 && lua_isinteger(L, 1) && lua_isfunction(L, 2)){
		uintptr_t ptr;
		p >> ptr;

		//CLog::log() << "making hook to " + std::to_string(ptr);

		//inject_asm::inject_asm(ptr);
		
		f().dynamichk.addhook(ptr, inject_asm::defaultfun);
		
		lua_pushvalue(L, 2);
		int	fnRef = luaL_ref(L, LUA_REGISTRYINDEX);

		auto &ls = CLuaH::Lua().getLastScript();

		ls.hooks[ptr] = fnRef;
		ls.hooksAdded = true;
	}


	return p.rtn();
}

int createGameMenu(const std::string &name, float px, float py, float width, int columns, bool interactive, bool background, bool alignment)
{
	typedef char(__cdecl *CMenuSystem__CreateNewMenu_t)(int a2, const char *a3, float a4, float a5, float a6, char a7, bool a8, bool a9, bool a10);
	CMenuSystem__CreateNewMenu_t CMenuSystem__CreateNewMenu = (CMenuSystem__CreateNewMenu_t)0x00582300;
	return CMenuSystem__CreateNewMenu(0, name.c_str(), px, py, width, columns, interactive, background, alignment);
}

void setMenu(int menu, int col, const std::string &header, const std::string &dat1, const std::string &dat2, const std::string &dat3, const std::string &dat4, const std::string &dat5, const std::string &dat6, const std::string &dat7, const std::string &dat8, const std::string &dat9, const std::string &dat10, const std::string &dat11, const std::string &dat12)
{
	typedef void **(__cdecl *sub_581E00_t)(unsigned __int8 a1, unsigned __int8 a2, const char *a3, const char *a4, const char *a5, const char *a6, const char *a7, const char *a8, const char *a9, const char *a10, const char *a11, const char *a12, const char *a13, const char *a14, const char *a15);
	sub_581E00_t sub_581E00 = (sub_581E00_t)0x00581E00;

	sub_581E00(menu, col, header.c_str(), (dat1 == "DUMMY") ? (const char*)0 : dat1.c_str(),
		(dat2 == "DUMMY") ? (const char*)0 : dat2.c_str(),
		(dat3 == "DUMMY") ? (const char*)0 : dat3.c_str(),
		(dat4 == "DUMMY") ? (const char*)0 : dat4.c_str(),
		(dat5 == "DUMMY") ? (const char*)0 : dat5.c_str(),
		(dat6 == "DUMMY") ? (const char*)0 : dat6.c_str(),
		(dat7 == "DUMMY") ? (const char*)0 : dat7.c_str(),
		(dat8 == "DUMMY") ? (const char*)0 : dat8.c_str(),
		(dat9 == "DUMMY") ? (const char*)0 : dat9.c_str(),
		(dat10 == "DUMMY") ? (const char*)0 : dat10.c_str(),
		(dat11 == "DUMMY") ? (const char*)0 : dat11.c_str(),
		(dat12 == "DUMMY") ? (const char*)0 : dat12.c_str());
}

int CLuaFunctions::createPanel(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() == 8 && lua_isstring(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4) && lua_isinteger(L, 5) && lua_isboolean(L, 6) && lua_isboolean(L, 7) && lua_isboolean(L, 8)){
		std::string str;
		double px, py, wid;
		int cols;
		bool inter, backg, align;
		p >> str >> px >> py >> wid >> cols >> inter >> backg >> align;
		

		if (!p.fail())
		{
			p << createGameMenu(str, px, py, wid, cols, inter, backg, align);
		}
		else{
			CLog::log() << "create menu error";
		}
	}
	else{
		CLog::log() << "create menu error";
	}


	return p.rtn();
}

int CLuaFunctions::newTextEntry(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() == 2)
	{
		std::string k, s;

		p >> k >> s;

		if (!p.fail())
		{
			f().manager.add(k.c_str(), s.c_str());
		}
	}

	return p.rtn();
}

int CLuaFunctions::setPanelCol(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() > 3)
	{
		int menu, col;
		std::string h, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12;

		d1 = d2 = d3 = d4 = d5 = d6 = d7 = d8 = d9 = d10 = d11 = d12 = "DUMMY";

		p >> menu >> col >> h >> d1 >> d2 >> d3 >> d4 >> d5 >> d6 >> d7 >> d8 >> d9 >> d10 >> d11 >> d12;

		setMenu(menu, col, h, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12);
	}
	else{
		CLog::log() << "setMenuCol error";
	}

	return p.rtn();
}

auto getGamePanelActiveRow = injector::cstd <char(unsigned __int8 a1)>::call<0x005807E0>;
auto getGamePanelSelectedRow = injector::cstd <char(unsigned __int8 a1)>::call<0x005807C0>;
auto removeGamePanel = injector::cstd <char(unsigned __int8 a1)>::call<0x00580750>;
auto textLowPriority = injector::cstd <void(const char *text, unsigned time, bool flag1, bool flag2)>::call<0x00580750>;
auto textHighPriority = injector::cstd <void(const char *text, unsigned time, bool flag1, bool flag2)>::call<0x0069F0B0>;
auto CStreaming__RequestModel = injector::cstd <char(int, int)>::call<0x004087E0>;


int CLuaFunctions::loadModel(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() == 1 && lua_isnumber(L, 1))
	{
		int m;
		p >> m;

		CStreaming__RequestModel(m, 0xC);
	}

	return p.rtn();
}

int CLuaFunctions::showLowPriorityText(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() == 2 && lua_isstring(L, 1) && lua_isnumber(L, 2))
	{
		std::string str;
		int time;
		p >> str >> time;
		strncpy(f().messageBuffer, str.c_str(), sizeof(f().messageBuffer));
		textLowPriority(f().messageBuffer, time, false, false);
	}

	return p.rtn();
}

int CLuaFunctions::showHighPriorityText(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() == 2 && lua_isstring(L, 1) && lua_isnumber(L, 2))
	{
		std::string str;
		int time;
		p >> str >> time;
		strncpy(f().messageBuffer, str.c_str(), sizeof(f().messageBuffer));
		textHighPriority(f().messageBuffer, time, false, false);
	}

	return p.rtn();
}

int CLuaFunctions::removePanel(lua_State *L)
{
	LuaParams p(L);

	for (int i = 0, size = p.getNumParams(); i < size; ++i)
	{
		int panel = 0;
		p >> panel;

		if (!p.fail())
		{
			removeGamePanel(panel);
		}
	}

	return p.rtn();
}

int CLuaFunctions::getPanelSelectedRow(lua_State *L)
{
	LuaParams p(L);

	for (int i = 0, size = p.getNumParams(); i < size; ++i)
	{
		int panel = 0;
		p >> panel;

		if (!p.fail())
		{
			p << getGamePanelSelectedRow(panel);
		}
	}

	return p.rtn();
}

std::string CLuaFunctions::msprintf(lua_State *L)
{
	LuaParams p(L);
	char vaargspace[1024];
	char vsprintspace[4096] = {0};
	int pos = 0;
	std::deque < std::string > strs;

	if (p.getNumParams() >= 1 && lua_isstring(L, 1)){
		std::string s;
		std::string tstr;
		bool tbool;
		double tdouble;
		int tint;
		p >> s;

		for (int i = 2, size = p.getNumParams(); i <= size; ++i)
		{
			switch (lua_type(L, i))
			{
			case LUA_TNIL:
				break;

			case LUA_TNUMBER:
				if (lua_isinteger(L, i))
				{
					p >> tint;
					*(int*)&vaargspace[pos] = tint;
					pos += sizeof(int);
				}
				else{
					p >> tdouble;
					*(double*)&vaargspace[pos] = tdouble;
					pos += sizeof(double);
				}
				break;

			case LUA_TBOOLEAN:
				p >> tbool;
				*(int*)&vaargspace[pos] = tbool;
				pos += sizeof(int);
				break;

			case LUA_TSTRING:
				p >> tstr;
				strs.push_back(tstr);
				*(char**)&vaargspace[pos] = &(strs.back()[0]);
				pos += sizeof(char*);
				break;

			case LUA_TTABLE:
			case LUA_TFUNCTION:
			case LUA_TUSERDATA:
			case LUA_TTHREAD:
			case LUA_TLIGHTUSERDATA:
			default:
				break;
			}

		}

		int csss = vsprintf(vsprintspace, s.c_str(), vaargspace);
	}

	return std::string(vsprintspace);
}

int CLuaFunctions::sprintf(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() >= 1 && lua_isstring(L, 1))
	{
		p << msprintf(L);
	}

	return p.rtn();
}

int CLuaFunctions::getPanelActiveRow(lua_State *L)
{
	LuaParams p(L);

	for (int i = 0, size = p.getNumParams(); i < size; ++i)
	{
		int panel = 0;
		p >> panel;

		if (!p.fail())
		{
			p << getGamePanelActiveRow(panel);
		}
	}

	return p.rtn();
}

void CLuaFunctions::registerFunctions(lua_State *L)
{
	f();

	lua_register(L, "showMessageBox", showMessageBox);
	lua_register(L, "writeMemory", writeMemory);
	lua_register(L, "readMemory", readMemory);
	lua_register(L, "showTextBox", showTextBox);
	lua_register(L, "newGTA3Script", newGTA3Script);
	lua_register(L, "GTA3ScriptSize", GTA3ScriptSize);
	lua_register(L, "GTA3ScriptPushOpcode", GTA3ScriptPushOpcode);
	lua_register(L, "setCheat", setCheat);
	lua_register(L, "makeHook", makeHook);
	lua_register(L, "createPanel", createPanel);
	lua_register(L, "setPanelCol", setPanelCol);
	lua_register(L, "newTextEntry", newTextEntry);
	lua_register(L, "getPanelSelectedRow", getPanelSelectedRow);
	lua_register(L, "getPanelActiveRow", getPanelActiveRow);
	lua_register(L, "removePanel", removePanel);
	lua_register(L, "showHighPriorityText", showHighPriorityText);
	lua_register(L, "showLowPriorityText", showLowPriorityText);
	lua_register(L, "sprintf", sprintf);
	lua_register(L, "getSCMVarPointer", getSCMVarPointer);
	lua_register(L, "loadModel", loadModel);
	lua_register(L, "callf", callf);
	lua_register(L, "callThiscall", callThiscall);
	lua_register(L, "forceFloat", forceFloat);
	lua_register(L, "getMakeHookReg", getMakeHookReg);
	lua_register(L, "setMakeHookReg", setMakeHookReg);
	lua_register(L, "getCheatBuffer", getCheatBuffer);
	lua_register(L, "clearCheatBuffer", clearCheatBuffer);
	lua_register(L, "readString", readString);
	lua_register(L, "getTextEntry", getTextEntry);
	lua_register(L, "makeNOP", makeNOP);
	lua_register(L, "keyPressed", keyPressed);
	lua_register(L, "iniRead", iniRead);
	lua_register(L, "iniWrite", iniWrite);

	lua_register(L, "setCallBackToEvent", setCallBackToEvent);
	lua_register(L, "log_register", log_register);
}

CLuaFunctions &CLuaFunctions::f()
{
	static CLuaFunctions functs;
	return functs;
}

/*
setCallBackToEvent(event, function)
*/
int CLuaFunctions::setCallBackToEvent(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() == 2 && lua_isstring(L, 1) && lua_isfunction(L, 2)){
		std::string eventName = lua_tostring(L, 1);

		lua_pushvalue(L, 2);
		int	fnRef = luaL_ref(L, LUA_REGISTRYINDEX);

		auto &ls = CLuaH::Lua().getLastScript();

		ls.callbacks[eventName] = fnRef;
		ls.callbacksAdded = true;
	}


	return p.rtn();
}

/**/
extern "C" void __declspec(dllexport) load_luascript_from_string(const char *script, const char *scriptname, void *cscmscmptr, bool autorun)
{
	auto &result = (CLuaH::Lua().files["*"][scriptname] = std::move(CLuaH::Lua().newScriptR(script, scriptname)));

	if (autorun)
	{
		CLuaH::Lua().runScript(result);
	}
}

extern "C" void __declspec(dllexport) luascript_run(const char *scriptpath, const char *scriptname)
{
	std::string lpath = scriptpath;

	if (lpath == ""){ lpath = "*"; }

	CLuaH::Lua().runScript(CLuaH::Lua().files[lpath][scriptname]);
}

extern "C" void __declspec(dllexport) luascript_set_str_variable(const char *scriptpath, const char *scriptname, const char *var, const char *value)
{
	std::string lpath = scriptpath;

	if (lpath == ""){ lpath = "*"; }

	setLuaGlobal(CLuaH::Lua().files[lpath][scriptname].luaState, var, value);
}

extern "C" void __declspec(dllexport) luascript_set_int_variable(const char *scriptpath, const char *scriptname, const char *var, int value)
{
	std::string lpath = scriptpath;
	
	if (lpath == ""){ lpath = "*"; }

	setLuaGlobal(CLuaH::Lua().files[lpath][scriptname].luaState, var, value);
}

void CLuaFunctions::registerGlobals(lua_State *L)
{

}

void CLuaFunctions::load_callback(int id)
{
	CLuaH::Lua().files["*"].clear();

	f().thisSaveID = id;

	try{
		CLuaH::Lua().runEventWithParams("gameLoading", { id });
	}
	catch (const std::exception &e)
	{
		CLog::log() << e.what();
	}
	catch (...)
	{
		CLog::log() << "Unknow exception - CLuaH 'gameLoading' event";
	}
}


int CLuaFunctions::setCheat(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() == 2 && lua_isstring(L, 1) && lua_isfunction(L, 2)){
		std::string eventName = lua_tostring(L, 1);

		lua_pushvalue(L, 2);
		int	fnRef = luaL_ref(L, LUA_REGISTRYINDEX);

		CLuaH::Lua().getLastScript().cheats[eventName] = fnRef;

		CLuaH::Lua().getLastScript().cheatsAdded = true;
	}

	return p.rtn();
}

void CLuaFunctions::save_callback(int id)
{
	CLog::log() << "Teste s";

	f().thisSaveID = id;

	try{
		CLuaH::Lua().runEventWithParams("gameSaving", { id });
	}
	catch (const std::exception &e)
	{
		CLog::log() << e.what();
	}
	catch (...)
	{
		CLog::log() << "Unknow exception - CLuaH 'gameSaving' event";
	}
}

CLuaFunctions::CLuaFunctions()
{
	manager.make_samp_compatible();
	injector::save_manager::on_load(load_callback);
	injector::save_manager::on_save(save_callback);
	thisSaveID = 0;

	injector::MakeInline<0x0053BFCC>([](injector::reg_pack &)
	{
		try{
			CLuaH::Lua().runEvent("gameUpdate");

			std::string buffer = (char*)0x00969110;
			//std::reverse(buffer.begin(), buffer.end());
			CLuaH::Lua().runCheatEvent(buffer);
		}
		catch (const std::exception &e)
		{
			CLog::log() << e.what();
		}
		catch (...)
		{
			CLog::log() << "Unknow exception - CLuaH 'gameUpdate' event";
		}
	});
}

int CLuaFunctions::log_register(lua_State *L)
{
	LuaParams p(L);
	std::string t;

	p >> t;

	if (!p.fail())
	{
		CLog::log() << CLuaH::Lua().getLastScript().filePath + "/" + CLuaH::Lua().getLastScript().fileName + " => " + t;
	}

	return p.rtn();
}

int CLuaFunctions::newGTA3Script(lua_State *L)
{
	LuaParams p(L);
	std::string t;

	for (int i = 0, size = p.getNumParams() - 1; i < size; i++, p << &f().GTA3Scripts[(p >> t, t)]);
	
	return p.rtn();
}

int CLuaFunctions::GTA3ScriptSize(lua_State *L)
{
	LuaParams p(L);
	std::string t;

	for (int i = 0, size = p.getNumParams() - 1; i < size; i++, p << f().GTA3Scripts[(p >> t, t)].size());

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

	for (int i = 0, size = p.getNumParams() - 1; i < size; i++, f().GTA3Scripts[(p >> t, t)].run());

	return p.rtn();
}

int CLuaFunctions::getSCMVarPointer(lua_State *L)
{
	LuaParams p(L);

	for (int i = 0, size = p.getNumParams(); i < size; i++)
	{
		unsigned int glob = 0;
		p >> glob;
		p << 0x00A49960 + glob * 4;
	}

	return p.rtn();
}


int CLuaFunctions::makeNOP(lua_State *L)
{
	LuaParams p(L);

	int mem, size;
	p >> mem >> size;

	if (!p.fail()){
		injector::MakeNOP(mem, size);
	}


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
		double datad = 0;
		int size;

		p >> mem >> data >> size;

		if (p.fail())
		{
			p << false;
			return p.rtn();
		}

		bool vp = false;
		bool readfloat = (lua_isnumber(L, 3) && !lua_isinteger(L, 3)) || (lua_isnumber(L, 2) && !lua_isinteger(L, 2));
		p >> readfloat >> vp;

		if (readfloat)
		{
			datad = lua_tonumber(L, 2);
		}

		switch (size){
		case 1:
			injector::WriteMemory<uint8_t>(mem, data, vp);
			break;

		case 2:
			injector::WriteMemory<uint16_t>(mem, data, vp);
			break;

		case 4:
			if (readfloat)
			{
				injector::WriteMemory<float>(mem, datad, vp);
			}
			else{
				injector::WriteMemory<int32_t>(mem, data, vp);
			}
			break;

		case 8:
			if (readfloat)
			{
				injector::WriteMemory<double>(mem, datad, vp);
			}
			else{
				injector::WriteMemory<int64_t>(mem, data, vp);
			}
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
		bool readfloat = (lua_isnumber(L, 2) && !lua_isinteger(L, 2));
		p >> readfloat >> vp;

		switch (size){
		case 1:
			p << injector::ReadMemory<uint8_t>(mem, vp);
			break;

		case 2:
			p << injector::ReadMemory<uint16_t>(mem, vp);
			break;

		case 4:
			if (readfloat)
			{
				p << injector::ReadMemory<float>(mem, vp);
			}
			else{
				p << injector::ReadMemory<int32_t>(mem, vp);
			}
			break;

		case 8:
			if (readfloat)
			{
				p << injector::ReadMemory<double>(mem, vp);
			}
			else{
				p << "Read memory error - can't read 8 bytes integer";
			}
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
