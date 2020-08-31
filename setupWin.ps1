# Script to deploy necessary tools to a windows machine.
# Must be run from admin powershell.

Set-ExecutionPolicy Bypass -Scope Process -Force

# Install Chocolatey:
# https://chocolatey.org/install
Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

# install the things we need to do the rest:
`choco install -y --force --params "ALLUSERS=1" openssh git.install vlc 7zip.install googlechrome vcredist140 dotnetfx sysinternals ccleaner python vscode saltminion sysinternals virtualbox vagrant vagrant-manager firacode mingw nomachine hugo-extended nodejs`
`choco install -y --force --params "ALLUSERS=1 ADD_CMAKE_TO_PATH=System" cmake`
```powershell
choco install -y --no-progress --force visualstudio2019community --package-parameters "\
--norestart --nocache --includeRecommended --includeOptional \
--add Microsoft.VisualStudio.Workload.NativeDesktop \
--add Microsoft.VisualStudio.Workload.NativeCrossPlat \
--add Microsoft.VisualStudio.Workload.ManagedDesktop \
--add Microsoft.VisualStudio.Workload.Python"
```

# setup vcpkg:
# https://github.com/microsoft/vcpkg
$optDir = "C:\opt\"

mkdir $optDir
set-location $optDir

git clone git@github.com:microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg integrate install

# Set triplet:

$env:VCPKG_DEFAULT_TRIPLET = "x64-windows"
.\vcpkg\vcpkg install boost:x64-windows

