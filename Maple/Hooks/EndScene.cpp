#include "Hooks.h"

#include "../UI/Overlay.h"

HRESULT Hooks::endSceneHook(IDirect3DDevice9* pDevice)
{
    if (!Overlay::Initialized)
        Overlay::Initialize(pDevice);

    Overlay::Render();

    return oEndScene(pDevice);
}