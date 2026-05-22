# Shared helpers for Feedback Analyzer scripts (PowerShell 5.1+)

function Get-RepoRoot {
    $root = Resolve-Path (Join-Path $PSScriptRoot "..")
    return $root.Path
}

function Require-Command {
    param(
        [Parameter(Mandatory = $true)]
        [string] $Name,

        [string] $InstallHint = ""
    )

    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        $hint = if ($InstallHint) { " $InstallHint" } else { "" }
        throw "Required command not found: $Name.$hint"
    }
}

function Test-MinGwCompiler {
    $gpp = Get-Command g++ -ErrorAction SilentlyContinue
    if (-not $gpp) {
        return $false
    }

    $version = & g++ --version 2>&1 | Select-Object -First 1
    return ($version -match "mingw|MinGW|w64")
}

function Assert-MinGwForCoverage {
    if (-not (Test-MinGwCompiler)) {
        throw @"
Coverage requires MinGW g++ (gcov). MSVC is not supported (u8""..."" compile errors with --coverage).
Install MinGW-w64 and ensure g++, gcov, lcov, and genhtml are on PATH.
Manual fallback:
  cmake -S . -B build_cov -DCMAKE_CXX_FLAGS=""--coverage -O0 -g"" -DCMAKE_EXE_LINKER_FLAGS=""--coverage""
"@
    }
}

function Invoke-RepoCmakeConfigure {
    param(
        [string] $BuildDir = "build",
        [switch] $Coverage,
        [string] $Generator = "",
        [switch] $Clean
    )

    $repoRoot = Get-RepoRoot
    $buildPath = Join-Path $repoRoot $BuildDir

    if ($Clean -and (Test-Path $buildPath)) {
        Write-Host ">> Removing $buildPath" -ForegroundColor Yellow
        Remove-Item -Recurse -Force $buildPath
    }

    $args = @("-S", $repoRoot, "-B", $buildPath, "-DCMAKE_BUILD_TYPE=Debug")
    if ($Coverage) {
        Assert-MinGwForCoverage
        $args += "-DENABLE_COVERAGE=ON"
        if (-not $Generator) {
            $Generator = "MinGW Makefiles"
        }
        $args += "-DCMAKE_CXX_COMPILER=g++", "-DCMAKE_C_COMPILER=gcc"
    }
    if ($Generator) {
        $args += "-G", $Generator
    }

    Write-Host ">> cmake $($args -join ' ')" -ForegroundColor Cyan
    & cmake @args
    if ($LASTEXITCODE -ne 0) {
        throw "cmake configure failed (exit $LASTEXITCODE)"
    }

    return $buildPath
}

function Invoke-RepoCmakeBuild {
    param(
        [string] $BuildDir = "build"
    )

    $repoRoot = Get-RepoRoot
    $buildPath = Join-Path $repoRoot $BuildDir

    Write-Host ">> cmake --build $buildPath" -ForegroundColor Cyan
    & cmake --build $buildPath
    if ($LASTEXITCODE -ne 0) {
        throw "cmake build failed (exit $LASTEXITCODE)"
    }

    return $buildPath
}

function Get-BuildExecutable {
    param(
        [string] $BuildDir = "build",
        [string] $Name = "feedback_analyzer.exe"
    )

    $repoRoot = Get-RepoRoot
    $candidates = @(
        (Join-Path $repoRoot "$BuildDir\$Name"),
        (Join-Path $repoRoot "$BuildDir\Debug\$Name"),
        (Join-Path $repoRoot "$BuildDir\Release\$Name")
    )

    foreach ($path in $candidates) {
        if (Test-Path $path) {
            return (Resolve-Path $path).Path
        }
    }

    throw "Executable not found: $Name (build dir: $BuildDir). Run cmake --build first."
}

function Invoke-Ctest {
    param(
        [string] $BuildDir = "build",
        [string] $Filter = "",
        [switch] $CtestVerbose
    )

    $repoRoot = Get-RepoRoot
    $buildPath = Join-Path $repoRoot $BuildDir

    $args = @("--test-dir", $buildPath, "--output-on-failure")
    if ($Filter) {
        $args += "-R", $Filter
    }
    if ($CtestVerbose) {
        $args += "-V"
    }

    Write-Host ">> ctest $($args -join ' ')" -ForegroundColor Cyan
    & ctest @args
    if ($LASTEXITCODE -ne 0) {
        throw "ctest failed (exit $LASTEXITCODE)"
    }
}

function Normalize-GoldenLineEndings {
    param([string] $Text)

    $normalized = $Text -replace "`r`n", "`n"
    $normalized = $normalized -replace "`r", "`n"
    return $normalized.TrimEnd("`n") + "`n"
}

function Mask-HttpTimestamps {
    param([string] $Text)

    return [regex]::Replace(
        $Text,
        '\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}',
        '{{TIMESTAMP}}'
    )
}

function Extract-HtmlStatsSection {
    param([string] $Html)

    $sections = [regex]::Matches(
        $Html,
        '(?s)<div class="section">\s*<h3>[^<]*분석 결과[^<]*</h3>(.*?)</div>\s*(?=<p class="alert|<div class="section"|</div>\s*</body>|$)'
    )

    if ($sections.Count -eq 0) {
        return ""
    }

    return ($sections | ForEach-Object { $_.Groups[1].Value }) -join "`n---`n"
}

function Extract-HtmlAlerts {
    param([string] $Html)

    $alerts = [regex]::Matches(
        $Html,
        '(?s)<p class="alert alert-(?:success|warning|danger)">(.*?)</p>'
    )

    if ($alerts.Count -eq 0) {
        return ""
    }

    $lines = foreach ($match in $alerts) {
        $inner = $match.Groups[1].Value -replace '<[^>]+>', ''
        $inner.Trim()
    }

    return ($lines -join "`n")
}
