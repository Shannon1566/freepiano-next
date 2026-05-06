# freepiano-next

freepiano-next 是一个 Windows x64 虚拟钢琴原型，使用 Qt 6/QML 和 C++20
开发。当前工程包含 QML 钢琴界面、键盘/鼠标输入路由、WASAPI 音频输出和
VST3 音源加载。

## 许可证与第三方组件

本项目源码使用 MIT License，详见 `LICENSE`。

项目内包含仓库化的 Steinberg VST3 SDK 组件，位于 `third_party/vst3sdk`。
VST3 SDK 使用 MIT License；本仓库保留了原始版权声明和 license 文本。
第三方组件说明见 `THIRD_PARTY_NOTICES.md`。

## 构建

```powershell
$env:QTFRAMEWORK_BYPASS_LICENSE_CHECK='1'
& 'D:\Program Files\VS\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' -S . -B build
& 'D:\Program Files\VS\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' --build build --config Release
```

运行测试：

```powershell
$env:PATH='D:\Qt\6.11.0\msvc2022_64\bin;' + $env:PATH
& 'D:\Program Files\VS\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe' --test-dir build -C Release --output-on-failure
```

完整 v1 计划见 `doc/freepiano-next-plan.md`。

## 本地 VST 音源

`vstplugs/` 用于存放本机第三方 VST 音源文件。该目录内容不会纳入 git；
构建 `freepiano-next` 时，CMake 会将其中内容复制到程序运行目录的 `plugins/`
子目录。
