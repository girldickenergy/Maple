#include "Overlay.h"

#include "imgui_impl_dx9.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#include "Menus/MainMenu.h"
#include "Menus/ScoreSubmissionDialogue.h"
#include "StyleProvider.h"
#include "../Features/AimAssist/AimAssist.h"
#include "../Sdk/Osu/GameBase.h"
#include "GL/gl3w.h"
#include "Menus/SnowVisualisation.h"
#include "../Features/Visuals/VisualsSpoofers.h"

void Overlay::enableRaw()
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

void Overlay::disableRaw()
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

void Overlay::backupRaw()
{
	uiDeviceCount = -1;
	GetRegisteredRawInputDevices(NULL, &uiDeviceCount, sizeof(RAWINPUTDEVICE));

	delete[] pDevices;

	pDevices = new RAWINPUTDEVICE[uiDeviceCount];
	GetRegisteredRawInputDevices(pDevices, &uiDeviceCount, sizeof(RAWINPUTDEVICE));
}

void Overlay::Initialize(IDirect3DDevice9* d3d9Device)
{
	if (d3d9Device)
		Renderer = Renderer::D3D9;
	
	D3D9Device = d3d9Device;
	
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

	ImGui_ImplWin32_Init(GameBase::GetWindowHandle());

	oHandleInput = SetWindowsHookEx(WH_GETMESSAGE, HandleInputHook, NULL, GetCurrentThreadId());

	Initialized = true;
}

void Overlay::Render()
{
	if ((MainMenu::IsOpen || ScoreSubmissionDialogue::IsOpen) && !rawInputDisabled)
		disableRaw();
	else if (!MainMenu::IsOpen && !ScoreSubmissionDialogue::IsOpen && rawInputDisabled)
		enableRaw();

	if (Renderer == Renderer::OGL3)
		ImGui_ImplOpenGL3_NewFrame();
	else
		ImGui_ImplDX9_NewFrame();

	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();
	if (MainMenu::IsOpen || ScoreSubmissionDialogue::IsOpen)
		io.MouseDrawCursor = true;
	else
		io.MouseDrawCursor = false;

	if (MainMenu::IsOpen || ScoreSubmissionDialogue::IsOpen)
		SnowVisualisation::Draw();

	if (MainMenu::IsOpen)
		MainMenu::Render();

	if (ScoreSubmissionDialogue::IsOpen)
		ScoreSubmissionDialogue::Render();

	AimAssist::DrawDebugOverlay();
	VisualsSpoofers::DrawPreemptiveDots();
	
	ImGui::Render();

	if (Renderer == Renderer::OGL3)
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	else
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Overlay::ToggleMainMenu()
{
	if (!MainMenu::IsOpen && GameBase::Mode() == OsuModes::Play)
		return;
	
	MainMenu::IsOpen = !MainMenu::IsOpen;
}

void Overlay::ShowScoreSubmissionDialogue()
{
	ScoreSubmissionDialogue::IsOpen = true;
}

void Overlay::HideScoreSubmissionDialogue()
{
	ScoreSubmissionDialogue::IsOpen = false;
}

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT Overlay::HandleInputHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode > 0)
		return CallNextHookEx(oHandleInput, nCode, wParam, lParam);

	MSG* pMsg = reinterpret_cast<MSG*>(lParam);
	
	if (wParam == PM_REMOVE)
	{
		if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_DELETE)
			ToggleMainMenu();

		if (MainMenu::IsOpen && pMsg->message == WM_KEYUP && pMsg->wParam == VK_ESCAPE)
		{
			ImGui_ImplWin32_WndProcHandler(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
			
			pMsg->message = WM_NULL;
			MainMenu::IsOpen = false;
		}
		else if (MainMenu::IsOpen || ScoreSubmissionDialogue::IsOpen)
			ImGui_ImplWin32_WndProcHandler(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
	}

	if (MainMenu::IsOpen || ScoreSubmissionDialogue::IsOpen)
	{
		if (pMsg->message == WM_CHAR)
			pMsg->message = WM_NULL;

		if ((pMsg->message >= WM_MOUSEMOVE && pMsg->message <= WM_MOUSELAST) || pMsg->message == WM_MOUSEWHEEL)
			pMsg->message = WM_NULL;

		return true;
	}

	return CallNextHookEx(oHandleInput, nCode, wParam, lParam);
}

BOOL __stdcall Overlay::HandleKeyboardInputHook(PBYTE arr)
{
	if (MainMenu::IsOpen)
		return false;

	return oHandleKeyboardInput(arr);
}
