#pragma once
#ifndef _LUA_HOOKER_DYNAMIC_HOOKER_H_
#define _LUA_HOOKER_DYNAMIC_HOOKER_H_
#include <cstdint>
#include <injector\assembly.hpp>
#include <injector\calling.hpp>
#include <injector\injector.hpp>
#include <algorithm>
#include <string>
#include <vector>

namespace injectcode{
	typedef void(__cdecl *callback_t)(injector::reg_pack&, uintptr_t address);

	static uintptr_t camefromaddress;
	static uintptr_t usrcall;
	static uintptr_t retnptr;

	inline void callwrapper(injector::reg_pack *r)
	{
		callback_t c = (callback_t)usrcall;
		c(*r, camefromaddress);
	}

	// Constructs a reg_pack and calls the wrapper functor
	static inline void __declspec(naked) make_reg_pack_and_call()
	{
		_asm
		{
			pop retnptr
				pop usrcall
				pop camefromaddress
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

	static inline void __declspec(naked) make_reg_pack_and_call_with_return()
	{
		_asm
		{
			pop retnptr
				pop usrcall
				pop camefromaddress

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

	class dynamic_hooker{

	#pragma pack(push, 1)
		struct code
		{
			uint8_t push;
			uintptr_t pushaddr;
			uint8_t push2;
			uintptr_t useraddr;
			uint8_t push3;
			uintptr_t retaddr;
			uint8_t jmp;
			uintptr_t jmpaddr;
		};
	#pragma pack(pop)

		std::deque<code> codelist;

	public:
		void addhook(uintptr_t address, callback_t c)
		{
			if (std::none_of(codelist.begin(), codelist.end(), [&](const code& x) { return x.pushaddr == address; }))
			{
				auto it = codelist.emplace(codelist.end(), code{
					0x68, address,      // push address
					0x68, uintptr_t(c), // push c
					0x68, 0x00,         // push <retnaddr>
					0xE8, 0x00          // jmp <target>
				});

				if (it->retaddr = injector::MakeCALL(address, &(*it)).as_int())
					injector::MakeJMP(&it->jmp, make_reg_pack_and_call_with_return, false);
				else
					injector::MakeJMP(&it->jmp, make_reg_pack_and_call, false);
			}
		}

		inline dynamic_hooker()
		{

		}

		inline ~dynamic_hooker()
		{

		}
	};


}

#endif


