#pragma once

#include <comdef.h>

class Security
{

	struct CLIENT_ID
	{
		HANDLE UniqueProcess;
		HANDLE UniqueThread;
	};

	struct TEB
	{
		NT_TIB NtTib;
		PVOID EnvironmentPointer;
		CLIENT_ID ClientId;
		PVOID ActiveRpcHandle;
		PVOID ThreadLocalStoragePointer;
		struct PEB* ProcessEnvironmentBlock;
	};

	struct PEB_LDR_DATA
	{
		ULONG Length;
		BOOLEAN Initialized;
		HANDLE SsHandle;
		LIST_ENTRY InLoadOrderModuleList;
		LIST_ENTRY InMemoryOrderModuleList;
		LIST_ENTRY InInitializationOrderModuleList;
		PVOID EntryInProgress;
		BOOLEAN ShutdownInProgress;
		HANDLE ShutdownThreadId;
	};

	struct PEB
	{
		BOOLEAN InheritedAddressSpace;
		BOOLEAN ReadImageFileExecOptions;
		BOOLEAN BeingDebugged;
		union
		{
			BOOLEAN BitField;
			struct
			{
				BOOLEAN ImageUsesLargePages : 1;
				BOOLEAN IsProtectedProcess : 1;
				BOOLEAN IsImageDynamicallyRelocated : 1;
				BOOLEAN SkipPatchingUser32Forwarders : 1;
				BOOLEAN IsPackagedProcess : 1;
				BOOLEAN IsAppContainer : 1;
				BOOLEAN IsProtectedProcessLight : 1;
				BOOLEAN SpareBits : 1;
			};
		};
		HANDLE Mutant;
		PVOID ImageBaseAddress;
		PEB_LDR_DATA* Ldr;
	};

	struct UNICODE_STRING
	{
		USHORT Length;
		USHORT MaximumLength;
		PWCH Buffer;
	};

	struct LDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY InLoadOrderLinks;
		LIST_ENTRY InMemoryOrderLinks;
		union
		{
			LIST_ENTRY InInitializationOrderLinks;
			LIST_ENTRY InProgressLinks;
		};
		PVOID DllBase;
		PVOID EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
	};

	static wchar_t* GetFileNameFromPath(wchar_t* Path) {
		wchar_t* LastSlash = NULL;
		for (DWORD i = 0; Path[i] != NULL; i++)
		{
			if (Path[i] == '\\')
				LastSlash = &Path[i + 1];
		}
		return LastSlash;
	}

	static wchar_t* RemoveFileExtension(wchar_t* FullFileName, wchar_t* OutputBuffer, DWORD OutputBufferSize) {
		wchar_t* LastDot = NULL;
		for (DWORD i = 0; FullFileName[i] != NULL; i++)
			if (FullFileName[i] == '.')
				LastDot = &FullFileName[i];

		for (DWORD j = 0; j < OutputBufferSize; j++)
		{
			OutputBuffer[j] = FullFileName[j];
			if (&FullFileName[j] == LastDot)
			{
				OutputBuffer[j] = NULL;
				break;
			}
		}
		OutputBuffer[OutputBufferSize - 1] = NULL;
		return OutputBuffer;
	}

public:
	static const wchar_t* GetWC(const char* c) {
		const size_t cSize = strlen(c) + 1;
		wchar_t* wc = new wchar_t[cSize];
		mbstowcs(wc, c, cSize);

		return wc;
	}

	static __forceinline bool CheckIfThreadIsAlive(HANDLE hnd) {
		DWORD exitCode;
		if (GetExitCodeThread(hnd, &exitCode))
			if (exitCode != STILL_ACTIVE)
				return false;

		return true;
	}

	[[clang::always_inline]] static __forceinline void CorruptMemory()
	{
		int mem = 0x000a1000 + (std::rand() % (0x100b2000 - 0x000a1000 + 1));
		int count = 0x00abcdef + (std::rand() % (0xfafdfa0f - 0x00abcdef + 1));
		int addition = 0x000000ff + (std::rand() % (0xff000000 - 0x000000ff + 1));

		for (int i = 0x00000000; i < count; i += addition)
		{
			unsigned char byte = std::rand() % 255;
			*(reinterpret_cast<char*>(mem + i)) = byte;
		}

		// If after this we for some reason haven't thrown SOME kind of exception, we do now.
		_asm
		{
			xor esp, esp;
			__emit 0xDE;
			__emit 0xF3;
			__emit 0x64;
			__emit 0xCC;
			__emit 0xAD;
			xor eax, eax;
			mov eax, [ebx];
		}
	}

	static __forceinline uintptr_t HdnGetProcAddress(void* hModule, const char* apiName)
	{
#if defined( _WIN32 )   
		unsigned char* lpBase = reinterpret_cast<unsigned char*>(hModule);
		IMAGE_DOS_HEADER* idhDosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(lpBase);
		if (idhDosHeader->e_magic == 0x5A4D)
		{
#if defined( _M_IX86 )  
			IMAGE_NT_HEADERS32* inhNtHeader = reinterpret_cast<IMAGE_NT_HEADERS32*>(lpBase + idhDosHeader->e_lfanew);
#elif defined( _M_AMD64 )  
			IMAGE_NT_HEADERS64* inhNtHeader = reinterpret_cast<IMAGE_NT_HEADERS64*>(lpBase + idhDosHeader->e_lfanew);
#endif  
			if (inhNtHeader->Signature == 0x4550)
			{
				IMAGE_EXPORT_DIRECTORY* iedExportDirectory = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(lpBase + inhNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
				for (unsigned int uiIter = 0; uiIter < iedExportDirectory->NumberOfNames; ++uiIter)
				{
					char* szNames = reinterpret_cast<char*>(lpBase + reinterpret_cast<unsigned long*>(lpBase + iedExportDirectory->AddressOfNames)[uiIter]);
					if (wcscmp(GetWC(apiName), (wchar_t*)GetWC(szNames)) == 0)
					{
						unsigned short usOrdinal = reinterpret_cast<unsigned short*>(lpBase + iedExportDirectory->AddressOfNameOrdinals)[uiIter];
						return reinterpret_cast<uintptr_t>(lpBase + reinterpret_cast<unsigned long*>(lpBase + iedExportDirectory->AddressOfFunctions)[usOrdinal]);
					}
				}
			}
		}
#endif  
		return 0;
	}

	static __forceinline void* HdnGetModuleBase(const char* moduleName) {
#if defined( _WIN64 )  
#define PEBOffset 0x60  
#define LdrOffset 0x18  
#define ListOffset 0x10  
		unsigned long long pPeb = __readgsqword(PEBOffset);
#elif defined( _WIN32 )  
#define PEBOffset 0x30  
#define LdrOffset 0x0C  
#define ListOffset 0x0C  
		unsigned long pPeb = __readfsdword(PEBOffset);
#endif       
		pPeb = *reinterpret_cast<decltype(pPeb)*>(pPeb + LdrOffset);
		LDR_DATA_TABLE_ENTRY* pModuleList = *reinterpret_cast<LDR_DATA_TABLE_ENTRY**>(pPeb + ListOffset);
		while (pModuleList->DllBase)
		{
			if (!wcscmp(pModuleList->BaseDllName.Buffer, GetWC(moduleName)))
				return pModuleList->DllBase;
			pModuleList = reinterpret_cast<LDR_DATA_TABLE_ENTRY*>(pModuleList->InLoadOrderLinks.Flink);
		}
		return nullptr;
	}

	static __forceinline HMODULE WINAPI GetModuleW(_In_opt_ LPCWSTR lpModuleName)
	{
		PEB* ProcessEnvironmentBlock = ((PEB*)((TEB*)((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock));
		if (lpModuleName == nullptr)
			return (HMODULE)(ProcessEnvironmentBlock->ImageBaseAddress);

		PEB_LDR_DATA* Ldr = ProcessEnvironmentBlock->Ldr;

		LIST_ENTRY* ModuleLists[3] = { 0,0,0 };
		ModuleLists[0] = &Ldr->InLoadOrderModuleList;
		ModuleLists[1] = &Ldr->InMemoryOrderModuleList;
		ModuleLists[2] = &Ldr->InInitializationOrderModuleList;
		for (int j = 0; j < 3; j++)
		{
			for (LIST_ENTRY* pListEntry = ModuleLists[j]->Flink;
				pListEntry != ModuleLists[j];
				pListEntry = pListEntry->Flink)
			{
				LDR_DATA_TABLE_ENTRY* pEntry = (LDR_DATA_TABLE_ENTRY*)((BYTE*)pListEntry - sizeof(LIST_ENTRY) * j);

				if (_wcsicmp(pEntry->BaseDllName.Buffer, lpModuleName) == 0)
					return (HMODULE)pEntry->DllBase;

				wchar_t* FileName = GetFileNameFromPath(pEntry->FullDllName.Buffer);
				if (!FileName)
					continue;

				if (_wcsicmp(FileName, lpModuleName) == 0)
					return (HMODULE)pEntry->DllBase;

				wchar_t FileNameWithoutExtension[256];
				RemoveFileExtension(FileName, FileNameWithoutExtension, 256);

				if (_wcsicmp(FileNameWithoutExtension, lpModuleName) == 0)
					return (HMODULE)pEntry->DllBase;
			}
		}
		return nullptr;
	}

	static __forceinline HMODULE WINAPI GetModuleA(_In_opt_ LPCSTR lpModuleName)
	{
		if (!lpModuleName) return GetModuleW(NULL);

		DWORD ModuleNameLength = (DWORD)strlen(lpModuleName) + 1;

		DWORD NewBufferSize = sizeof(wchar_t) * ModuleNameLength;
		wchar_t* W_ModuleName = (wchar_t*)_alloca(NewBufferSize);
		for (DWORD i = 0; i < ModuleNameLength; i++)
			W_ModuleName[i] = lpModuleName[i];

		HMODULE hReturnModule = GetModuleW(W_ModuleName);

		RtlSecureZeroMemory(W_ModuleName, NewBufferSize);

		return hReturnModule;
	}
};