# Runs Sports Arcade native automation tests on a Windows machine with Unreal Engine 5.5.
# Requires a successful SportsArcadeEditor build first.
param(
    [Parameter(Mandatory=$true)][string]$EngineRoot,
    [string]$LogPath = ''
)
$ErrorActionPreference = 'Stop'
$EngineRoot = (Resolve-Path $EngineRoot).Path
$Project = (Resolve-Path (Join-Path $PSScriptRoot '..\..\SportsArcade.uproject')).Path
$Editor = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$VersionFile = Join-Path $EngineRoot 'Engine\Build\Build.version'
if (-not (Test-Path $VersionFile)) { throw "Missing Build.version: $VersionFile" }
$Version = Get-Content -Raw $VersionFile | ConvertFrom-Json
if ($Version.MajorVersion -ne 5 -or $Version.MinorVersion -ne 5) {
    throw "Sports Arcade requires UE 5.5 (found $($Version.MajorVersion).$($Version.MinorVersion))."
}
if (-not (Test-Path $Editor)) { throw "Missing UnrealEditor.exe: $Editor" }
$Arguments = @(
    ('"' + $Project + '"'),
    '-unattended',
    '-nop4',
    '-nosplash',
    '-nullrhi',
    '-nosound',
    '-ExecCmds="Automation RunTests SportsArcade"',
    '-TestExit="Automation Test Queue Empty"',
    '-log'
)
if ($LogPath) {
    $ResolvedLogPath = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($LogPath)
    & $Editor @Arguments 2>&1 | Tee-Object -FilePath $ResolvedLogPath
} else {
    & $Editor @Arguments
}
$ExitCode = $LASTEXITCODE
if ($ExitCode -ne 0) { throw "Unreal native automation command returned exit code $ExitCode." }
Write-Host 'Unreal automation process exited successfully. Inspect the test log for per-test results.'
