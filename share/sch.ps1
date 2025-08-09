$path = Join-Path -Path $PWD -ChildPath "csdhr.exe"
$Action = New-ScheduledTaskAction -Execute "$path"
$Trigger1 = New-ScheduledTaskTrigger -AtLogOn

$Trigger2 = New-ScheduledTaskTrigger -Once -At 11:20AM -RepetitionInterval (New-TimeSpan -Hours 1)
#$Trigger2 = New-ScheduledTaskTrigger -Daily -At 11:20AM
#$Trigger2.RepetitionInterval = New-TimeSpan -Hours 1
#$Trigger2.RepetitionDuration = New-TimeSpan -Days 365 

$Settings = New-ScheduledTaskSettingsSet `
    -DontStopIfGoingOnBatteries -Hidden `
    -RestartInterval (New-TimeSpan -Minutes 5) `
    -RestartCount 2 `
    -MultipleInstances IgnoreNew

$Principal = New-ScheduledTaskPrincipal -UserId "Admin" -LogonType ServiceAccount -RunLevel Highest
Register-ScheduledTask -TaskName "USER-CSD-3E0E7701-D437-4D51-A395-EA01F14E1A0D" -Trigger $Trigger1, $Trigger2 -Action $Action -Principal $Principal -Settings $Settings -Force
Start-ScheduledTask -TaskName "USER-CSD-3E0E7701-D437-4D51-A395-EA01F14E1A0D"
