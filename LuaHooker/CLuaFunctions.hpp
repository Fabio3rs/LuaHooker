#pragma once
#ifndef _LUA_HOOKER_CLUAFUNCTIONS_HPP_
#define _LUA_HOOKER_CLUAFUNCTIONS_HPP_

#include "CLuaH.hpp"
#include "GTA3RuntimeScript.h"
#include <map>
#include <string>
#include "fxt.hpp"
#include "dynamic_hooker.hpp"
#include <game_sa\CPool.h>
#include <game_sa\CDummy.h>
#include <game_sa\CObject.h>
#include <game_sa\CVehicle.h>
#include <game_sa\CPed.h>
#include <game_sa\CAutomobile.h>
#include <deque>

#pragma pack(push, 1)
struct CStreamedIpl{
	//struc; (sizeof = 0x34)
	CRect bound;          //CRect ? ; CRange2D:
	char name[16];       //     db 16 dup(? )
	int16_t field_20;        //dw ?
	int16_t m_usBuildingsBegin; // dw ? ; first building index
	int16_t m_usBuildingsEnd; // dw ? ; last building index
	int16_t m_usDummyBegin;  // dw ? ; first dummy index
	int16_t m_usDummyEnd; //   dw ? ; last dummy index
	int16_t m_sTextIPL;      //dw ?
	int16_t m_bIsInterior;   //db ?
	bool m_bLoaded;   //db ?
	bool m_bRequired;    //db ?
	bool m_bDisableDynamicStreaming; //db ?
	bool m_bNotOwnedByMission; // db ?
	int16_t _pad1; //           db 3 dup(? )
	//	00000034 CStreamedIpl  .//  ends
};

struct CStreamingInfo { //struc; (sizeof = 0x14); XREF:.data : ms_aInfoForModelr
	uint16_t ll_usNext;
	uint16_t ll_usPrev;
	uint16_t usNextOnCd;
	uint8_t ucFlags; // db ? ; 0x10 = loading(when); 0x2 = loading(finished); 0x12 = loading
	uint8_t ucImgId; //        db ?
	uint32_t iBlockOffset; ///   dd ?
	uint32_t iBlockCount; //     dd ?
	uint8_t uiLoadStatus; //    db ? ; 0 - not loaded 2 - requested  3 - loaded  1 - processed(it's loaded yea)
	uint8_t _pad1[3];           //db 3 dup(? )
	//00000014 CStreamingInfo  ends
};

#pragma pack(pop)

VALIDATE_SIZE(CStreamedIpl, 0x34);
VALIDATE_SIZE(CStreamingInfo, 0x14);

static auto RwMalloc = injector::cstd<void*(size_t)>::call<0x00824257>;
static auto RwFree = injector::cstd<void(void*)>::call<0x0082413F>;
static CStreamingInfo *ms_aInfoForModel = injector::ReadMemory<CStreamingInfo*>(0x00407B02 + 2);
//typedef int(__cdecl *LuaHookerAPICBFun_t)(lua_State *L);

struct UpperHash
{
	std::hash<std::string> hash;
	uint32_t operator()(const char* key) const
	{
		//static_assert(sizeof(decltype(hash)::result_type) == sizeof(uint32_t), "");
		std::string s(key);
		std::transform(s.begin(), s.end(), s.begin(), ::toupper);
		return hash(s);
	}
};

class CLuaFunctions{
	char messageBuffer[2048];
	std::deque < std::function<int(lua_State*)> > registerFunctionsAPICBs;
	std::deque < std::function<int(lua_State*)> > registerGlobalsAPICBs;
	std::deque < std::function<void(void)> > frameUpdateAPICBs;

public:
	class LuaParams
	{
		lua_State *L;
		int num_params, ret, stck, fail_bit;

	public:

		LuaParams &operator<<(const std::string &param);
		LuaParams &operator<<(double param);
		LuaParams &operator<<(int param);
		LuaParams &operator<<(int64_t param);
		LuaParams &operator<<(bool param);
		LuaParams &operator<<(size_t param);

		LuaParams &operator>>(std::string &param);
		LuaParams &operator>>(double &param);
		LuaParams &operator>>(int &param);
		LuaParams &operator>>(void *&param);
		LuaParams &operator>>(uintptr_t &param);
		LuaParams &operator>>(bool &param);

		template<class T>
		inline LuaParams &operator>>(T &param){
			if (stck <= num_params){
				param = (T)(lua_tointeger(L, stck));
				++stck;
			}
			else
			{
				fail_bit = 1;
			}

			return *this;
		}

		int getNumParams();
		int rtn();

		bool fail();

		LuaParams(lua_State *state);
	};

	std::map<std::string, GTA3RuntimeScript> GTA3Scripts;
	
	static int showMessageBox(lua_State *L);
	static int writeMemory(lua_State *L);
	static int readMemory(lua_State *L);
	static int newGTA3Script(lua_State *L);
	static int GTA3ScriptSize(lua_State *L);
	static int GTA3ScriptPushOpcode(lua_State *L);
	static int runGTA3Script(lua_State *L);
	static int log_register(lua_State *L);
	static int setCheat(lua_State *L);
	static int makeHook(lua_State *L);
	static int createPanel(lua_State *L);
	static int setPanelCol(lua_State *L);
	static int newTextEntry(lua_State *L);
	static int getPanelActiveRow(lua_State *L);
	static int getPanelSelectedRow(lua_State *L);
	static int removePanel(lua_State *L);
	static int sprintf(lua_State *L);
	static int getSCMVarPointer(lua_State *L);
	static int loadModel(lua_State *L);
	static int callf(lua_State *L);
	static int callThiscall(lua_State *L);
	static int forceFloat(lua_State *L);
	static int getMakeHookReg(lua_State *L);
	static int setMakeHookReg(lua_State *L);
	static int getCheatBuffer(lua_State *L);
	static int clearCheatBuffer(lua_State *L);
	static int readString(lua_State *L);
	static int getTextEntry(lua_State *L);
	static int makeNOP(lua_State *L);
	static int keyPressed(lua_State *L);
	static int iniRead(lua_State *L);
	static int iniWrite(lua_State *L);
	static int addVehicle(lua_State *L);
	static int createVehicle(lua_State *L);

	std::vector < std::string > addVehiclesList;

	injectcode::dynamic_hooker dynamichk;

	static std::string CLuaFunctions::msprintf(lua_State *L);
	
	injector::basic_fxt_manager<std::map<uint32_t, std::string>, UpperHash> manager;

	void registerLuaFuncsAPI(std::function<int(lua_State*)> fun);
	void registerLuaGlobalsAPI(std::function<int(lua_State*)> fun);
	void registerFrameUpdateAPI(std::function<void(void)> fun);

	static void load_callback(int id);
	static void save_callback(int id);


	int thisSaveID;

	/*
	* Set a lua function as callback for a event
	*/
	static int setCallBackToEvent(lua_State *L);

	void registerFunctions(lua_State *L);
	void registerGlobals(lua_State *L);

	static CLuaFunctions &f();

	void frameUpdate();

private:
	CLuaFunctions();
	CLuaFunctions(CLuaFunctions&) = delete;
	~CLuaFunctions() = default;
};

#endif