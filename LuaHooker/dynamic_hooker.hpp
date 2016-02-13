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
			int8_t push;
			uintptr_t pushaddr;
			int8_t push2;
			uintptr_t useraddr;
			int8_t push3;
			uintptr_t retaddr;
			int8_t jmp;
			uintptr_t jmpaddr;

			inline code()
			{
				push = 0x68;
				pushaddr = 0;

				push2 = 0x68;
				useraddr = 0;

				push3 = 0x68;
				retaddr = 0;

				jmp = 0xE8;
				jmpaddr = 0;
			}
		};
	#pragma pack(pop)

		std::deque<code> codelist;

	public:
		inline void addhook(uintptr_t address, callback_t c)
		{
			for (auto &code : codelist)
			{
				if (code.pushaddr == address)
				{
					return;
				}
			}

			code cod;
			codelist.push_back(cod);
			code &ncode = codelist.back();

			ncode.pushaddr = address;
			ncode.useraddr = (uintptr_t)c;
			ncode.retaddr = (uintptr_t)(void*)injector::MakeCALL((void*)address, (void*)&ncode).get();

			if (ncode.retaddr != NULL)
			{
				injector::MakeJMP(&ncode.jmp, make_reg_pack_and_call_with_return, false);
			}
			else
			{
				injector::MakeJMP(&ncode.jmp, make_reg_pack_and_call, false);
			}

			DWORD oldp;

			VirtualProtect(&ncode, sizeof(code), PAGE_EXECUTE_READWRITE, &oldp);
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


