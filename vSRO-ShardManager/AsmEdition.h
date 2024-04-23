#include <Windows.h>
#pragma once
// Direct ASM injection
namespace AsmEdition
{
	// Jump back to the code flow from donating guild points
	static DWORD jmpAddr_DonateGP_01 = 0x004364F3;
	static DWORD jmpAddr_DonateGP_02 = 0x004365D2;
	// Hook on guild point increasing (0x004364EE)
	static _declspec(naked) void OnDonateGuildPoints()
	{
		// Rebuild asm
		__asm
		{
			mov ecx, dword ptr[eax + 0x3c] // rebuild
			add ecx, edi // rebuild
			cmp ecx, 0x7FFFFFFF // compare ecx with int.MaxValue
			jbe _continue // go to _continue if ecx <= int.MaxValue
			mov ecx, 0x7FFFFFFF // set ecx as int.MaxValue
			mov ax, 0x1ABC // set ax with 0x1ABC as custom error code
			jmp _skip_code // go to _skip_code
		}
		// Contine code flow
	_continue:
		__asm jmp jmpAddr_DonateGP_01;
	_skip_code:
		__asm jmp jmpAddr_DonateGP_02;
	}
	// Jump back to the code flow from error code on guild points
	static DWORD jmpAddr_DonateErrCode_01 = 0x00438B99;
	static DWORD jmpAddr_DonateErrCode_02 = 0x00438BA1;
	static DWORD jmpAddr_DonateErrCode_03 = 0x00438B71;
	// Hook on donating guild point error code (0x00438B68)
	static _declspec(naked) void OnDonateGuildPointsErrorCode()
	{
		// Rebuild asm
		__asm
		{
			movzx eax, ax // rebuild
			cmp ax, 0x1 // rebuild
			je _first_case // rebuild
			cmp ax, 0x1ABC // compare ax with 0x1ABC as custom error code
			je _skip_message // go to _skip_message
			jmp _continue // go to _continue
		}
		// Contine code flow
	_first_case:
		__asm jmp jmpAddr_DonateErrCode_01;
	_skip_message:
		__asm jmp jmpAddr_DonateErrCode_02;
	_continue:
		__asm jmp jmpAddr_DonateErrCode_03;
	}
	// Jump back to the code flow from error msg on guild points
	static DWORD jmpAddr_DonateErrMsg_01 = 0x0043AA60;
	static DWORD jmpAddr_DonateErrMsg_02 = 0x0043AA03;
	static DWORD jmpAddr_DonateErrMsg_03 = 0x0043AA26;
	static DWORD callAddr_DonateErrMsg_01 = 0x00438010;
	// Hook on donating guild point error msg (0x0043A9F6)
	static _declspec(naked) void OnDonateGuildPointsErrorMsg()
	{
		// Rebuild asm
		__asm
		{
			movzx eax, ax // rebuild
			cmp ax, 0x1 // rebuild
			mov dword ptr ss : [esp + 0x1C] , eax // rebuild
			je _first_case // rebuild
			cmp ax, 0x1ABC // compare ax with 0x1ABC as custom error code
			jne _continue // go to _continue
			mov eax, dword ptr ss : [esp + 0x10]
			push eax
			call callAddr_DonateErrMsg_01
			add esp, 0x4
			jmp _skip_stuffs
		}
		// Contine code flow
	_first_case:
		__asm jmp jmpAddr_DonateErrMsg_01;
	_continue:
		__asm jmp jmpAddr_DonateErrMsg_02;
	_skip_stuffs:
		__asm jmp jmpAddr_DonateErrMsg_03;
	}



	DWORD HonorString = 0x00784F14;
	DWORD JobString = 0x0077DD9D;
	const char* callJobString = "{?=CALL _UpdateTriangleJobWeekly}";
	const char* callString1 = "Failed To Update Training Camp Honor Ranking! Query Error: %d!";
	const char* callString2 = "Failed To Update Training Camp Honor Ranking! Result: %d";

	const DWORD calla = 0x006B6BD0;
	const DWORD calla1 = 0x006B28D0;
	const DWORD callb = 0x00459A5D;
	const DWORD callc = 0x006863D0;
	const DWORD calld = 0x00402DF0;
	const DWORD calle = 0x00412120;
	static _declspec(naked) void ASM_TreeJob()
	{
		__asm
		{
			sub esp, 0x10;
			cmp dword ptr ds : [0x008A8948] , 0x0;
			push esi;
			mov dword ptr ss : [esp + 0x8] , 0x0;
			mov dword ptr ss : [esp + 0xC] , 0x0;
			jne update_job;
			call calla;
		update_job:
			mov eax, dword ptr ds : [0x008A8948] ;
			mov eax, dword ptr ds : [eax + 0x8] ;
			lea ecx, ss: [esp + 0x8] ;
			push ecx;
			push 0x3;
			push callJobString;
			push 0x7;
			push eax;
			call calla1;
			add esp, 0x14;
			test eax, eax;
			jne update_honor;
			call calla;
		update_honor:
			mov eax, dword ptr ds : [0x008A8948] ;
			mov eax, dword ptr ds : [eax + 0x8] ;
			lea ecx, ss: [esp + 0x8] ;
			push ecx;
			push 0x3;
			push HonorString;
			push 0x7;
			push eax;
			call calla1;
			add esp, 0x14;
			test eax, eax;
			jge a;
			push eax;
			push callString1;
			push 0x1000000;
			call callc;
			add esp, 0xC;
			pop esi;
			add esp, 0x10;
			ret;
		a:
			mov eax, dword ptr ss : [esp + 0x8] ;
			test eax, eax;
			jg b;
			push eax;
			push callString2;
			push 0x1000000;
			call callc;
			add esp, 0xC;
			pop esi;
			add esp, 0x10;
			ret;
		b:
			mov eax, dword ptr ds : [0x00858868] ;
			mov edx, dword ptr ds : [eax] ;
			push 0x0;
			push eax;
			mov eax, dword ptr ds : [edx + 0x48] ;
			call eax;
			mov esi, eax;
			mov ecx, dword ptr ds : [esi + 0x1050] ;
			lea edx, ss: [esp + 0x7] ;
			mov word ptr ds : [ecx] , 0x3C80;
			push edx;
			mov eax, 0x1;
			mov ecx, esi;
			mov byte ptr ss : [esp + 0xB] , 0xB;
			call calld;
			cmp dword ptr ds : [0x008A88F8] , 0x0;
			jne c;
			call calla;
		c:
			push 0x0;
			push esi;
			call calle;
			pop esi;
			add esp, 0x10;
			ret;
		}
	}






}