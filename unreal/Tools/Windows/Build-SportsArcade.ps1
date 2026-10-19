# Windows launch helper; requires local UE 5.5 installation.
param(
    [Parameter(Mandatory=$true)][string]$EngineRoot,
    [switch]$GenerateProjectFiles,
    [switch]$LaunchEditor
)
$ErrorActionPreference = 'Stop'
$Project = (Resolve-Path (Join-Path $PSScriptRoot '..\..\SportsArcade.uproject')).Path
$EngineRoot = (Resolve-Path $EngineRoot).Path
$Build = Join-Path $EngineRoot 'Engine\Build\BatchFiles\Build.bat'
$Editor = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$VersionSelector = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealVersionSelector-Win64-Shipping.exe'
if (-not (Test-Path $Build)) { throw "Unreal Build.bat not found: $Build" }
if ($GenerateProjectFiles) {
    $Generator = Join-Path $EngineRoot 'Engine\Build\BatchFiles\GenerateProjectFiles.bat'
    if (Test-Path $Generator) {
        & $Generator -project="$Project" -game -engine
    } elseif (Test-Path $VersionSelector) {
        & $VersionSelector /projectfiles "$Project"
    } else {
        throw 'Neither GenerateProjectFiles.bat nor UnrealVersionSelector is available.'
    }
    if ($LASTEXITCODE -ne 0) { throw "Project generation failed: $LASTEXITCODE" }
}
& $Build SportsArcadeEditor Win64 Development "-Project=$Project" -WaitMutex
if ($LASTEXITCODE -ne 0) { throw "SportsArcadeEditor build failed: $LASTEXITCODE" }
if ($LaunchEditor) {
    if (-not (Test-Path $Editor)) { throw "UnrealEditor.exe missing: $Editor" }
    Start-Process -FilePath $Editor -ArgumentList ('"' + $Project + '"')
}
Write-Host 'SportsArcadeEditor build command succeeded.'
