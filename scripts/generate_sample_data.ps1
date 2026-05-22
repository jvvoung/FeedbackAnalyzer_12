# Generate data/*.csv from test_feedback_trend.csv using SentimentClassifier + FileHandler.
#
# Usage:
#   .\scripts\generate_sample_data.ps1
#   .\scripts\generate_sample_data.ps1 -SkipBuild
#
# Outputs:
#   data/test_feedback_trend.csv      (input — committed)
#   data/feedback_evaluated.csv       (date,text,sentiment — auto-filled)
#   data/filtered_feedback.csv        (FileHandler BOM + text column)

[CmdletBinding()]
param(
    [string] $BuildDir = "build",
    [switch] $SkipBuild
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "_Common.ps1")

$repoRoot = Get-RepoRoot
$dataDir = Join-Path $repoRoot "data"
$inputCsv = Join-Path $dataDir "test_feedback_trend.csv"

if (-not (Test-Path $inputCsv)) {
    throw "Missing input CSV: $inputCsv"
}

Require-Command cmake

if (-not $SkipBuild) {
    if (-not (Test-Path (Join-Path $repoRoot $BuildDir))) {
        Invoke-RepoCmakeConfigure -BuildDir $BuildDir | Out-Null
    }
    Write-Host ">> cmake --build $BuildDir --target generate_sample_data" -ForegroundColor Cyan
    & cmake --build (Join-Path $repoRoot $BuildDir) --target generate_sample_data
    if ($LASTEXITCODE -ne 0) {
        throw "build generate_sample_data failed (exit $LASTEXITCODE)"
    }
}

$exe = Get-BuildExecutable -BuildDir $BuildDir -Name "generate_sample_data.exe"
Write-Host ">> $exe $repoRoot" -ForegroundColor Cyan
& $exe $repoRoot
if ($LASTEXITCODE -ne 0) {
    throw "generate_sample_data failed (exit $LASTEXITCODE)"
}

Write-Host ""
Write-Host "Sample data ready in data/" -ForegroundColor Green
Get-ChildItem $dataDir -Filter "*.csv" | ForEach-Object { Write-Host "  $($_.Name)" }
