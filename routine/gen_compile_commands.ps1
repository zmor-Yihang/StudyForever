# Regenerate compile_commands.json (the file list clangd uses for cross-file indexing).
# Usage: run  ./gen_compile_commands.ps1  from the project root.
# Re-run after adding/removing .c files. Compile flags live in .clangd, not here.

$root = $PSScriptRoot
if (-not $root) { $root = (Get-Location).Path }
$rootUnix = $root.Replace('\','/')

$entries = Get-ChildItem -Path $root -Recurse -Filter *.c -File |
    Sort-Object FullName |
    ForEach-Object {
        $rel = $_.FullName.Substring($root.Length + 1).Replace('\','/')
        [PSCustomObject]@{
            directory = $rootUnix
            file      = $rel
            command   = "clang $rel"
        }
    }

$json = @($entries) | ConvertTo-Json -Depth 3
[System.IO.File]::WriteAllText(
    (Join-Path $root 'compile_commands.json'),
    $json,
    (New-Object System.Text.UTF8Encoding($false))
)

Write-Host "Rebuilt compile_commands.json with $($entries.Count) source files."