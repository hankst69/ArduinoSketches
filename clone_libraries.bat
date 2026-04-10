@echo off
set "_ARDUINO_SKETCHES_DIR=%~dp0"
if "%_ARDUINO_SKETCHES_DIR:~-1%" equ "\" set "_ARDUINO_SKETCHES_DIR=%_ARDUINO_SKETCHES_DIR:~0,-1%"

echo *** updating libraries ***

@rem https://github.com/arduino/library-registry/blob/main/FAQ.md#how-can-i-add-a-library-to-library-manager
set "_ARDUINO_LIBRARIES_DIR=%_ARDUINO_SKETCHES_DIR%\libraries"


rem https://github.com/felis/USB_Host_Shield_2.0
set "_APPLETV_JB_UnoHostLib_DIR=%_ARDUINO_LIBRARIES_DIR%\USB_Host_Shield_2.0"
if not exist "%_APPLETV_JB_UnoHostLib_DIR%" (
  call git clone "https://github.com/felis/USB_Host_Shield_2.0.git" "%_APPLETV_JB_UnoHostLib_DIR%"
  rem now we automatically patch the downloaded usb_host_shield_lib with usb_host_library.patch from checkm8-a5 ...
  cd /d "%_APPLETV_JB_UnoHostLib_DIR%"
  call git checkout cd87628af4a693eeafe1bf04486cf86ba01d29b8
  call git clone "https://github.com/hankst69/checkm8-a5.git" "%_APPLETV_JB_UnoHostLib_DIR%\checkm8-a5"
  call git apply "%_APPLETV_JB_UnoHostLib_DIR%\checkm8-a5\usb_host_library.patch"
  rmdir /s /q "%_APPLETV_JB_UnoHostLib_DIR%\checkm8-a5"
)


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