#ifndef MOCKSTAGESERVICE_H
#define MOCKSTAGESERVICE_H

#include "interfaces/IStageService.h"

class MockStageService final : public IStageService
{
public:
    bool moveStage(const StageControlSettings &settings) override;
    bool autoFocus() override;

    StageControlSettings lastSettings() const;
    bool autoFocusRequested() const;

private:
    StageControlSettings m_lastSettings;
    bool m_bAutoFocusRequested = false;
};

#endif // MOCKSTAGESERVICE_H
