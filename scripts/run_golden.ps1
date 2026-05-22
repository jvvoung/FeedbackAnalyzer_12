# Run Domain Golden Master regression tests (GM-D-01~04).
#
# Usage:
#   .\scripts\run_golden.ps1
#   .\scripts\run_golden.ps1 -BuildDir build -Verbose

[CmdletBinding()]
param(
    [string] $BuildDir = "build",
    [switch] $SkipBuild,
    [switch] $CtestVerbose
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "_Common.ps1")

Require-Command cmake
Require-Command ctest

$repoRoot = Get-RepoRoot

if (-not $SkipBuild) {
    if (-not (Test-Path (Join-Path $repoRoot $BuildDir))) {
        Invoke-RepoCmakeConfigure -BuildDir $BuildDir | Out-Null
    }
    Invoke-RepoCmakeBuild -BuildDir $BuildDir | Out-Null
}

Write-Host ""
Write-Host "Running Golden Master tests (expect 4/4 Pass, 1 Capture skipped)..." -ForegroundColor Cyan
Invoke-Ctest -BuildDir $BuildDir -Filter "GoldenMaster" -Verbose:$CtestVerbose

Write-Host ""
Write-Host "Golden Master gate: PASS" -ForegroundColor Green
