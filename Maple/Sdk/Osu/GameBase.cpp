#include "GameBase.h"

#include <Vanilla.h>

void GameBase::Initialize()
{
	RawGameBase = Vanilla::Explorer["osu.GameBase"];

	instanceAddress = RawGameBase["Instance"].Field.GetAddress();
	modeAddress = RawGameBase["Mode"].Field.GetAddress();

	RawGameBase["get_Window"].Method.Compile();
	getWindow = (fnGetWindow)RawGameBase["get_Window"].Method.GetNativeStart();

	Vanilla::Explorer["osu.Framework.WindowsGameWindow"]["get_Handle"].Method.Compile();
	getHandle = (fnGetHandle)Vanilla::Explorer["osu.Framework.WindowsGameWindow"]["get_Handle"].Method.GetNativeStart();

	clientHashAddress = RawGameBase["ClientHash"].Field.GetAddress();
	uniqueIDAddress = RawGameBase["UniqueId"].Field.GetAddress();
	uniqueID2Address = RawGameBase["UniqueId2"].Field.GetAddress();
	uniqueCheckAddress = RawGameBase["UniqueCheck"].Field.GetAddress();

	TypeExplorer obfuscatedStringType = RawGameBase["UniqueId"].Field.GetTypeUnsafe();

	obfuscatedRandomValueField = obfuscatedStringType["Randomvalue"].Field;
	obfuscatedValueField = obfuscatedStringType["_Value"].Field;
}

void* GameBase::Instance()
{
	return *static_cast<void**>(instanceAddress);
}

OsuModes GameBase::Mode()
{
	return *static_cast<OsuModes*>(modeAddress);
}

HWND GameBase::GetWindowHandle()
{
	void* windowInstance = getWindow(Instance());

	return getHandle(windowInstance);
}

void* GameBase::GetUniqueIDInstance()
{
	return *static_cast<void**>(uniqueIDAddress);
}

void* GameBase::GetUniqueID2Instance()
{
	return *static_cast<void**>(uniqueID2Address);
}

void* GameBase::GetUniqueCheckInstance()
{
	return *static_cast<void**>(uniqueCheckAddress);
}

std::wstring GameBase::GetClientHash()
{
	return (*(COMString**)clientHashAddress)->Data().data();
}

std::wstring GameBase::GetUniqueID()
{
	void* uniqueIDInstance = GetUniqueIDInstance();

	int salt = *(int*)obfuscatedRandomValueField.GetAddress(uniqueIDInstance);
	std::wstring obfuscatedUniqueID = (*(COMString**)obfuscatedValueField.GetAddress(uniqueIDInstance))->Data().data();
	std::wstring deobfuscatedUniqueID = {};

	for (int i = 0; i < obfuscatedUniqueID.length(); i++)
		deobfuscatedUniqueID += obfuscatedUniqueID[i] ^ salt;

	return deobfuscatedUniqueID;
}

std::wstring GameBase::GetUniqueID2()
{
	void* uniqueID2Instance = GetUniqueID2Instance();

	int salt = *(int*)obfuscatedRandomValueField.GetAddress(uniqueID2Instance);
	std::wstring obfuscatedUniqueID2 = (*(COMString**)obfuscatedValueField.GetAddress(uniqueID2Instance))->Data().data();
	std::wstring deobfuscatedUniqueID2 = {};

	for (int i = 0; i < obfuscatedUniqueID2.length(); i++)
		deobfuscatedUniqueID2 += obfuscatedUniqueID2[i] ^ salt;

	return deobfuscatedUniqueID2;
}

std::wstring GameBase::GetUniqueCheck()
{
	void* uniqueCheckInstance = GetUniqueCheckInstance();

	int salt = *(int*)obfuscatedRandomValueField.GetAddress(uniqueCheckInstance);
	std::wstring obfuscatedUniqueCheck = (*(COMString**)obfuscatedValueField.GetAddress(uniqueCheckInstance))->Data().data();
	std::wstring deobfuscatedUniqueCheck = {};

	for (int i = 0; i < obfuscatedUniqueCheck.length(); i++)
		deobfuscatedUniqueCheck += obfuscatedUniqueCheck[i] ^ salt;

	return deobfuscatedUniqueCheck;
}
