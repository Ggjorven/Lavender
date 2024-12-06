#include "lvpch.h"
#include "EngineTracker.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender::Track
{

    bool Viewport::InView(const glm::uvec2& mainWindowPosition)
    {
        return Utils::ToolKit::PositionInRect(mainWindowPosition, Track::Viewport::Position, { Track::Viewport::Width, Track::Viewport::Height });
    }

}