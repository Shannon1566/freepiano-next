#include "audio/FpnWasapiAudioEngine.h"

#include "vst2/FpnVst2Host.h"
#include "vst3/FpnVst3Host.h"

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <audioclient.h>
#include <avrt.h>
#include <mmreg.h>
#include <mmdeviceapi.h>
#include <windows.h>
#include <mmsystem.h>
#endif

#include <algorithm>

namespace {
constexpr int processFrames = 64;
}

FpnWasapiAudioEngine::FpnWasapiAudioEngine(QObject *parent)
    : QObject(parent)
    , m_fpnStatusText(QStringLiteral("Audio engine idle"))
{
}

FpnWasapiAudioEngine::~FpnWasapiAudioEngine()
{
    fpnStop();
}

bool FpnWasapiAudioEngine::fpnStart()
{
#ifndef Q_OS_WIN
    fpnSetStatusText(QStringLiteral("Audio output requires Windows"));
    return false;
#else
    if (m_fpnAudioThread.joinable()) {
        return true;
    }

    m_fpnStopRequested = false;
    m_fpnAudioThread = std::thread(&FpnWasapiAudioEngine::fpnAudioLoop, this);
    fpnSetRunning(true);
    fpnSetStatusText(QStringLiteral("WASAPI output rendering VST3"));
    return true;
#endif
}

void FpnWasapiAudioEngine::fpnStop()
{
    m_fpnStopRequested = true;
    if (m_fpnAudioThread.joinable()) {
        m_fpnAudioThread.join();
    }
    fpnSetRunning(false);
    fpnSetStatusText(QStringLiteral("Audio engine stopped"));
}

bool FpnWasapiAudioEngine::fpnIsRunning() const
{
    return m_fpnRunning;
}

QString FpnWasapiAudioEngine::fpnStatusText() const
{
    return m_fpnStatusText;
}

void FpnWasapiAudioEngine::fpnSetVst3Host(FpnVst3Host *host)
{
    m_fpnVst3Host = host;
}

void FpnWasapiAudioEngine::fpnSetVst2Host(FpnVst2Host *host)
{
    m_fpnVst2Host = host;
}

void FpnWasapiAudioEngine::fpnSetUseVst2Host(bool useVst2Host)
{
    m_fpnUseVst2Host = useVst2Host;
}

void FpnWasapiAudioEngine::fpnSubmitEvents(const QVector<FpnMidiEvent> &events)
{
    Q_UNUSED(events);
}

void FpnWasapiAudioEngine::fpnAudioLoop()
{
#ifdef Q_OS_WIN
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IMMDeviceEnumerator *enumerator = nullptr;
    IMMDevice *device = nullptr;
    IAudioClient *client = nullptr;
    IAudioRenderClient *renderClient = nullptr;
    WAVEFORMATEX *mixFormat = nullptr;
    HANDLE taskHandle = nullptr;
    DWORD taskIndex = 0;

    const auto fail = [this](const QString &message) {
        fpnSetStatusText(message);
        fpnSetRunning(false);
    };

    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
                                  nullptr,
                                  CLSCTX_ALL,
                                  __uuidof(IMMDeviceEnumerator),
                                  reinterpret_cast<void **>(&enumerator));
    if (FAILED(hr)
        || FAILED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device))
        || FAILED(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void **>(&client)))
        || FAILED(client->GetMixFormat(&mixFormat))) {
        fail(QStringLiteral("Failed to open default WASAPI device"));
        goto cleanup;
    }

    if (mixFormat->wFormatTag != WAVE_FORMAT_EXTENSIBLE
        || reinterpret_cast<WAVEFORMATEXTENSIBLE *>(mixFormat)->SubFormat != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT
        || mixFormat->wBitsPerSample != 32) {
        fail(QStringLiteral("Default WASAPI mix format is not 32-bit float"));
        goto cleanup;
    }
    m_fpnSampleRate = static_cast<double>(mixFormat->nSamplesPerSec);

    if (FAILED(client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 100000, 0, mixFormat, nullptr))
        || FAILED(client->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void **>(&renderClient)))) {
        fail(QStringLiteral("Failed to initialize WASAPI render client"));
        goto cleanup;
    }

    taskHandle = AvSetMmThreadCharacteristicsW(L"Pro Audio", &taskIndex);
    timeBeginPeriod(1);

    if (FAILED(client->Start())) {
        fail(QStringLiteral("Failed to start WASAPI audio client"));
        goto cleanup;
    }

    while (!m_fpnStopRequested) {
        UINT32 bufferFrameCount = 0;
        UINT32 paddingFrames = 0;
        if (FAILED(client->GetBufferSize(&bufferFrameCount))
            || FAILED(client->GetCurrentPadding(&paddingFrames))) {
            Sleep(1);
            continue;
        }

        const UINT32 availableFrames = bufferFrameCount - paddingFrames;
        if (availableFrames < processFrames) {
            Sleep(1);
            continue;
        }

        const UINT32 framesToWrite = std::min<UINT32>(availableFrames, processFrames);
        BYTE *data = nullptr;
        if (FAILED(renderClient->GetBuffer(framesToWrite, &data))) {
            Sleep(1);
            continue;
        }

        auto *out = reinterpret_cast<float *>(data);
        const int channels = mixFormat->nChannels;
        if (m_fpnUseVst2Host && m_fpnVst2Host) {
            m_fpnVst2Host->fpnRender(out, static_cast<int>(framesToWrite), channels, m_fpnSampleRate);
        } else if (m_fpnVst3Host) {
            m_fpnVst3Host->fpnRender(out, static_cast<int>(framesToWrite), channels, m_fpnSampleRate);
        } else {
            std::fill(out, out + framesToWrite * channels, 0.0f);
        }

        renderClient->ReleaseBuffer(framesToWrite, 0);
    }

    client->Stop();

cleanup:
    if (taskHandle) {
        AvRevertMmThreadCharacteristics(taskHandle);
    }
    timeEndPeriod(1);
    if (mixFormat) {
        CoTaskMemFree(mixFormat);
    }
    if (renderClient) {
        renderClient->Release();
    }
    if (client) {
        client->Release();
    }
    if (device) {
        device->Release();
    }
    if (enumerator) {
        enumerator->Release();
    }
    CoUninitialize();
#endif
}

void FpnWasapiAudioEngine::fpnSetRunning(bool running)
{
    if (m_fpnRunning == running) {
        return;
    }

    m_fpnRunning = running;
    emit fpnRunningChanged();
}

void FpnWasapiAudioEngine::fpnSetStatusText(const QString &text)
{
    if (m_fpnStatusText == text) {
        return;
    }

    m_fpnStatusText = text;
    emit fpnStatusTextChanged();
}
