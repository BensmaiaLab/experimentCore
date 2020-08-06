# Build Environment setup for my tools

This is some notes on the things I've been trying to setup for my build process.

## Things we want/need installed

* git
* cmake
* python3.8
* Windows Terminal
* Windows Subsystem for Linux
* Ubuntu
* VS 2008 (Last compiled for)
* VS 2019 (Community)
* VS Code

## Setup for Protoc

* Start Menu > x64 Native Tools CLI for VS 2019
* Make sure git and cmake are installed and on path
* `cd E:\repos\grpc\third_party\protobuf\cmake`
* `mkdir build & cd build; mkdir release * cd release`
* `make -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../../../../install ../..`
* `nmake`
* add "E:\repos\grpc\third_party\protobuf\cmake\build\release" to path
* now can run `protoc` on windows

## Chocolatey

Can I use [chocolatey](https://chocolatey.org/install) to install stuff on windows?

```pwsh
Set-ExecutionPolicy AllSigned

Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
```

### Dev Systems

`choco install -y --force --params "ALLUSERS=1" openssh git.install vlc 7zip.install googlechrome vcredist140 dotnetfx sysinternals ccleaner python vscode salt-minion sysinternals virtualbox vagrant vagrant-manager firacode mingw cmake nomachine hugo-extended nodejs`

### Build system

`choco install -y --force --params "ALLUSERS=1" openssh git.install visualstudio2019buildtools vcredist140 salt-minion mingw cmake nomachine hugo-extended nodejs`

### Endpoints

Things I need to do to configure experimental systems:

* Open Powershell to remote connections:
  * `Enable-PSRemoting`

### Lab PCs

`choco install -y --force --params "ALLUSERS=1" openssh git.install salt-minion nomachine vlc 7zip.install googlechrome ccleaner python vscode`


## Git Repo and Docker containers on Github

* https://help.github.com/en/packages/publishing-and-managing-packages/about-github-packages
* Setup my salt master! On Docker?s
  * https://docs.saltstack.com/en/master/topics/installation/windows.html



## Vagrant for TestBoxen

```pwsh
vagrant init detectionlab/win10
vagrant up
vagrant powershell # To Remote in
```

## You want SysInternals tools.

Especially 'Process Monitor' (`procmon`) and 'Process Explorer' (`procex`).
Procmon logs all filesystem access (including registry keys, and other
pseudo-filesystems), and procex acts as a drop-in Task Manager replacement on
steroids. Both are essential for debugging the behavior of windows systems.

You will want [symbol loading](https://www.xitalogy.com/windows-internals/2019/08/14/windows-internals-how-to-configure-symbols-in-sysinternals-process-explorer.html)
setup for debugging purposes.

