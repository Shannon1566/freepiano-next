#include "audio/WasapiAudioEngine.h"

#include "vst3/Vst3Host.h"

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

WasapiAudioEngine::WasapiAudioEngine(QObject *parent)
    : QObject(parent)
    , m_statusText(QStringLiteral("Audio engine idle"))
{
}

WasapiAudioEngine::~WasapiAudioEngine()
{
    stop();
}

bool WasapiAudioEngine::start()
{
#ifndef Q_OS_WIN
    setStatusText(QStringLiteral("Audio output requires Windows"));
    return false;
#else
    if (m_audioThread.joinable()) {
        return true;
    }

    m_stopRequested = false;
    m_audioThread = std::thread(&WasapiAudioEngine::audioLoop, this);
    setRunning(true);
    setStatusText(QStringLiteral("WASAPI output rendering VST3"));
    return true;
#endif
}

void WasapiAudioEngine::stop()
{
    m_stopRequested = true;
    if (m_audioThread.joinable()) {
        m_audioThread.join();
    }
    setRunning(false);
    setStatusText(QStringLiteral("Audio engine stopped"));
}

bool WasapiAudioEngine::isRunning() const
{
    return m_running;
}

QString WasapiAudioEngine::statusText() const
{
    return m_statusText;
}

void WasapiAudioEngine::setVst3Host(Vst3Host *host)
{
    m_vst3Host = host;
}

void WasapiAudioEngine::submitEvents(const QVector<MidiEvent> &events)
{
    Q_UNUSED(events);
}

void WasapiAudioEngine::audioLoop()
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
        setStatusText(message);
        setRunning(false);
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
    m_sampleRate = static_cast<double>(mixFormat->nSamplesPerSec);

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

    while (!m_stopRequested) {
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
        if (m_vst3Host) {
            m_vst3Host->render(out, static_cast<int>(framesToWrite), channels, m_sampleRate);
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

void WasapiAudioEngine::setRunning(bool running)
{
    if (m_running == running) {
        return;
    }

    m_running = running;
    emit runningChanged();
}

void WasapiAudioEngine::setStatusText(const QString &text)
{
    if (m_statusText == text) {
        return;
    }

    m_statusText = text;
    emit statusTextChanged();
}
