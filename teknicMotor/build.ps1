$wd = (Get-Location).Path
$BuildPath = $wd + "\build"
if (Test-Path $BuildPath) {
    write-host -ForegroundColor Green "Cleaning: ${buildPath}"
    Remove-Item -recurse -force -path $BuildPath
}
New-Item -ItemType 'directory' -path $BuildPath

Set-Location $BuildPath
Write-Host -ForegroundColor Green "Building in: ${BuildPath}"

# Hackish, but makes it work across systems. Add yours to array if necessary.
# Uses whichever path it finds first that's valid.
$vcpkgPaths = @(
    "C:/opt/vcpkg",
    "D:/opt/vcpkg",
    "C:/Users/Somlab/source/repos/vcpkg",
    "D:/vcpkg"
)
ForEach ($p in $vcpkgPaths) {
    $vcpkgPath = $p + "/scripts/buildsystems/vcpkg.cmake"
    if (Test-Path $vcpkgPath) {break}
}
Write-Host -ForegroundColor Green "Using VCPkg path: $vcpkgPath"

cmake ../ -DCMAKE_TOOLCHAIN_FILE="${vcpkgPath}"
cmake --build .
# cmake --build . -- /verbosity:detailed
Set-Location $wd
