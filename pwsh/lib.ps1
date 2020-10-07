
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

function Get-CredentialFromString {
    [Diagnostics.CodeAnalysis.SuppressMessageAttribute("PSAvoidUsingPlainTextForPassword", '', Scope="Function", Target="*")]
    # Take string versions of user/pass and construct a credential object.
    # Bypasses a lot of the shenanigans MS put in the way to make this hard to
    # do so you don't mess up the security model.
    Param (
        [parameter(Mandatory=$true, Position=0)]
        [ValidateNotNullOrEmpty()]
        [string]$username,
        [parameter(Mandatory=$true, Position=1)]
        [ValidateNotNullOrEmpty()]
        [string]$password
    )
    $hashedPass = ConvertTo-SecureString -AsPlainText $password -Force
    New-Object System.Management.Automation.PSCredential -ArgumentList $username, $hashedPass
}
