#pragma once
#ifndef _LUA_HOOKER_CLHTEXTS_HPP_
#define _LUA_HOOKER_CLHTEXTS_HPP_
#include "CLuaFunctions.hpp"
#include <memory>
#include <utility>
#include <functional>

class CLHTexts
{
	size_t messageBufferSize;
	std::unique_ptr<char[]> messageBuffer;

	CLHTexts();
	CLHTexts(CLHTexts&) = delete;
	~CLHTexts() = default;
public:
	static CLHTexts &s();

	static int showTextBox(lua_State *L);
	static int showHighPriorityText(lua_State *L);
	static int showLowPriorityText(lua_State *L);
	
protected:
	static int registerFunctions(lua_State *L);
	static int registerGlobals(lua_State *L);
};

#endif