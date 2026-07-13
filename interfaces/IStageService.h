#ifndef ISTAGESERVICE_H
#define ISTAGESERVICE_H

#include "models/StageControlSettings.h"

class IStageService
{
public:
    virtual ~IStageService() = default;

    virtual bool moveStage(const StageControlSettings &settings) = 0;
    virtual bool autoFocus() = 0;
};

#endif // ISTAGESERVICE_H
