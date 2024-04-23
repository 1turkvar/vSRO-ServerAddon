#pragma once
#include <Windows.h>
#include <stdio.h>

template<typename T>
int addr_from_this(T funptr) {
	union  {
		int addr;
		T ptr;
	} myu;

	myu.ptr = funptr;
	return myu.addr;
}

template<typename T>
void placeHook(int trampoline_location, T& target_location)
{
	placeHook(trampoline_location, reinterpret_cast<int>(&target_location));
}

static bool placeHook(int trampoline_location, int target_location)
{

	char jmp_inst[] = { (char)0xE9, 0x00, 0x00, 0x00, 0x00 };
	int distance;
	DWORD dwProtect = 0;

	distance = target_location - trampoline_location - 5;

	// Write jump-distance to instruction
	memcpy((jmp_inst + 1), &distance, 4);

	if (!VirtualProtect((LPVOID)trampoline_location, sizeof(jmp_inst), PAGE_EXECUTE_READWRITE, &dwProtect)) {
		perror("Failed to unprotect memory\n");
		return false;
	}

	memcpy((LPVOID)trampoline_location, jmp_inst, sizeof(jmp_inst));

	DWORD otherProtect;
	if (!VirtualProtect((LPVOID)trampoline_location, sizeof(jmp_inst), dwProtect, &otherProtect)) {
		perror("Failed to restore protection on memory");
	}	return true;
}

static bool replaceOffset(int trampoline_location, int target_location)
{

	char inst_offset[] = { 0x00, 0x00, 0x00, 0x00 };
	int distance;
	DWORD dwProtect = 0;

	int offset_location = trampoline_location + 1;

	distance = target_location - trampoline_location - 5;

	// Write jump-distance to instruction
	memcpy(inst_offset, &distance, 4);

	if (!VirtualProtect((LPVOID)offset_location, sizeof(inst_offset), PAGE_EXECUTE_READWRITE, &dwProtect)) {
		perror("Failed to unprotect memory\n");
		return false;
	}

	memcpy((LPVOID)offset_location, inst_offset, sizeof(inst_offset));

	DWORD otherProtect;
	if (!VirtualProtect((LPVOID)offset_location, sizeof(inst_offset), dwProtect, &otherProtect)) {
		perror("Failed to restore protection on memory");
	}
	return true;
}

static void replaceOffsetEdit(int trampoline_location, int target_location)
{
	char inst_offset[] = { 0x00, 0x00, 0x00, 0x00 };
	int distance;
	DWORD dwProtect = 0;

	int offset_location = trampoline_location + 1;

	distance = target_location;

	// Write jump-distance to instruction
	memcpy(inst_offset, &distance, 4);

	if (!VirtualProtect((LPVOID)offset_location, sizeof(inst_offset), PAGE_EXECUTE_READWRITE, &dwProtect)) {
		perror("Failed to unprotect memory\n");
		return;
	}

	memcpy((LPVOID)offset_location, inst_offset, sizeof(inst_offset));

	DWORD otherProtect;
	if (!VirtualProtect((LPVOID)offset_location, sizeof(inst_offset), dwProtect, &otherProtect)) {
		perror("Failed to restore protection on memory");
	}
}

static void replaceAddr(int addr, int value)
{
	DWORD dwProtect;

	if (!VirtualProtect((LPVOID)addr, sizeof(int), PAGE_EXECUTE_READWRITE, &dwProtect)) {
		perror("Failed to unprotect memory\n");
		return;
	}

	*((int*)addr) = value;

	DWORD otherProtect;
	if (!VirtualProtect((LPVOID)addr, sizeof(int), dwProtect, &otherProtect)) {
		perror("Failed to restore protection on memory");
	}
}

static void vftableHook(unsigned int vftable_addr, int offset, int target_func)
{
	replaceAddr(vftable_addr + offset*sizeof(void*), target_func);
}

// Calls a virtual function from table pointer address
template<typename T>
T CallVirtual(void* vftable_addr, unsigned index)
{
	return (*static_cast<T**>(vftable_addr))[index];
}