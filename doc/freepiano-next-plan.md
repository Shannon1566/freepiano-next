# freepiano-next 技术方案

## Summary

`freepiano-next` 是一个 Windows x64 虚拟钢琴软件，使用 **Qt 6 + QML** 构建前端，使用 **C++20** 构建音频、VST3 Host 和事件处理后端。第一版目标是实现鼠标点击虚拟琴键、窗口焦点内电脑键盘弹奏，并通过 **VST3 mda Piano** 发声。

第一版不实现外接 MIDI 设备输入，但预留 MIDI 输入接口；不实现录音、回放、音频导出、插件市场或复杂配置编辑。

## Technology Stack

- 构建系统：CMake 3.27+
- 语言标准：C++20
- 前端：Qt 6.5+、Qt Quick、QML、Qt Quick Controls Basic
- 后端集成：Qt C++ `QObject` / `Q_PROPERTY` / `Q_INVOKABLE`
- 音频输出：Windows WASAPI shared mode
- 插件系统：Steinberg VST3 SDK 3.8.x，源码位于 `D:\00proj\vst3sdk`
- 默认音源：VST3 SDK 示例 bundle `D:\00proj\vst3sdk\build-vs2026\VST3\Release\mda-vst3.vst3` 中的 `mda Piano` class
- 配置格式：JSON
- 测试：编译构建测试
- 目标平台：Windows 10/11 x64

## Architecture

- `ui`
  - QML 主界面、虚拟钢琴键盘、状态栏、基础设置面板。
  - 负责鼠标点击、鼠标拖动换键、窗口焦点内键盘输入。
- `core`
  - `PianoController` 作为 QML 与后端之间的唯一主要桥接对象。
  - 接收 `noteOn`、`noteOff`、`sustain`、`panic` 等高层命令。
- `input`
  - `KeyboardInputMapper`：把 Qt key event 映射为 MIDI note。
  - `MousePianoInput`：由 QML 触发，不直接接触音频线程。
  - `IMidiInputProvider`：预留外接 MIDI 输入接口，v1 使用 `NullMidiInputProvider`。
- `midi`
  - `MidiEventRouter`：统一处理 note-on、note-off、sustain、重复按键、all-notes-off。
  - 输出标准 MIDI 1.0 note/controller 事件。
- `audio`
  - `WasapiAudioEngine`：管理 WASAPI 设备、buffer、音频线程生命周期。
  - 音频线程只读取事件队列并调用 VST3 process，不访问 QML、不做文件 IO、不动态分配内存。
- `vst3`
  - `Vst3Host`：加载、初始化和驱动 VST3 Instrument。
  - 第一版只加载默认 bundle `D:\00proj\vst3sdk\build-vs2026\VST3\Release\mda-vst3.vst3`，并选择其中的 `mda Piano` class。
  - 后续再扩展用户选择插件和系统插件扫描。

## Local VST3 Paths

- VST3 SDK 根目录：`D:\00proj\vst3sdk`
- 已生成的 VS2026 构建目录：`D:\00proj\vst3sdk\build-vs2026`
- VST3 示例插件产物目录：`D:\00proj\vst3sdk\build-vs2026\VST3\Release`
- 默认音源 bundle：`D:\00proj\vst3sdk\build-vs2026\VST3\Release\mda-vst3.vst3`
- VST3 host 工具产物目录：`D:\00proj\vst3sdk\build-vs2026\bin\Release`
- VST3 静态库产物目录：`D:\00proj\vst3sdk\build-vs2026\lib\Release`
- SDK 必要接口目录：`D:\00proj\vst3sdk\pluginterfaces`
- SDK 基础工具目录：`D:\00proj\vst3sdk\base`
- VST3 hosting 源码目录：`D:\00proj\vst3sdk\public.sdk\source\vst\hosting`
- VST3 utility 源码目录：`D:\00proj\vst3sdk\public.sdk\source\vst\utility`
- 可参考的 editor host 示例：`D:\00proj\vst3sdk\public.sdk\samples\vst-hosting\editorhost`
- 可参考的 audio host 示例：`D:\00proj\vst3sdk\public.sdk\samples\vst-hosting\audiohost`

## Public Interfaces

QML 只调用高层接口，不直接发送底层 VST3 事件：

```cpp
class PianoController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool audioRunning READ audioRunning NOTIFY audioRunningChanged)
    Q_PROPERTY(bool instrumentLoaded READ instrumentLoaded NOTIFY instrumentLoadedChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

public:
    Q_INVOKABLE void noteOn(int note, int velocity = 100);
    Q_INVOKABLE void noteOff(int note);
    Q_INVOKABLE void setSustain(bool enabled);
    Q_INVOKABLE void setOctaveShift(int semitones);
    Q_INVOKABLE void panic();
    Q_INVOKABLE void loadDefaultInstrument();
};
```

核心 MIDI 事件结构：

```cpp
struct MidiEvent {
    uint8_t status;
    uint8_t data1;
    uint8_t data2;
    uint8_t data3;
    uint32_t sampleOffset;
    InputSource source;
};
```

键盘映射使用 JSON：

```json
{
  "id": "default",
  "octaveShift": 0,
  "keys": {
    "A": { "note": 60, "velocity": 100 },
    "S": { "note": 62, "velocity": 100 },
    "D": { "note": 64, "velocity": 100 }
  }
}
```

## Implementation Plan

- 初始化工程：
  - 使用 CMake 初始化 Qt6 QML 应用。
  - v1 直接引用本机 VST3 SDK 绝对路径 `D:\00proj\vst3sdk`；工程稳定后再裁剪到 `third_party\vst3sdk-minimal`。
- 实现前端：
  - QML 首屏直接显示可弹奏钢琴界面。
  - 白键和黑键使用稳定尺寸布局，鼠标按下触发 `noteOn`，释放触发 `noteOff`。
  - 键盘输入仅在窗口 active focus 内响应。
- 实现事件层：
  - 建立 UI 线程到音频线程的线程安全 MIDI 事件队列。
  - 处理重复 keydown，避免系统按键重复造成重复 note-on。
  - 窗口失焦、音频停止、插件重载时执行 `panic()`。
- 实现音频层：
  - 使用 WASAPI shared mode 打开默认输出设备。
  - 默认稳定优先，buffer 目标约 10-30ms。
  - 音频回调中拉取 MIDI 事件并调用 VST3 process。
- 实现 VST3 Host：
  - 加载 `D:\00proj\vst3sdk\build-vs2026\VST3\Release\mda-vst3.vst3`。
  - 从 bundle class list 中选择 `mda Piano` 对应的 Instrument/AudioEffect class。
  - 初始化 component、controller、process setup、event input 和 audio output bus。
  - 支持 note-on、note-off、sustain controller。
- 打包默认音源：
  - 将 `D:\00proj\vst3sdk\build-vs2026\VST3\Release\mda-vst3.vst3` 复制到应用可发现的 `plugins` 或 `instruments` 目录。
  - 启动时优先加载该默认插件。

## Test Plan

- 鼠标输入：用户手动操作
- 键盘输入：用户手动操作
- 音频与插件：用户手动操作

## Assumptions

- 第一版仅支持 Windows x64。
- 第一版只加载默认 VST3 钢琴插件，不做系统插件扫描。
- 第一版暂不实现外接 MIDI 设备输入，但接口必须保留。
- 第一版不实现录音、回放、导出、皮肤系统或复杂键位编辑器。
- 音频以稳定为优先，不追求极限低延迟。
