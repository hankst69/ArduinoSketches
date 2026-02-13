@echo off
set "_ARDUINO_SKETCHES_DIR=%~dp0"
if "%_ARDUINO_SKETCHES_DIR:~-1%" equ "\" set "_ARDUINO_SKETCHES_DIR=%_ARDUINO_SKETCHES_DIR:~0,-1%"

echo *** updating projects ***

set "_ARDUINO_SKETCHES_PROJECTS_DIR=%_ARDUINO_SKETCHES_DIR%\projects"

set "_AS_espBode_DIR=%_ARDUINO_SKETCHES_DIR%\ESP\AWGControl\espBode"
set "_AS_espBode-awg_DIR=%_ARDUINO_SKETCHES_DIR%\ESP\AWGControl\espBode-awg"
set "_AS_fygen_DIR=%_ARDUINO_SKETCHES_PROJECTS_DIR%\AWGControl\fygen"
set "_AS_SimpleCWKeyer_DIR=%_ARDUINO_SKETCHES_DIR%\unoR3\SimpleCWKeyer"
set "_AS_ATS-Mini_DIR=%_ARDUINO_SKETCHES_DIR%\ESP\ATS-Mini"

set "_AS_espBode_DIR=%_ARDUINO_SKETCHES_PROJECTS_DIR%\espBode"
set "_AS_espBode-awg_DIR=%_ARDUINO_SKETCHES_PROJECTS_DIR%\espBode-awg"
set "_AS_fygen_DIR=%_ARDUINO_SKETCHES_PROJECTS_DIR%\fygen"
set "_AS_SimpleCWKeyer_DIR=%_ARDUINO_SKETCHES_PROJECTS_DIR%\SimpleCWKeyer"
set "_AS_ATS-Mini_DIR=%_ARDUINO_SKETCHES_PROJECTS_DIR%\ATS-Mini"

@rem https://github.com/hankst69/espBode
@rem https://github.com/Hamhackin/espBode
@rem https://github.com/timkoers/espBode
@rem https://github.com/sq6sfo/espBode
if not exist "%_AS_espBode_DIR%" (
  call git clone "https://github.com/hankst69/espBode.git" "%_AS_espBode_DIR%"
)
cd /d "%_AS_espBode_DIR%"
echo updating "%_AS_espBode_DIR%"
call git pull
if not exist "%_AS_espBode-awg_DIR%" (
  call git clone "https://github.com/hankst69/espBode.git" "%_AS_espBode-awg_DIR%"
  git switch awgDevices
)
cd /d "%_AS_espBode-awg_DIR%"
echo updating "%_AS_espBode-awg_DIR%"
call git pull

@rem "https://github.com/hankst69/fygen"
@rem "https://github.com/mattwach/fygen"
rem if not exist "%_AS_fygen_DIR%" (
rem   call git clone "https://github.com/hankst69/fygen.git" "%_AS_fygen_DIR%"
rem )
rem cd /d "%_AS_fygen_DIR%"
rem echo updating "%_AS_fygen_DIR%"
rem call git pull


@rem https://github.com/hankst69/SimpleCWKeyer.git
if not exist "%_AS_SimpleCWKeyer_DIR%" (
  call git clone "https://github.com/hankst69/SimpleCWKeyer.git" "%_AS_SimpleCWKeyer_DIR%"
)
cd /d "%_AS_SimpleCWKeyer_DIR%"
echo updating "%_AS_SimpleCWKeyer_DIR%"
call git pull

@rem https://github.com/hankst69/ats-mini.git
if not exist "%_AS_ATS-Mini_DIR%" (
  call git clone "https://github.com/hankst69/ats-mini.git" "%_AS_ATS-Mini_DIR%"
)
cd /d "%_AS_ATS-Mini_DIR%"
echo updating "%_AS_ATS-Mini_DIR%"
call git pull

echo.
cd /d "%_ARDUINO_SKETCHES_DIR%"