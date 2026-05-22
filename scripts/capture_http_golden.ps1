# Capture HTTP/CSV Golden Master baselines (Phase 2+ — local capture helper).
#
# Starts feedback_analyzer.exe, curls endpoints, normalizes timestamps, and writes:
#   tests/golden/http/H01_analyze_positive.html  (stat snippet)
#   tests/golden/http/H02_filter_no_data.html    (alert snippet)
#   tests/golden/http/H04_upload_stats.html      (alert snippet)
#   tests/golden/csv/H03_filtered_download.csv   (BOM + text column CSV)
#
# Usage:
#   .\scripts\capture_http_golden.ps1
#   .\scripts\capture_http_golden.ps1 -Port 8080 -SkipBuild
#
# Note: HTTP Golden tests are not yet wired into ctest — this script prepares baseline files only.

[CmdletBinding()]
param(
    [string] $BuildDir = "build",
    [int] $Port = 8080,
    [string] $HostName = "127.0.0.1",
    [switch] $SkipBuild,
    [int] $StartupWaitSec = 2
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "_Common.ps1")

Require-Command curl.exe

$repoRoot = Get-RepoRoot
$httpDir = Join-Path $repoRoot "tests\golden\http"
$csvDir = Join-Path $repoRoot "tests\golden\csv"
New-Item -ItemType Directory -Path $httpDir -Force | Out-Null
New-Item -ItemType Directory -Path $csvDir -Force | Out-Null

if (-not $SkipBuild) {
    if (-not (Test-Path (Join-Path $repoRoot $BuildDir))) {
        Invoke-RepoCmakeConfigure -BuildDir $BuildDir | Out-Null
    }
    Invoke-RepoCmakeBuild -BuildDir $BuildDir | Out-Null
}

$exePath = Get-BuildExecutable -BuildDir $BuildDir
$baseUrl = "http://${HostName}:$Port"

function Invoke-CurlBody {
    param(
        [string] $Method,
        [string] $Url,
        [string] $Data = "",
        [string] $ContentType = "application/x-www-form-urlencoded"
    )

    $args = @("-sS", "-X", $Method, $Url)
    if ($Data) {
        $args += "-H", "Content-Type: $ContentType", "-d", $Data
    }

    $output = & curl.exe @args 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "curl failed ($Method $Url): $output"
    }
    return [string]$output
}

function Invoke-CurlUpload {
    param(
        [string] $Url,
        [string] $FilePath
    )

    $output = & curl.exe -sS -X POST $Url -F "file=@$FilePath" 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "curl upload failed ($Url): $output"
    }
    return [string]$output
}

function Save-GoldenText {
    param(
        [string] $Path,
        [string] $Content
    )

    $text = Normalize-GoldenLineEndings (Mask-HttpTimestamps $Content)
    [System.IO.File]::WriteAllText($Path, $text, [System.Text.UTF8Encoding]::new($false))
    Write-Host "  wrote $Path" -ForegroundColor Green
}

function Save-GoldenBytes {
    param(
        [string] $Path,
        [byte[]] $Bytes
    )

    [System.IO.File]::WriteAllBytes($Path, $Bytes)
    Write-Host "  wrote $Path ($($Bytes.Length) bytes)" -ForegroundColor Green
}

Write-Host ""
Write-Host "Starting server: $exePath" -ForegroundColor Cyan
$server = Start-Process -FilePath $exePath -PassThru -WindowStyle Hidden

try {
    Start-Sleep -Seconds $StartupWaitSec

    if ($server.HasExited) {
        throw "Server exited immediately (exit $($server.ExitCode))"
    }

    Write-Host "Capturing HTTP Golden baselines from $baseUrl ..." -ForegroundColor Cyan

    # GM-H-01: analyze negative delivery → stat region
    $analyzeText = [System.Text.Encoding]::UTF8.GetString(
        [byte[]](0xEB,0xB0,0xB0,0xEC,0x86,0xA1,0xEC,0x9D,0xB4,0x20,0xEB,0x84,0x88,0xEB,0xAC,0xB4,0x20,
                 0xEB,0x8A,0xA6,0xEC,0x96,0xB4,0xEC,0x9A,0x94,0x2E,0x20,0xED,0x99,0x94,0xEA,0xB0,0x80,
                 0x20,0xEB,0x82,0xA9,0xEB,0x8B,0x88,0xEB,0x8B,0xA4,0x2E))
    $h01Body = Invoke-CurlBody -Method POST -Url "$baseUrl/analyze" -Data ("text=" + [uri]::EscapeDataString($analyzeText))
    $h01Stats = Extract-HtmlStatsSection $h01Body
    if (-not $h01Stats) {
        $h01Stats = Extract-HtmlAlerts $h01Body
    }
    Save-GoldenText -Path (Join-Path $httpDir "H01_analyze_positive.html") -Content $h01Stats

    # GM-H-02: filter with no session → warning alert
    $h02Body = Invoke-CurlBody -Method POST -Url "$baseUrl/filter" `
        -Data "sentiment=$([uri]::EscapeDataString('중립'))&keyword=$([uri]::EscapeDataString('전체'))"
    $h02Alerts = Extract-HtmlAlerts $h02Body
    Save-GoldenText -Path (Join-Path $httpDir "H02_filter_no_data.html") -Content $h02Alerts

    # GM-H-03: upload CSV (text column, 2 rows)
    $csvUpload = "text`n택배가 빨라요.`n좋아요 만족합니다.`n"
    $tempCsv = Join-Path $env:TEMP "fa_golden_upload.csv"
    [System.IO.File]::WriteAllText($tempCsv, $csvUpload, [System.Text.UTF8Encoding]::new($false))
    $h03Body = Invoke-CurlUpload -Url "$baseUrl/upload" -FilePath $tempCsv
    Remove-Item $tempCsv -ErrorAction SilentlyContinue
    $h03Alerts = Extract-HtmlAlerts $h03Body
    Save-GoldenText -Path (Join-Path $httpDir "H04_upload_stats.html") -Content $h03Alerts

    # GM-H-04: analyze → filter → download CSV
    Invoke-CurlBody -Method POST -Url "$baseUrl/analyze" -Data ("text=" + [uri]::EscapeDataString($analyzeText)) | Out-Null
    Invoke-CurlBody -Method POST -Url "$baseUrl/filter" `
        -Data "sentiment=$([uri]::EscapeDataString('전체'))&keyword=$([uri]::EscapeDataString('전체'))" | Out-Null

    $downloadTemp = Join-Path $env:TEMP "fa_golden_download.csv"
    & curl.exe -sS "$baseUrl/download" -o $downloadTemp
    if ($LASTEXITCODE -ne 0) {
        throw "curl download failed (exit $LASTEXITCODE)"
    }
    Save-GoldenBytes -Path (Join-Path $csvDir "H03_filtered_download.csv") -Bytes ([System.IO.File]::ReadAllBytes($downloadTemp))
    Remove-Item $downloadTemp -ErrorAction SilentlyContinue

    Write-Host ""
    Write-Host "HTTP/CSV golden capture complete." -ForegroundColor Green
    Write-Host "Review tests/golden/http/ and tests/golden/csv/ before commit." -ForegroundColor Yellow
    Write-Host "Timestamps masked as {{TIMESTAMP}}; wire into ctest in a future Phase 2+ turn." -ForegroundColor DarkGray
}
finally {
    if (-not $server.HasExited) {
        Stop-Process -Id $server.Id -Force -ErrorAction SilentlyContinue
    }
}
