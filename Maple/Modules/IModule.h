#pragma once

#include "../MapleBase.h"
#include "../SDK/OsuKeys.h"
#include "../SDK/Vector2.h"

class IModule
{
protected:
    std::shared_ptr<MapleBase> m_MapleBase;

public:
    virtual void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase)
    {
        m_MapleBase = mapleBase;
    }

    /**
     * \brief Executed on each GUI draw call
     */
    virtual void __fastcall OnRender() = 0;

    /**
     * \brief Executed each time osu! tries to update cursor position
     * \param currentPosition Current cursor position
     * \return New cursor position to set
     */
    virtual Vector2 __fastcall OnCursorPositionUpdate(Vector2 currentPosition) = 0;
    /**
     * \brief Executed each time osu! tries to update gameplay keypresses
     * \param currentKeys Currently pressed gameplay keys
     * \return New gameplay keys to set
     */
    virtual OsuKeys __fastcall OnGameplayKeysUpdate(OsuKeys currentKeys) = 0;

    /**
     * \brief Executed on each Player initialization
     */
    virtual void __fastcall OnPlayerLoad() = 0;
    /**
     * \brief Executed on each Player exit
     */
    virtual void __fastcall OnPlayerExit() = 0;

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
     * \returns This Module's name
     */
    virtual std::string GetName() = 0;
};
