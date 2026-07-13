#include "mocks/MockStageService.h"

bool MockStageService::moveStage(const StageControlSettings &settings)
{
    m_lastSettings = settings;
    return true;
}

bool MockStageService::autoFocus()
{
    m_bAutoFocusRequested = true;
    return true;
}

StageControlSettings MockStageService::lastSettings() const
{
    return m_lastSettings;
}

bool MockStageService::autoFocusRequested() const
{
    return m_bAutoFocusRequested;
}
