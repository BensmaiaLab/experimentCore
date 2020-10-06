
function Write-Info ($s) { Write-Host -ForegroundColor Green $s }

function Get-VCPkgRoot {
    # Ask where.exe if vcpkg is on the path already. Otherwise, search known locations.
    $VCPkgRoot = ""
    $VCPkgRoot = Split-Path (where.exe vcpkg.exe)
    if ($VCPkgRoot -ne  "") {Return ($VCPkgRoot + '\')}
    $vcpkgPaths = @("C:\Repositories\vcpkg\", "C:\Repositories\vcpkg-master\", "D:\opt\vcpkg\")
    $vcpkgPaths | ForEach-Object {
        if (Test-Path $_) {
            Return $_
            Break
        }
    }
    if ($VCPkgRoot -eq  "") {
        Write-Error "Could not locate VCPkg."
        Exit 1
    }
}

function Test-VcpkgInstalled {
    param(
        [Parameter(Mandatory=$true)][String[]]$pkgName,
        [String[]]$VcPkg=(Get-VCPkgRoot) + "vcpkg.exe"
    )
    $results = Invoke-Expression "$VcPkg list $pkgName"
    if ($results.length -eq 0) {
        return $False
    }
    return $True
}
