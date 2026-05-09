# Attempt to get a Windows-style drive letter, but stay quiet if it fails
$driveRoot = Split-Path -Path $PSScriptRoot -Qualifier -ErrorAction SilentlyContinue

# If $driveRoot is empty (because you're on your Pi 5), just use the full path
if (-not $driveRoot) { $driveRoot = $PSScriptRoot }

Write-Host "ok boot $driveRoot" -ForegroundColor Cyan
python server.py

