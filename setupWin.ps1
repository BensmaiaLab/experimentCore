# Script to deploy necessary tools to a windows machine.
# Must be run from admin powershell.

Set-ExecutionPolicy Bypass -Scope Process -Force

# Install Chocolatey:
# https://chocolatey.org/install
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

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
if (-NOT($Env:NIDAQ_HOME)) {
    [Environment]::SetEnvironmentVariable("NIDAQ_HOME", "C:\Program Files (x86)\National Instruments", "Machine")
}

# Manually update system path
# This adds cmake
$pathsToAdd = @('C:\Program Files\CMake\bin', 'D:\vcpkg\installed\x64-windows\tools\protobuf')

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

Write-Host -ForegroundColor Green @"


================================================================================
! System should be fully kicked! GLHF
================================================================================

"@