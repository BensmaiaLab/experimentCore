

function Copy-ToTemp {
    param(
        [parameter(Mandatory=$true, Position=0)]
        [ValidateNotNullOrEmpty()]
        [string]$Path
    )
    $item = Get-Item -Path $Path
    # Set the extension to match so no problems when executing/unzipping
    $TempFile = New-TemporaryFile | Rename-Item -NewName {$_.Name + $item.Extension} -PassThru
    Copy-Item -Path $item -Destination $TempFile
    Return $TempFile
}

function Mount-GetDrive {
    param(
        [parameter(Mandatory=$true, Position=0)]
        [ValidateNotNullOrEmpty()]
        [string]$Path
    )
    $mountPath = Mount-DiskImage -ImagePath "$Path" -StorageType "ISO"
    Return (Get-DiskImage -DevicePath $mountPath.DevicePath | Get-Volume).DriveLetter
}


# Copy locally, mount iso, and install the internal file.
function Install-IsoArtifact {
    param(
        $Path,
        $installFile
    )
    New-PSDrive -Name "Artifacts" -PSProvider "FileSystem" -Root "\\CITRINE\artifacts"
    $localFile = Copy-ToTemp -Path "Artifacts:\$Path"
    $driveLetter = Mount-GetDrive $localFile

    Invoke-Expression "${driveLetter}:\${installFile}"

    Dismount-DiskImage $localFile
    Remove-PSDrive -Name "Artifacts"
}

Install-IsoArtifact -Path "ni-daqmx_20.1.0_offline.iso"

