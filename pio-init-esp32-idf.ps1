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
    --board esp32-s3-devkitc-1 `
    -O framework=espidf `
    -O upload_protocol=esptool `
    -O monitor_speed=115200 `
    -O check_tool=clangtidy `
    -O check_skip_packages=yes

$mainPath = Join-Path -Path $projectDir -ChildPath "src\main.cpp"
$srcDir = Split-Path -Path $mainPath -Parent
if (!(Test-Path $srcDir)) {
    New-Item -ItemType Directory -Path $srcDir -Force | Out-Null
}
Set-Content -Path $mainPath -Value 'extern "C" void app_main(void) {}'

& code $projectDir
