#include <Windows.h>
#pragma once

// Direct ASM injection
namespace AsmEdition
{
	// Jump back to the code flow from donating guild points
	static DWORD jmpAddr_DonateGP = 0x005C413A;
	// Hook on guild point increasing (0x005C4135)
	static _declspec(naked) void OnDonateGuildPoints()
	{
		// Rebuild asm
		__asm
		{
			mov ecx, dword ptr[eax + 0x3c] // rebuild
			add ecx, esi // rebuild
			cmp ecx, 0x7FFFFFFF // compare ecx with int.MaxValue
			jbe _continue // go to _continue if ecx <= int.MaxValue
			mov ecx, 0x7FFFFFFF // set ecx as int.MaxValue
			jmp _continue // go to _continue
		}
		// Contine code flow
		_continue:
			__asm jmp jmpAddr_DonateGP;
	}

	const char* str_Job = "TreeJob Ranking Updated!!";
	const DWORD a = 0x00936640;
	static __declspec(naked) void ASM_RankUpdate()
	{
		__asm
		{
			push str_Job;
			push 0x1000000;
			call a;
			add esp, 0x10;
			pop edi;
			pop esi;
			pop ebx;
			mov esp, ebp;
			pop ebp;
			ret;
		}
	}
}