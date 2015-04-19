#pragma once
#ifndef _GTA3RUNTIMESCRIPT_H_
#define _GTA3RUNTIMESCRIPT_H_

#include <vector>
#include <array>
#include <string>
#include <cmath>
#include <cstdint>
#include <deque>

class GTA3RuntimeScript{
	std::vector <uint8_t> code;

public:
	void push_opcode(uint16_t opcode, const char *format, const std::deque<uint32_t> &arr);
	void clear();

	void run();

	GTA3RuntimeScript();
};


#endif
