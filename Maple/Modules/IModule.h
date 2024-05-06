#pragma once

#include "Numerics/Vector2.h"

#include "../IMapleComponent.h"
#include "../INameable.h"
#include "../Structs/Input/CursorState.h"
#include "../Structs/Input/KeyState.h"

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

    // todo: revisit OnCursorPositionUpdate and OnGameplayKeysUpdate

    /**
     * \brief Executed each time osu! tries to update cursor position
     * \param state Current cursor state
     * \return New cursor state to set, the decision on whether Maple should resync is taken by the last module to return its new cursor state. (???)
     */
    virtual CursorState __fastcall OnCursorPositionUpdate(CursorState state)
    {
        return state;
    }
    /**
     * \brief Executed each time osu! tries to update gameplay keypresses
     * \param state Current state of gameplay keys
     * \return New state of gameplay keys to set
     */
    virtual KeyState __fastcall OnGameplayKeysUpdate(KeyState state)
    {
        return state;
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
