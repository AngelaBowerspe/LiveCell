#ifndef ICAMERASERVICE_H
#define ICAMERASERVICE_H

#include "models/CaptureSettings.h"

class ICameraService
{
public:
    virtual ~ICameraService() = default;

    virtual bool applyCaptureSettings(const CaptureSettings &settings) = 0;
    virtual bool capture(const CaptureSettings &settings) = 0;
    virtual bool startRecord(const CaptureSettings &settings) = 0;
    virtual void stopRecord() = 0;
    virtual bool isRecording() const = 0;
};

#endif // ICAMERASERVICE_H
