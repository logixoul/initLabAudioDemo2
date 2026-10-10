$flags = '-O2', '-std=c++20', '-Ilib', '-Ilib/imgui'

# Rebuild the library object only when something under lib/ is newer than it.
$newestLib = Get-ChildItem lib -Recurse -File | Sort-Object LastWriteTime | Select-Object -Last 1
if (-not (Test-Path lib_impl.o) -or $newestLib.LastWriteTime -gt (Get-Item lib_impl.o).LastWriteTime) {
    Write-Host "Building lib_impl.o..."
    clang++ @flags -c lib/lib_impl.cpp -o lib_impl.o
    if ($LASTEXITCODE) { exit $LASTEXITCODE }
}

clang++ @flags main.cpp lib_impl.o -lopengl32 -lgdi32 -luser32 -o synth.exe
exit $LASTEXITCODE
