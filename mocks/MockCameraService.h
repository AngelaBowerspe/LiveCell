#ifndef MOCKCAMERASERVICE_H
#define MOCKCAMERASERVICE_H

#include "interfaces/ICameraService.h"

class MockCameraService final : public ICameraService
{
public:
    bool applyCaptureSettings(const CaptureSettings &settings) override;
    bool capture(const CaptureSettings &settings) override;
    bool startRecord(const CaptureSettings &settings) override;
    void stopRecord() override;
    bool isRecording() const override;

    CaptureSettings lastSettings() const;

private:
    CaptureSettings m_lastSettings;
    bool m_bRecording = false;
};

#endif // MOCKCAMERASERVICE_H
