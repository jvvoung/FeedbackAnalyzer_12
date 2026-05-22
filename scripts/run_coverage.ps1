# Build with gcov, run ctest, collect lcov, and generate HTML coverage report.
#
# Usage:
#   .\scripts\run_coverage.ps1
#   .\scripts\run_coverage.ps1 -BuildDir build_cov -UpdateBaseline -OpenHtml
#
# Requires: MinGW g++, gcov, lcov, genhtml (MSVC not supported for coverage builds)

[CmdletBinding()]
param(
    [string] $BuildDir = "build_cov",
    [switch] $SkipBuild,
    [switch] $SkipTests,
    [switch] $SkipConfigure,
    [switch] $CleanBuildDir,
    [switch] $UpdateBaseline,
    [switch] $OpenHtml,
    [string] $BaselineName = "coverage_baseline_phase1.info"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "_Common.ps1")

Require-Command cmake
Require-Command ctest
Require-Command lcov
Require-Command genhtml
Assert-MinGwForCoverage

$repoRoot = Get-RepoRoot
$buildPath = Join-Path $repoRoot $BuildDir
$coverageInfo = Join-Path $repoRoot "coverage.info"
$coverageFiltered = Join-Path $repoRoot "coverage.filtered.info"
$htmlDir = Join-Path $buildPath "coverage_html"

if (-not $SkipConfigure) {
    Invoke-RepoCmakeConfigure -BuildDir $BuildDir -Coverage -Clean:$CleanBuildDir | Out-Null
}

if (-not $SkipBuild) {
    Invoke-RepoCmakeBuild -BuildDir $BuildDir | Out-Null
}

if (-not $SkipTests) {
    Write-Host ""
    Write-Host "Running tests with coverage instrumentation..." -ForegroundColor Cyan
    Invoke-Ctest -BuildDir $BuildDir
}

Write-Host ""
Write-Host "Collecting lcov data from $BuildDir..." -ForegroundColor Cyan

& lcov --capture --directory $buildPath --output-file $coverageInfo --rc lcov_branch_coverage=0
if ($LASTEXITCODE -ne 0) {
    throw "lcov --capture failed (exit $LASTEXITCODE)"
}

& lcov --remove $coverageInfo `
    "*/tests/*" "*/googletest/*" "*/httplib.h" "*/main.cpp" `
    --output-file $coverageFiltered `
    --rc lcov_branch_coverage=0
if ($LASTEXITCODE -ne 0) {
    throw "lcov --remove failed (exit $LASTEXITCODE)"
}

if (Test-Path $htmlDir) {
    Remove-Item -Recurse -Force $htmlDir
}
New-Item -ItemType Directory -Path $htmlDir -Force | Out-Null

& genhtml $coverageFiltered --output-directory $htmlDir --legend --title "Feedback Analyzer Coverage"
if ($LASTEXITCODE -ne 0) {
    throw "genhtml failed (exit $LASTEXITCODE)"
}

Write-Host ""
Write-Host "=== Coverage summary (src/cpp) ===" -ForegroundColor Cyan
& lcov --list $coverageFiltered --rc lcov_branch_coverage=0 |
    Select-String -Pattern "Constants|CsvParser|Feedback|FeedbackClassifier|Filters|KeywordRegistry|KeywordUtils|SentimentClassifier|TextAnalyzer|FormParser|HtmlPageRenderer|CsvExporter|RouteHandlers|AppMessages|FileHandler" |
    ForEach-Object { $_.Line }

if ($UpdateBaseline) {
    $baselinePath = Join-Path $repoRoot $BaselineName
    Copy-Item -Force $coverageFiltered $baselinePath
    Write-Host ""
    Write-Host "Baseline updated: $baselinePath" -ForegroundColor Green
}

Write-Host ""
Write-Host "Artifacts:" -ForegroundColor Green
Write-Host "  lcov raw:      $coverageInfo"
Write-Host "  lcov filtered: $coverageFiltered"
Write-Host "  HTML report:   $htmlDir\index.html"

if ($OpenHtml) {
    $indexHtml = Join-Path $htmlDir "index.html"
    if (Test-Path $indexHtml) {
        Start-Process $indexHtml
    }
}

Write-Host ""
Write-Host "Coverage run complete." -ForegroundColor Green
