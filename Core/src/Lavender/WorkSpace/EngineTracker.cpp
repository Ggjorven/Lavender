#include "lvpch.h"
#include "EngineTracker.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender::Track
{

    bool Viewport::InView(const glm::uvec2& mainWindowPosition)
    {
        if (mainWindowPosition.x >= Viewport::Position.x && mainWindowPosition.x <= Viewport::Position.x + Viewport::Width &&
            mainWindowPosition.y >= Viewport::Position.y && mainWindowPosition.y <= Viewport::Position.y + Viewport::Height)
        {
            return true;
        }

        return false;
    }

}