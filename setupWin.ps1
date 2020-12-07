# Script to deploy necessary tools to a windows machine.
# Must be run from admin powershell.

Set-ExecutionPolicy Bypass -Scope Process -Force

# Install Chocolatey:
# https://chocolatey.org/install
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

# install the things we need to do the rest:
choco install -y --force --params "ALLUSERS=1" openssh git.install vlc 7zip.install googlechrome vcredist140 dotnetfx sysinternals ccleaner python vscode saltminion sysinternals virtualbox vagrant vagrant-manager firacode mingw nomachine hugo-extended nodejs
choco install -y --force --params "ALLUSERS=1 ADD_CMAKE_TO_PATH=System" cmake
powershell
choco install -y --no-progress --force visualstudio2019community --package-parameters "\
--norestart --nocache --includeRecommended --includeOptional \
--add Microsoft.VisualStudio.Workload.NativeDesktop \
--add Microsoft.VisualStudio.Workload.NativeCrossPlat \
--add Microsoft.VisualStudio.Workload.ManagedDesktop \
--add Microsoft.VisualStudio.Workload.Python"


$optDir = "C:\opt\"
# Reset path after all the updates:
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User") 

if (!(Test-Path $optDir)) { New-Item -ItemType 'directory' -path $optDir }
set-location $optDir

# setup vcpkg:
# https://github.com/microsoft/vcpkg
if (!(Test-Path ${OptDir}\vcpkg)) {git clone git@github.com:microsoft/vcpkg.git }
Invoke-Expression "${optDir}vcpkg\bootstrap-vcpkg.bat"
Invoke-Expression "${optDir}vcpkg\vcpkg integrate install"

# Set triplet:
$env:VCPKG_DEFAULT_TRIPLET = "x64-windows"

# And download ALL the code packages
.\vcpkg\vcpkg install boost:x64-windows grpc:x64-windows


#TODO: Should I make sure these apps are installed first? Ugh.
# Stereognosis project needs these env vars
if (-NOT($Env:Teknic)) {
    [Environment]::SetEnvironmentVariable("Teknic", "C:\Program Files (x86)\Teknic\ClearView\sdk\", "Machine")
}

# Don't use: C:\Program Files (x86)\National Instruments\NI-DAQ\DAQmx ANSI C Dev\
# Current location also has 64 bit libs.
# or NIDAQ_HOME - has include, lib32, and lib64
if (-NOT($Env:NIDAQ)) {
    [Environment]::SetEnvironmentVariable("NIDAQ", "C:\Program Files (x86)\National Instruments\Shared\ExternalCompilerSupport\C\", "Machine")
}
if (-NOT($Env:Matlab32)) { #TODO: Could probably add some autodetect code to find the exact year/version.
    [Environment]::SetEnvironmentVariable("Matlab32", "C:\Program Files (x86)\MATLAB\R2020b\extern\", "Machine")
}
if (-NOT($Env:Matlab64)) { #TODO: Could probably add some autodetect code to find the exact year/version.
    [Environment]::SetEnvironmentVariable("Matlab64", "C:\Program Files\MATLAB\R2020b\extern\", "Machine")
}
if (-NOT($Env:Matlab32RT)) {
    [Environment]::SetEnvironmentVariable("Matlab32RT", "C:\Program Files (x86)\MATLAB\MATLAB Compiler Runtime\v717\extern\", "Machine")
}
if (-NOT($Env:Matlab64RT)) {
    [Environment]::SetEnvironmentVariable("Matlab64RT", "C:\Program Files\MATLAB\MATLAB Runtime\v99\extern\", "Machine")
}

# Manually update system path
# This adds cmake
$pathsToAdd = @('C:\Program Files\CMake\bin', 'C:\opt\vcpkg\installed\x64-windows\tools\protobuf')

# Does the whole system?
# $regSysPath = 'Registry::HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Session Manager\Environment'
# $oldSysPath = (Get-ItemProperty -Path $regSysPath -Name PATH).path
# Set-ItemProperty -Path $regSysPath -Name PATH -Value ($oldSysPath + ';' + ($pathsToAdd -join ";"))

$regUsrPath = 'Registry::HKEY_CURRENT_USER\Environment'
$oldUsrPath = (Get-ItemProperty -Path $regUsrPath -Name PATH).path
Set-ItemProperty -Path $regUsrPath -Name PATH -Value ($oldUsrPath + ';' + ($pathsToAdd -join ";"))

# Manually adds to path
$env:Path += ';' + ($pathsToAdd -join ";")

#TODO: Check that this isn't already on the path
# Automatically add the cmake toolchain file to my VS Code install
$p = ($ENV:APPDATA + '\Code\User\settings.json')
$s = Get-Content $p | ConvertFrom-Json -AsHashtable
if (!($s.ContainsKey('cmake.configureSettings'))) {
    $cmOptDir = $optDir.Replace('\', '/') + 'vcpkg'
    $s.'cmake.configureSettings' = @{CMAKE_TOOLCHAIN_FILE = "$cmOptDir/scripts/buildsystems/vcpkg.cmake"}
    $s | ConvertTo-Json | Out-File $p
}



# Install my rec'd VSCode extensions
$vsCodeExtToInstall = @("aaron-bond.better-comments",
"budparr.language-hugo-vscode",
"CoenraadS.bracket-pair-colorizer",
"cymonk.sql-formatter",
"DavidAnson.vscode-markdownlint",
"dbaeumer.vscode-eslint",
"eamodio.gitlens",
"esbenp.prettier-vscode",
"lihui.vs-color-picker",
"marcostazi.VS-code-vagrantfile",
"ms-python.python",
"ms-vscode.cmake-tools",
"ms-vscode.cpptools",
"ms-vscode.powershell",
"ms-vsliveshare.vsliveshare",
"msjsdiag.debugger-for-chrome",
"naumovs.color-highlight",
"oderwat.indent-rainbow",
"Tobiah.language-pde",
"twxs.cmake",
"VisualStudioExptTeam.vscodeintellicode",
"vsciot-vscode.vscode-arduino",
"zxh404.vscode-proto3")

forEach ($ext in $vsCodeExtToInstall) {
    code --install-extension $ext
}

Set-PSRepository PSGallery -InstallationPolicy Trusted
Install-Module VSSetup


# Setup PC for remote access
Enable-PSRemoting -force
# This enables access from Citrine (DM's workstation) and the build server
$TrustedIPs = @('192.170.210.124', '128.135.218.161')
ForEach ($ip in $TrustedIPs) {
    Set-Item -Path WSMan:\localhost\Client\TrustedHosts -Value $ip -Concatenate -Force
}
# To see this list:
# (Get-Item -Path WSMan:\localhost\Client\TrustedHosts).Value -Split ','

# Map standard network drives
if (-NOT (Test-Path "L:")) {
    New-PSDrive -Name "L" -PSProvider "FileSystem" -Root "\\bensmaia-lab\LabSharing"
}
if (-NOT (Test-Path "K:")) {
    New-PSDrive -Name "K" -PSProvider "FileSystem" -Root "\\192.170.210.124\artifacts"
}

Write-Host -ForegroundColor Green @"


================================================================================
! System should be fully kicked! GLHF
================================================================================

"@
