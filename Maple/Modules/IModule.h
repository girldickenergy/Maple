#pragma once

#include "Gameplay/Keys/OsuKeys.h"
#include "Numerics/Vector2.h"

#include "../MapleBase.h"
#include "../IMapleComponent.h"
#include "../INameable.h"

class IModule : public IMapleComponent, public INameable
{
public:
    /**
     * \brief Executed on each Main Menu draw call\n
     * Use this to render your module's tab.
     */
    virtual void __fastcall OnMenuRender() {}
    /**
     * \brief Executed on each Overlay draw call\n
     * Use this to render your visuals.
     */
    virtual void __fastcall OnOverlayRender() {}

    /**
     * \brief Executed each time osu! tries to update cursor position
     * \param currentPosition Current cursor position
     * \return New cursor position to set
     */
    virtual Vector2 __fastcall OnCursorPositionUpdate(Vector2 currentPosition)
    {
        return currentPosition;
    }
    /**
     * \brief Executed each time osu! tries to update gameplay keypresses
     * \param currentKeys Currently pressed gameplay keys
     * \return New gameplay keys to set
     */
    virtual OsuKeys __fastcall OnGameplayKeysUpdate(OsuKeys currentKeys)
    {
        return currentKeys;
    }
    /**
     * \brief Executed on each Player initialization
     */
    virtual void __fastcall OnPlayerLoad() {}
    /**
     * \brief Executed each time osu! tries to submit a score (also doubles as a Player exit event)
     * \return Whether or not a score should be submitted
     */
    virtual bool __fastcall OnScoreSubmission()
    {
        return true;
    }

    /**
     * \brief Whether or not this Module will handle OnCursorPositionUpdate event
     */
    virtual bool __fastcall RequiresCursorPosition()
    {
        return false;
    }
    /**
     * \brief Whether or not this Module will handle OnGameplayKeysUpdate event
     */
    virtual bool __fastcall RequiresGameplayKeys()
    {
        return false;
    }
};
