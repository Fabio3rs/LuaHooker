#pragma once
#ifndef _LUA_HOOKER_CLUAH_HPP_
#define _LUA_HOOKER_CLUAH_HPP_

#include "includes.hpp"

class CLuaH{
	bool inited;

public:
	static CLuaH				&Lua();

	inline bool					initSuccess(){ return inited; }

	bool						loadFiles(const std::string &path);

	CLuaH();
};


#endif