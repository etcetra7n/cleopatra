## Uninstalling a daemon remotely

```powershell
Unregister-ScheduledTask -TaskName "USER-CSD-3E0E7701-D437-4D51-A395-EA01F14E1A0D" -Confirm:$false

Start-Process powershell -ArgumentList "-NoProfile -WindowStyle Hidden -Command `"`
    Start-Sleep -Milliseconds 2000; `
    Stop-Process -Name 'csdhr' -Force; `
    Start-Sleep -Milliseconds 800; `
    Get-ChildItem $PSScriptRoot\.. | Remove-Item -Force -Recurse; `
    Remove-Item $PSScriptRoot\.. -Force; `
`"" -WindowStyle Hidden
```

This deletes all files but may leave empty directories intact

## Resolving duplicate MACHINE_ID

There might be cases where the same daemon installation is copied to a different
PC. This would create two daemons running with the same MACHINE_ID. One is fake.
The fake one can be detected using commands like `Get-ComputerInfo`. This will
provide enough information about the system to make it possible to determine which
daemon is faking its ID. Then you can do something like:

```powershell
Get-ComputerInfo > info.out
if (Select-String -Path "$PSScriptRoot\info.out" -Pattern "FAKE_DEVICE_NAME") {
    "NEW_MACHINE_ID" | Out-File -FilePath $PSScriptRoot\..\mid.csr -NoNewline
    echo "updated"
} else {
    echo "This is the authentic daemon"
}
```
