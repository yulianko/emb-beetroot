param(
    [string]$projectDir
)

if (-not $projectDir) {
    $projectDir = "."
}

if (!(Test-Path $projectDir)) {
    New-Item -ItemType Directory -Path $projectDir
}

& pio init `
    -d $projectDir `
    --board blackpill_f401cc `
    -O platform=ststm32 `
    -O framework=arduino `
    -O upload_protocol=stlink `
    -O debug_tool=stlink `
    -O monitor_speed=115200 `
    -O check_tool=clangtidy `
    -O check_skip_packages=yes `
    -O "build_flags= -D PIO_FRAMEWORK_ARDUINO_ENABLE_CDC -D USBCON"

$mainPath = Join-Path -Path $projectDir -ChildPath "src\main.cpp"
$srcDir = Split-Path -Path $mainPath -Parent
if (!(Test-Path $srcDir)) {
    New-Item -ItemType Directory -Path $srcDir -Force | Out-Null
}
Set-Content -Path $mainPath -Value @'
#include <Arduino.h>

void setup() {}

void loop() {}
'@

& code $projectDir
