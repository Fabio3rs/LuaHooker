#include "CLuaFunctions.hpp"
#include <injector\calling.hpp>
#include <injector\injector.hpp>
#include <injector\assembly.hpp>
#include "saving.hpp"
#include "CLog.h"

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

namespace inject_asm
{
	typedef std::function<void(injector::reg_pack&, uintptr_t address)> fun_t;

	struct clback{
		injector::memory_pointer_raw retnptr;
		bool setted;

		clback()
		{
			retnptr = 0u;
			setted = 0;
		}
	};

	void defaultfun(injector::reg_pack &pack, uintptr_t address)
	{
		CLuaH::Lua().runHookEvent(address);
	}

	static std::map<uintptr_t, clback> hookmaps;

	void *retnptr;

	static void callwrapper(injector::reg_pack* regs)
	{
		auto &s = inject_asm::hookmaps[(uintptr_t)regs->retn];
		retnptr = s.retnptr.get_raw<void*>();
		defaultfun(*regs, (uintptr_t)regs->retn);
	}

	// Constructs a reg_pack and calls the wrapper functor
	inline void __declspec(naked) make_reg_pack_and_call()
	{
		_asm
		{
			// Construct the reg_pack structure on the stack
			pushad              // Pushes general purposes registers to reg_pack
				add[esp + 12], 4     // Add 4 to reg_pack::esp 'cuz of our return pointer, let it be as before this func is called
				pushfd              // Pushes EFLAGS to reg_pack

				// Call wrapper sending reg_pack as parameter
				push esp
				call callwrapper
				add esp, 4

				// Destructs the reg_pack from the stack
				sub[esp + 12 + 4], 4   // Fix reg_pack::esp before popping it (doesn't make a difference though) (+4 because eflags)
				popfd               // Warning: Do not use any instruction that changes EFLAGS after this (-> sub affects EF!! <-)
				popad

				// Back to normal flow
				ret
		}
	}

	inline void __declspec(naked) make_reg_pack_and_call_with_return()
	{
		_asm
		{
			// Construct the reg_pack structure on the stack
			pushad              // Pushes general purposes registers to reg_pack
				add[esp + 12], 4     // Add 4 to reg_pack::esp 'cuz of our return pointer, let it be as before this func is called
				pushfd              // Pushes EFLAGS to reg_pack

				// Call wrapper sending reg_pack as parameter
				push esp
				call callwrapper
				add esp, 4

				// Destructs the reg_pack from the stack
				sub[esp + 12 + 4], 4   // Fix reg_pack::esp before popping it (doesn't make a difference though) (+4 because eflags)
				popfd               // Warning: Do not use any instruction that changes EFLAGS after this (-> sub affects EF!! <-)
				popad

				// jump
				push retnptr
				retn
		}
	}

	void inject_asm(uintptr_t address)
	{
		auto &s = hookmaps[address + 5];

		if (!s.setted){
			s.setted = true;
			s.retnptr = injector::MakeCALL(address, make_reg_pack_and_call);

			if (s.retnptr){ injector::MakeCALL(address, make_reg_pack_and_call_with_return); }
		}
	}
};


int CLuaFunctions::makeHook(lua_State *L)
{
	LuaParams p(L);

	if (p.getNumParams() == 2 && lua_isinteger(L, 1) && lua_isfunction(L, 2)){
		uintptr_t ptr;
		p >> ptr;

		CLog::log() << "making hook to " + std::to_string(ptr);

		inject_asm::inject_asm(ptr);

		
		lua_pushvalue(L, 2);
		int	fnRef = luaL_ref(L, LUA_REGISTRYINDEX);

		auto &ls = CLuaH::Lua().getLastScript();

		ls.hooks[ptr + 5] = fnRef;
		ls.hooksAdded = true;
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
	injector::save_manager::on_load(load_callback);
	injector::save_manager::on_save(save_callback);
	thisSaveID = 0;

	injector::MakeInline<0x0053BFCC>([](injector::reg_pack &)
	{
		try{
			CLuaH::Lua().runEvent("gameUpdate");

			std::string buffer = (char*)0x969110;
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
		CLog::log() << t;
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
