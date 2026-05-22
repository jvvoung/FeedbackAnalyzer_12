# Regenerate Domain Golden Master baseline (local only — do NOT run in CI).
#
# Usage:
#   .\scripts\update_golden.ps1
#   .\scripts\update_golden.ps1 -BuildDir build -SkipBuild
#
# Equivalent:
#   cmake --build build --target update_golden
#   $env:GOLDEN_UPDATE = "1"; ctest --test-dir build -R GoldenMasterCapture; Remove-Item Env:GOLDEN_UPDATE

[CmdletBinding()]
param(
    [string] $BuildDir = "build",
    [switch] $SkipBuild,
    [switch] $Verify
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "_Common.ps1")

$repoRoot = Get-RepoRoot
$goldenFile = Join-Path $repoRoot "tests\golden\domain_golden_expected.txt"

if ($env:CI -eq "true" -or $env:GITHUB_ACTIONS -eq "true") {
    throw "GOLDEN_UPDATE must not run in CI. Baseline updates are local-only."
}

Require-Command cmake
Require-Command ctest

if (-not $SkipBuild) {
    if (-not (Test-Path (Join-Path $repoRoot $BuildDir))) {
        Invoke-RepoCmakeConfigure -BuildDir $BuildDir | Out-Null
    }
    Invoke-RepoCmakeBuild -BuildDir $BuildDir | Out-Null
}

Write-Host ""
Write-Host "Regenerating Domain Golden Master: $goldenFile" -ForegroundColor Yellow
Write-Host "(GOLDEN_UPDATE=1 — review diff before commit)" -ForegroundColor Yellow
Write-Host ""

$previous = $null
if (Test-Path $goldenFile) {
    $previous = Get-Content -Raw -Encoding UTF8 $goldenFile
}

try {
    $env:GOLDEN_UPDATE = "1"
    Invoke-Ctest -BuildDir $BuildDir -Filter "GoldenMasterCapture" -CtestVerbose
}
finally {
    Remove-Item Env:GOLDEN_UPDATE -ErrorAction SilentlyContinue
}

if (-not (Test-Path $goldenFile)) {
    throw "Golden file was not created: $goldenFile"
}

Write-Host ""
Write-Host "Updated: $goldenFile" -ForegroundColor Green

if ($null -ne $previous -and $previous -ne (Get-Content -Raw -Encoding UTF8 $goldenFile)) {
    Write-Host "Baseline content changed — review git diff before committing." -ForegroundColor Yellow
}

if ($Verify) {
    Write-Host ""
    Write-Host "Verifying Golden Master tests..." -ForegroundColor Cyan
    Invoke-Ctest -BuildDir $BuildDir -Filter "GoldenMaster" -CtestVerbose
}

Write-Host ""
Write-Host "Done. Next: git diff tests/golden/domain_golden_expected.txt" -ForegroundColor Green
