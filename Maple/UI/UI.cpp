#include "UI.h"

#include "ThemidaSDK.h"
#include "GL/gl3w.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#include "stb_image.h"
#include "PatternScanning/VanillaPatternScanner.h"
#include "Hooking/VanillaHooking.h"

#include "StyleProvider.h"
#include "../Logging/Logger.h"
#include "../SDK/GL/GLControl.h"
#include "../Storage/StorageConfig.h"
#include "Visualisations/SnowVisualisation/SnowVisualisation.h"
#include "Windows/MainMenu.h"
#include "Windows/ScoreSubmissionDialog.h"
#include "../Features/Spoofer/Spoofer.h"
#include "../Features/Enlighten/Enlighten.h"
#include "../Features/AimAssist/AimAssist.h"
#include "../Features/Relax/Relax.h"
#include "../Utilities/Security/xorstr.hpp"

#include <TlHelp32.h>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT UI::wndProcHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode > 0)
		return CallNextHookEx(oWndProc, nCode, wParam, lParam);

	MSG* pMsg = reinterpret_cast<MSG*>(lParam);
	pMsg->wParam = mapLeftRightKeys(pMsg->wParam, pMsg->lParam);

	if (wParam == PM_REMOVE)
	{
		if (pMsg->message == WM_KEYUP && pMsg->wParam == Config::Relax::ToggleKey)
			Relax::IsRunning = !Relax::IsRunning;

		if (pMsg->message == WM_KEYUP && pMsg->wParam == StorageConfig::MenuKey)
			MainMenu::ToggleVisibility();

		if (MainMenu::GetIsVisible() && pMsg->message == WM_KEYUP && pMsg->wParam == VK_ESCAPE)
		{
			ImGui_ImplWin32_WndProcHandler(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);

			pMsg->message = WM_NULL;
			MainMenu::Hide();
		}
		else if (MainMenu::GetIsVisible() || ScoreSubmissionDialog::GetIsVisible())
			ImGui_ImplWin32_WndProcHandler(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
	}

	if (MainMenu::GetIsVisible() || ScoreSubmissionDialog::GetIsVisible())
	{
		if (pMsg->message == WM_CHAR)
			pMsg->message = WM_NULL;

		if ((pMsg->message >= WM_MOUSEMOVE && pMsg->message <= WM_MOUSELAST) || pMsg->message == WM_MOUSEWHEEL)
			pMsg->message = WM_NULL;

		return true;
	}

	return CallNextHookEx(oWndProc, nCode, wParam, lParam);
}

BOOL __stdcall UI::getKeyboardStateHook(PBYTE arr)
{
	if (MainMenu::GetIsVisible() || ScoreSubmissionDialog::GetIsVisible())
		return false;
	
	return oGetKeyboardState(arr);
}

HDC _hdc;
BOOL _declspec(naked) UI::wglSwapBuffersHook(HDC hdc)
{
	__asm
	{
		mov ecx, [ebp + 8]
		mov [_hdc], ecx
		xor ecx, ecx
		pushad
		pushfd
		call wglSwapBuffersWorker
		popfd
		popad
		jmp oWglSwapBuffers
	}
}

void UI::wglSwapBuffersWorker()
{
	if (!initialized)
		initialize(WindowFromDC(_hdc));

	if (!Spoofer::Initialized)
		Spoofer::Initialize();

	render();
}

IDirect3DSwapChain9* _pSwapChain;
int _declspec(naked) UI::presentHook(IDirect3DSwapChain9* pSwapChain, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags)
{
	__asm
	{
		mov [_pSwapChain], ecx - 0x20
		pushad
		pushfd
		call presentWorker
		popfd
		popad
		jmp oPresent
	}
}

void UI::presentWorker()
{
	if (!initialized)
	{
		IDirect3DDevice9* d3dDevice;
		D3DPRESENT_PARAMETERS d3dPresentParameters;

		_pSwapChain->GetDevice(&d3dDevice);
		_pSwapChain->GetPresentParameters(&d3dPresentParameters);

		initialize(d3dPresentParameters.hDeviceWindow, d3dDevice);
	}

	if (!Spoofer::Initialized)
		Spoofer::Initialize();

	render();
}

void UI::enableRaw()
{
	if (!rawInputDisabled)
		return;

	rawInputDisabled = false;

	if (uiDeviceCount == -1)
		return;

	if (!uiDeviceCount)
		return;

	RegisterRawInputDevices(pDevices, uiDeviceCount, sizeof(RAWINPUTDEVICE));
}

void UI::disableRaw()
{
	if (rawInputDisabled)
		return;

	rawInputDisabled = true;

	backupRaw();

	if (uiDeviceCount == -1)
		return;

	if (!uiDeviceCount)
		return;

	for (unsigned int i = 0; i < uiDeviceCount; i++)
	{
		RAWINPUTDEVICE device;
		memcpy(&device, pDevices + i, sizeof(RAWINPUTDEVICE));
		device.dwFlags = RIDEV_REMOVE;
		device.hwndTarget = NULL;
		RegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE));
	}
}

void UI::backupRaw()
{
	uiDeviceCount = -1;
	GetRegisteredRawInputDevices(NULL, &uiDeviceCount, sizeof(RAWINPUTDEVICE));

	delete[] pDevices;

	pDevices = new RAWINPUTDEVICE[uiDeviceCount];
	GetRegisteredRawInputDevices(pDevices, &uiDeviceCount, sizeof(RAWINPUTDEVICE));
}

WPARAM UI::mapLeftRightKeys(WPARAM vk, LPARAM lParam)
{
	WPARAM new_vk = vk;
	UINT scancode = (lParam & 0x00ff0000) >> 16;
	int extended = (lParam & 0x01000000) != 0;

	switch (vk) {
	case VK_SHIFT:
		new_vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
		break;
	case VK_CONTROL:
		new_vk = extended ? VK_RCONTROL : VK_LCONTROL;
		break;
	case VK_MENU:
		new_vk = extended ? VK_RMENU : VK_LMENU;
		break;
	default:
		new_vk = vk;
		break;
	}

	return new_vk;
}

void UI::initialize(HWND window, IDirect3DDevice9* d3d9Device)
{
	if (d3d9Device)
		Renderer = Renderer::D3D9;

	D3D9Device = d3d9Device;
	Window = window;

	stbi_set_flip_vertically_on_load(0);
	stbi_set_flip_vertically_on_load_thread(0);

	ImGui::CreateContext();

	if (Renderer == Renderer::OGL3)
	{
		gl3wInit();
		ImGui_ImplOpenGL3_Init();
	}
	else
		ImGui_ImplDX9_Init(D3D9Device);

	ImGui::StyleColorsDark();

	StyleProvider::LoadFonts();
	StyleProvider::LoadTextures();
	StyleProvider::UpdateColours();
	StyleProvider::UpdateScale();

	ImGui_ImplWin32_Init(Window);
	
	oWndProc = SetWindowsHookEx(WH_GETMESSAGE, wndProcHook, NULL, GetCurrentThreadId());

	if (StorageConfig::ShowMenuAfterInjection)
		MainMenu::Show();
	
	initialized = true;
}

void UI::render()
{
	if ((MainMenu::GetIsVisible() || ScoreSubmissionDialog::GetIsVisible()) && !rawInputDisabled)
		disableRaw();
	else if (!MainMenu::GetIsVisible() && !ScoreSubmissionDialog::GetIsVisible() && rawInputDisabled)
		enableRaw();

	if (Renderer == Renderer::OGL3)
		ImGui_ImplOpenGL3_NewFrame();
	else
		ImGui_ImplDX9_NewFrame();

	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();
	if (MainMenu::GetIsVisible() || ScoreSubmissionDialog::GetIsVisible())
		io.MouseDrawCursor = true;
	else
		io.MouseDrawCursor = false;

	if (Config::Visuals::UI::Snow && (MainMenu::GetIsVisible() || ScoreSubmissionDialog::GetIsVisible()))
		SnowVisualisation::Render();
	
	MainMenu::Render();
	ScoreSubmissionDialog::Render();

	Enlighten::Render();
	AimAssist::Render();

	ImGui::Render();

	if (Renderer == Renderer::OGL3)
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	else
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

//todo: jesus fucking christ get rid of this fuckery
void DoSuspendThread(DWORD targetProcessId, DWORD targetThreadId)
{
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h != INVALID_HANDLE_VALUE)
	{
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		if (Thread32First(h, &te))
		{
			if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
			{
				if (te.th32ThreadID != targetThreadId && te.th32OwnerProcessID == targetProcessId)
				{
					HANDLE thread = ::OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
					if (thread != NULL)
					{
						SuspendThread(thread);
						CloseHandle(thread);
					}
				}
			}
		}
		CloseHandle(h);
	}
}

void DoResumeThread(DWORD targetProcessId, DWORD targetThreadId)
{
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h != INVALID_HANDLE_VALUE)
	{
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		if (Thread32First(h, &te))
		{
			if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
			{
				if (te.th32ThreadID != targetThreadId && te.th32OwnerProcessID == targetProcessId)
				{
					HANDLE thread = ::OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
					if (thread != NULL)
					{
						ResumeThread(thread);
						CloseHandle(thread);
					}
				}
			}
		}
		CloseHandle(h);
	}
}

void UI::Initialize()
{
	STR_ENCRYPT_START

	DoSuspendThread(GetCurrentProcessId(), GetCurrentThreadId());

	void* pGetKeyboardState = GetProcAddress(GetModuleHandleA(xorstr_("user32.dll")), xorstr_("GetKeyboardState"));
	if (VanillaHooking::InstallHook(xorstr_("User32::GetKeyboardState"), reinterpret_cast<uintptr_t>(pGetKeyboardState), reinterpret_cast<uintptr_t>(getKeyboardStateHook), reinterpret_cast<uintptr_t*>(&oGetKeyboardState)) == VanillaResult::Success)
		Logger::Log(LogSeverity::Info, xorstr_("Hooked User32::GetKeyboardState"));
	else
		Logger::Log(LogSeverity::Error, xorstr_("Failed to hook User32::GetKeyboardState"));

	if (GLControl::GetUsesAngle())
	{
		if (const uintptr_t pPresent = VanillaPatternScanner::FindPatternInModule(xorstr_("FF 75 14 8D 49 E0"), xorstr_("d3d9.dll")))
		{
			if (VanillaHooking::InstallHook(xorstr_("D3D9::Present"), pPresent, reinterpret_cast<uintptr_t>(presentHook), reinterpret_cast<uintptr_t*>(&oPresent)) == VanillaResult::Success)
				Logger::Log(LogSeverity::Info, xorstr_("Hooked D3D9::Present"));
			else
				Logger::Log(LogSeverity::Error, xorstr_("Failed to hook D3D9::Present"));
		}
		else Logger::Log(LogSeverity::Error, xorstr_("Failed to hook D3D9::Present"));
	}
	else
	{
		if (const uintptr_t pWglSwapBuffers = VanillaPatternScanner::FindPatternInModule(xorstr_("53 56 57 8B 7D 08 33 DB 8B CF E8"), xorstr_("opengl32.dll")))
		{
			if (VanillaHooking::InstallHook(xorstr_("OpenGL32::wglSwapBuffers"), pWglSwapBuffers, reinterpret_cast<uintptr_t>(wglSwapBuffersHook), reinterpret_cast<uintptr_t*>(&oWglSwapBuffers)) == VanillaResult::Success)
				Logger::Log(LogSeverity::Info, xorstr_("Hooked OpenGL32::wglSwapBuffers"));
			else
				Logger::Log(LogSeverity::Error, xorstr_("Failed to hook OpenGL32::wglSwapBuffers"));
		}
		else Logger::Log(LogSeverity::Error, xorstr_("Failed to hook OpenGL32::wglSwapBuffers"));
	}

	DoResumeThread(GetCurrentProcessId(), GetCurrentThreadId());

	STR_ENCRYPT_END
}
