#pragma once

#include "../MapleBase.h"
#include "../IMapleComponent.h"
#include "../INameable.h"
#include "../SDK/OsuKeys.h"
#include "../SDK/Vector2.h"

class IModule : public IMapleComponent, public INameable
{
public:
    /**
     * \brief Executed on each GUI draw call
     */
    virtual void __fastcall OnRender() {}

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
     * \brief Executed each time osu! tries to submit a score
     * \return Whether or not a score should be submitted
     */
    virtual bool __fastcall OnScoreSubmission()
    {
        return true;
    }

    /**
     * \brief Executed on each Player initialization
     */
    virtual void __fastcall OnPlayerLoad() {}
    /**
     * \brief Executed on each Player exit
     */
    virtual void __fastcall OnPlayerExit() {}

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
    /**
     * \brief Whether or not this Module will handle OnScoreSubmission event
     */
    virtual bool __fastcall RequiresScoreSubmission()
    {
        return false;
    }
};
