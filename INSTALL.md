# Installation

Build using:

```
cmake . -G "Unix Makefiles"
make
```

Place the `daemon.exe` at `C:\Users\Admin\AppData\Local\Cleopatra\daemon.exe`

Follow the below steps to make sure the program starts up on windows startup

- Method #1 (Recommended): Press Win+R, type `taskschd.msc`. In the Actions 
pane, click on Create Task. Check the box for Run with highest privileges.
Set Trigger to Run at Startup. Set one trigger for launching on startup, and
set another trigger for launching at 11:20 AM (A reasonable time at which 
most people have their PC on). Go to Action to set your program location. 
Click 'Ok' to save. Click on `Start` to initiate the scheduler cycle. 
(Instead of all this steps you may also just go to taskschd.msc and click 
on 'Import Task' and select `CleopatraSD_Scheduler.xml`. The detail of steps
to do this manually is just here for the record)

- Method #2: Go to registry key at 
`HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run` and edit
its values accordingly (Note: This method wont start up the program with 
administrator privileges)
