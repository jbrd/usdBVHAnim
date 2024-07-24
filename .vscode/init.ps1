# This script is called when initialising the Powershell terminal embedded in VsCode.
# It ensures that the terminal imports the Visual Studio x64 Developer Tools build environment.

# Install VSSetup if not already installed
if(-not (Get-Module VSSetup -ListAvailable)){
Install-Module VSSetup -Scope CurrentUser -Force
}

# Get latest version of visual studio
$latest = Get-VSSetupInstance -all | Select-VSSetupInstance -latest

# Import VCVars
$directory = Join-Path -path $latest.InstallationPath -childPath "VC\Auxiliary\Build"
pushd $directory
cmd /c "vcvars64.bat&set" |
foreach {
  if ($_ -match "=") {
    $v = $_.split("=", 2); set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
  }
}
popd