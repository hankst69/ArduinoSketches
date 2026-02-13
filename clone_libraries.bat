@echo off
set "_ARDUINO_SKETCHES_DIR=%~dp0"
if "%_ARDUINO_SKETCHES_DIR:~-1%" equ "\" set "_ARDUINO_SKETCHES_DIR=%_ARDUINO_SKETCHES_DIR:~0,-1%"

echo *** updating libraries ***

@rem https://github.com/arduino/library-registry/blob/main/FAQ.md#how-can-i-add-a-library-to-library-manager
set "_ARDUINO_LIBRARIES_DIR=%_ARDUINO_SKETCHES_DIR%\libraries"

rem https://github.com/T-vK/ESP32-BLE-Keyboard.git  https://github.com/hankst69/ESP32-BLE-Keyboard.git
rem https://github.com/T-vK/ESP32-BLE-Mouse.git
rem https://github.com/T-vK/ESP32-BLE-Gamepad.git
rem https://github.com/sobrinho/ESP32-BLE-Abs-Mouse.git
set "_AL_ESP32-BLE-Keyboard_DIR=%_ARDUINO_LIBRARIES_DIR%\ESP32-BLE-Keyboard"
set "_AL_ESP32-BLE-Mouse_DIR=%_ARDUINO_LIBRARIES_DIR%\ESP32-BLE-Mouse"
set "_AL_ESP32-BLE-Gamepad_DIR=%_ARDUINO_LIBRARIES_DIR%\ESP32-BLE-Gamepad"
set "_AL_ESP32-BLE-Abs-Mouse_DIR=%_ARDUINO_LIBRARIES_DIR%\ESP32-BLE-Abs-Mouse"

if not exist "%_AL_ESP32-BLE-Keyboard_DIR%" (
  call git clone "https://github.com/hankst69/ESP32-BLE-Keyboard.git" "%_AL_ESP32-BLE-Keyboard_DIR%"
)
cd /d "%_AL_ESP32-BLE-Keyboard_DIR%"
echo updating "%_AL_ESP32-BLE-Keyboard_DIR%"
call git pull

if not exist "%_AL_ESP32-BLE-Mouse_DIR%" (
  call git clone "https://github.com/T-vK/ESP32-BLE-Mouse.git" "%_AL_ESP32-BLE-Mouse_DIR%"
)
cd /d "%_AL_ESP32-BLE-Mouse_DIR%"
echo updating "%_AL_ESP32-BLE-Mouse_DIR%"
call git pull

if not exist "%_AL_ESP32-BLE-Gamepad_DIR%" (
  call git clone "https://github.com/lemmingDev/ESP32-BLE-Gamepad.git" "%_AL_ESP32-BLE-Gamepad_DIR%"
)
cd /d "%_AL_ESP32-BLE-Gamepad_DIR%"
echo updating "%_AL_ESP32-BLE-Gamepad_DIR%"
call git pull

if not exist "%_AL_ESP32-BLE-Abs-Mouse_DIR%" (
  call git clone "https://github.com/sobrinho/ESP32-BLE-Abs-Mouse.git" "%_AL_ESP32-BLE-Abs-Mouse_DIR%"
)
cd /d "%_AL_ESP32-BLE-Abs-Mouse_DIR%"
echo updating "%_AL_ESP32-BLE-Abs-Mouse_DIR%"
call git pull


cd /d "%_ARDUINO_LIBRARIES_DIR%"