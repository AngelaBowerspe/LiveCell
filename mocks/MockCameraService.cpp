#include "mocks/MockCameraService.h"

bool MockCameraService::applyCaptureSettings(const CaptureSettings &settings)
{
    m_lastSettings = settings;
    return true;
}

bool MockCameraService::capture(const CaptureSettings &settings)
{
    m_lastSettings = settings;
    return true;
}

bool MockCameraService::startRecord(const CaptureSettings &settings)
{
    m_lastSettings = settings;
    m_bRecording = true;
    return true;
}

void MockCameraService::stopRecord()
{
    m_bRecording = false;
}

bool MockCameraService::isRecording() const
{
    return m_bRecording;
}

CaptureSettings MockCameraService::lastSettings() const
{
    return m_lastSettings;
}
