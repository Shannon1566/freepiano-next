cmake_minimum_required(VERSION 3.27)

project(freepiano-next VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(FREEPIANO_QT_PREFIX "D:/Qt/6.11.0/msvc2022_64" CACHE PATH "Qt 6 installation prefix")
if(EXISTS "${FREEPIANO_QT_PREFIX}/lib/cmake/Qt6/Qt6Config.cmake")
    list(PREPEND CMAKE_PREFIX_PATH "${FREEPIANO_QT_PREFIX}")
endif()
set(FREEPIANO_WINDEPLOYQT "${FREEPIANO_QT_PREFIX}/bin/windeployqt.exe" CACHE FILEPATH "Qt windeployqt executable")
option(FREEPIANO_DEPLOY_QT_RUNTIME "Run windeployqt after building freepiano-next" ON)

set(VST3_SDK_ROOT "${CMAKE_SOURCE_DIR}/third_party/vst3sdk" CACHE PATH "Steinberg VST3 SDK root")
set(VST3_SDK_LIB_DIR "${VST3_SDK_ROOT}/lib/Release" CACHE PATH "Steinberg VST3 SDK binary library directory")
set(DEFAULT_VST3_BUNDLE "${VST3_SDK_ROOT}/plugins/mda-vst3.vst3" CACHE PATH "Default mda Piano VST3 bundle")
set(USER_VST_PLUGINS_DIR "${CMAKE_SOURCE_DIR}/vstplugs" CACHE PATH "Local third-party VST plug-in directory")

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(freepiano-next
    src/main.cpp
    src/audio/FpnWasapiAudioEngine.cpp
    src/audio/FpnWasapiAudioEngine.h
    src/core/FpnPianoController.cpp
    src/core/FpnPianoController.h
    src/input/IFpnMidiInputProvider.h
    src/input/FpnKeyboardInputMapper.cpp
    src/input/FpnKeyboardInputMapper.h
    src/input/FpnNullMidiInputProvider.h
    src/midi/FpnMidiEvent.h
    src/midi/FpnMidiEventRouter.cpp
    src/midi/FpnMidiEventRouter.h
    src/vst2/FpnVst2Host.cpp
    src/vst2/FpnVst2Host.h
    src/vst2/aeffect.h
    src/vst2/aeffectx.h
    src/vst2/vstfxstore.h
    src/vst3/FpnVst3Host.cpp
    src/vst3/FpnVst3Host.h
    ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/plugprovider.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/connectionproxy.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/module_win32.cpp
)

target_compile_definitions(freepiano-next PRIVATE
    FREEPIANO_DEFAULT_VST3_BUNDLE="${DEFAULT_VST3_BUNDLE}"
    FREEPIANO_VST3_SDK_ROOT="${VST3_SDK_ROOT}"
)

target_include_directories(freepiano-next PRIVATE
    src
    ${VST3_SDK_ROOT}
)

target_link_libraries(freepiano-next PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
)

if(WIN32)
    target_link_libraries(freepiano-next PRIVATE
        ole32
        avrt
        mmdevapi
        winmm
        ${VST3_SDK_LIB_DIR}/sdk_hosting.lib
        ${VST3_SDK_LIB_DIR}/sdk_common.lib
        ${VST3_SDK_LIB_DIR}/sdk.lib
        ${VST3_SDK_LIB_DIR}/base.lib
        ${VST3_SDK_LIB_DIR}/pluginterfaces.lib
    )
endif()

if(EXISTS "${DEFAULT_VST3_BUNDLE}")
    add_custom_command(TARGET freepiano-next POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:freepiano-next>/plugins"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${DEFAULT_VST3_BUNDLE}"
            "$<TARGET_FILE_DIR:freepiano-next>/plugins/mda-vst3.vst3"
        COMMENT "Copying default mda Piano VST3 bundle"
    )
endif()

if(IS_DIRECTORY "${USER_VST_PLUGINS_DIR}")
    add_custom_command(TARGET freepiano-next POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:freepiano-next>/plugins"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${USER_VST_PLUGINS_DIR}"
            "$<TARGET_FILE_DIR:freepiano-next>/plugins"
        COMMENT "Copying local third-party VST plug-ins"
    )
endif()

qt_add_qml_module(freepiano-next
    URI FreePiano
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/FpnComputerKeyboard.qml
        qml/FpnPianoKey.qml
        qml/FpnPianoKeyboard.qml
)

if(WIN32 AND FREEPIANO_DEPLOY_QT_RUNTIME)
    if(EXISTS "${FREEPIANO_WINDEPLOYQT}")
        add_custom_command(TARGET freepiano-next POST_BUILD
            COMMAND "${FREEPIANO_WINDEPLOYQT}"
                --dir "$<TARGET_FILE_DIR:freepiano-next>"
                --qmldir "${CMAKE_SOURCE_DIR}/qml"
                "$<TARGET_FILE:freepiano-next>"
            COMMENT "Deploying Qt runtime dependencies"
            VERBATIM
        )
    else()
        message(WARNING "windeployqt not found: ${FREEPIANO_WINDEPLOYQT}")
    endif()
endif()
