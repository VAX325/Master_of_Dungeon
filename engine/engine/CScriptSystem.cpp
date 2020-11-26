#include "CScriptSystem.h"

#include "Client/ClientPerems.h"
#include "Server/ServerPerems.h"

#include "Main.h"

#include <typeinfo>
#include <string>

#include "Base_include.h"

using namespace luabridge;

#pragma comment(lib, "lua-5.2.4.lib")

CScriptSystem::CScriptSystem()
{


}

CScriptSystem::~CScriptSystem()
{
	
}

void CScriptSystem::LuaStart(lua_State* L)
{
	Files fls;

	if (IsClient())
	{
		fs = GetFileSystemObjCl();
		fls = fs->GetAllFilesInFolder((char*)"../gamedata/scripts_cl/", (char*)"sc");
	}
	else
	{
		fs = GetFileSystemObj();
		fls = fs->GetAllFilesInFolder((char*)"../gamedata/scripts_sv/", (char*)"sc");
	}

	auto it = fls.begin();

	char* buffer = (char*)malloc(512);

	while (it != fls.end())
	{
		if (IsClient())
		{
			strcpy(buffer, "../gamedata/scripts_cl/");
		}
		else
		{
			strcpy(buffer, "../gamedata/scripts_sv/");
		}

		strcat(buffer, it->second);

		Scripts[it->second] = buffer;

		luaL_dofile(L, buffer);

		ZeroMemory(buffer, 512);

		it++;
	}

	lua_pcall(L, 0, 0, 0);

	LuaRef mainLua = getGlobal(L, "Main");

	try {
		mainLua();
	}
	catch (const LuaException& e) {
		if (IsClient())
		{
			GetLogObjCl()->LogError((char*)e.what(), true);
		}
		else
		{
			GetLogObj()->LogError((char*)e.what(), true);
		}
	}
}

void CScriptSystem::LuaUpdate(lua_State* L)
{
	luabridge::LuaRef LuaUpdateFunc = getGlobal(L, "Update");

	try {
		LuaUpdateFunc();
	}
	catch (const LuaException& e) {
		if (IsClient())
		{
			GetLogObjCl()->LogError((char*)e.what(), true);
		}
		else
		{
			GetLogObj()->LogError((char*)e.what(), true);
		}
	}
}