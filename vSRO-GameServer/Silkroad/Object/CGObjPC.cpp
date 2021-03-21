#include "CGObjPC.h"
#include "../../Utils/Memory.h"

/* Public Properties */
void CGObjPC::GetGameWorldId(uint32_t* OutGameWorldId)
{
	call_virtual<void(__thiscall*)(CGObjPC*, uint32_t*)>(this, 5)(this, OutGameWorldId);
}
/* Public Methods */
uint16_t CGObjPC::AddItem(const char* Codename, int32_t Amount, bool RandomizeStats, int8_t OptLevel)
{
	int16_t operation_result = 0;
	call_virtual<void(__thiscall*)(CGObjPC*, int32_t, const char*, int32_t, int16_t*, int8_t, int32_t, int32_t, int8_t, int32_t, int32_t, int8_t, int32_t)>(this, 139)(this, 0, Codename, Amount, &operation_result, 0, RandomizeStats ? 1 : 0, 0, OptLevel, 0, 1, 0, 0);
	return operation_result;
}
void CGObjPC::UpdateGold(int64_t Offset)
{
	UpdateGold(Offset, 25, true, true);
}
void CGObjPC::UpdateTitle(uint8_t Level)
{
	reinterpret_cast<void(__thiscall*)(CGObjPC*, uint8_t)>(0x004A9F40)(this, Level);
}
bool CGObjPC::MoveTo(uint16_t RegionId, uint32_t PosX, uint32_t PosY, uint32_t PosZ)
{
	uint32_t gwid = 0;
	GetGameWorldId(&gwid);
	CPosition pos;
	pos.RegionID = RegionId;
	pos.UnkUShort01 = m_Position.UnkUShort01;
	pos.PosX = PosX;
	pos.PosY = PosY;
	pos.PosZ = PosZ;
	return MoveTo(gwid, pos, 2);
}
bool CGObjPC::MoveTo(uint32_t GameWorldId, uint16_t RegionId, uint32_t PosX, uint32_t PosY, uint32_t PosZ)
{
	CPosition pos;
	pos.RegionID = RegionId;
	pos.UnkUShort01 = m_Position.UnkUShort01;
	pos.PosX = PosX;
	pos.PosY = PosY;
	pos.PosZ = PosZ;
	return MoveTo(GameWorldId, pos, 2);
}
/* Private Helpers */
void CGObjPC::UpdateGold(int64_t Offset, int32_t Unknown, bool Realtime, bool ShowMessage)
{
	call_virtual<void(__thiscall*)(CGObjPC*, int64_t, int32_t, int32_t, int32_t)>(this, 91)(this, Offset, Unknown, (Realtime ? 1 : 0), (ShowMessage ? 1 : 0));
}
bool CGObjPC::MoveTo(uint32_t GameWorldId, CPosition Position, int32_t Type)
{
	return call_virtual<bool(__thiscall*)(CGObjPC*, uint32_t*, CPosition, int32_t)>(this, 222)(this, &GameWorldId, Position, Type);
}