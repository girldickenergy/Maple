#include "GameBase.h"

#include <Vanilla.h>

void GameBase::Initialize()
{
	RawGameBase = Vanilla::Explorer["osu.GameBase"];

	instanceAddress = RawGameBase["Instance"].Field.GetAddress();
	modeAddress = RawGameBase["Mode"].Field.GetAddress();
	clientBoundsField = RawGameBase["ClientBounds"].Field;

	RawGameBase["get_Window"].Method.Compile();
	getWindow = (fnGetWindow)RawGameBase["get_Window"].Method.GetNativeStart();

	Vanilla::Explorer["osu.Framework.WindowsGameWindow"]["get_Handle"].Method.Compile();
	getHandle = (fnGetHandle)Vanilla::Explorer["osu.Framework.WindowsGameWindow"]["get_Handle"].Method.GetNativeStart();

	clientHashAddress = RawGameBase["ClientHash"].Field.GetAddress();
	uniqueIDAddress = RawGameBase["UniqueId"].Field.GetAddress();
	uniqueID2Address = RawGameBase["UniqueId2"].Field.GetAddress();
	uniqueCheckAddress = RawGameBase["UniqueCheck"].Field.GetAddress();

	TypeExplorer obfuscatedStringType = RawGameBase["UniqueId"].Field.GetTypeUnsafe();

	obfuscatedStringChangesField = obfuscatedStringType["c"].Field;

	obfuscatedStringType["get_Value"].Method.Compile();
	obfuscatedStringGetValue = static_cast<fnObfuscatedStringGetValue>(obfuscatedStringType["get_Value"].Method.GetNativeStart());

	obfuscatedStringType["set_Value"].Method.Compile();
	obfuscatedStringSetValue = static_cast<fnObfuscatedStringSetValue>(obfuscatedStringType["set_Value"].Method.GetNativeStart());
}

void* GameBase::Instance()
{
	return *static_cast<void**>(instanceAddress);
}

OsuModes GameBase::Mode()
{
	return *static_cast<OsuModes*>(modeAddress);
}

sRectangle* GameBase::GetClientBounds()
{
	return static_cast<sRectangle*>(clientBoundsField.GetAddress());
}

HWND GameBase::GetWindowHandle()
{
	void* windowInstance = getWindow(Instance());

	return getHandle(windowInstance);
}

std::wstring GameBase::GetClientHash()
{
	return (*(COMString**)clientHashAddress)->Data().data();
}

void GameBase::SetClientHash(const std::wstring& clientHash)
{
	*(COMString**)clientHashAddress = COMString::CreateString(clientHash.c_str());
}

std::wstring GameBase::GetUniqueID()
{
	void* uniqueIDInstance = *static_cast<void**>(uniqueIDAddress);

	return obfuscatedStringGetValue(uniqueIDInstance)->Data().data();
}

void GameBase::SetUniqueID(const std::wstring& uniqueID)
{
	void* uniqueIDInstance = *static_cast<void**>(uniqueIDAddress);
	obfuscatedStringSetValue(uniqueIDInstance, COMString::CreateString(uniqueID.c_str()));

	void* changesFieldAddress = obfuscatedStringChangesField.GetAddress(uniqueIDInstance);
	*(int*)changesFieldAddress = *(int*)changesFieldAddress - 1;
}

std::wstring GameBase::GetUniqueID2()
{
	void* uniqueID2Instance = *static_cast<void**>(uniqueID2Address);

	return obfuscatedStringGetValue(uniqueID2Instance)->Data().data();
}

void GameBase::SetUniqueID2(const std::wstring& uniqueID2)
{
	void* uniqueID2Instance = *static_cast<void**>(uniqueID2Address);
	obfuscatedStringSetValue(uniqueID2Instance, COMString::CreateString(uniqueID2.c_str()));

	void* changesFieldAddress = obfuscatedStringChangesField.GetAddress(uniqueID2Instance);
	*(int*)changesFieldAddress = *(int*)changesFieldAddress - 1;
}

std::wstring GameBase::GetUniqueCheck()
{
	void* uniqueCheckInstance = *static_cast<void**>(uniqueCheckAddress);

	return obfuscatedStringGetValue(uniqueCheckInstance)->Data().data();
}

void GameBase::SetUniqueCheck(const std::wstring& uniqueCheck)
{
	void* uniqueCheckInstance = *static_cast<void**>(uniqueCheckAddress);
	obfuscatedStringSetValue(uniqueCheckInstance, COMString::CreateString(uniqueCheck.c_str()));

	void* changesFieldAddress = obfuscatedStringChangesField.GetAddress(uniqueCheckInstance);
	*(int*)changesFieldAddress = *(int*)changesFieldAddress - 1;
}
