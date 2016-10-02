#pragma once
#ifndef _LUA_HOOKER_CLHTEXTS_HPP_
#define _LUA_HOOKER_CLHTEXTS_HPP_
#include "CLuaFunctions.hpp"
#include <memory>
#include <utility>
#include <functional>

class CLHCamera
{
	size_t messageBufferSize;
	std::unique_ptr<char[]> messageBuffer;

	CLHCamera();
	CLHCamera(CLHCamera&) = delete;
	~CLHCamera() = default;
public:
	static CLHCamera &s();

	static int shakeCamera(lua_State *L);
	
protected:
	static int registerFunctions(lua_State *L);
	static int registerGlobals(lua_State *L);
};

#endif