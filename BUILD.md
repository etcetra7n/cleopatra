# Installation

## Manually building executable

```
cd build
./build_RELEASE.bat
```

`csdhr.exe` will be placed at `C:\Users\Admin\AppData\Local\csdhr\csdhr.exe` during
installation

## Manually building installer

```
cd installer/build
./BUILD.bat
```

## Automatic On-demand build on GitHub Actions

There is a GitHub Build workflow in this repo, which builds both executable and the
and installer and can be manually triggered through `workflow_dispatch`.

Go to Actions tab in this GitHub repository. Select `On-demand build on Windows`
workflow and click `Run workflow`. Come back after about 10 minutes to greet the
newly built executables

## Setting up auto startup

> [!NOTE]
> The installer takes care of this step automatically. This section is only here
>  for the record. This step is only required to be done manually when not installing
> through the installer

Follow the below steps to make sure the program starts up on windows startup

- Method #1 (Recommended): Press Win+R, type `taskschd.msc`. In the Actions 
pane, click on Create Task. Check the box for Run with highest privileges.
Set Trigger to Run at Startup. Set one trigger for launching on startup, and
set another trigger for launching at 11:20 AM (A reasonable time at which 
most people have their PC on). Go to Action to set your program location. 
Click 'Ok' to save. Click on `Start` to initiate the scheduler cycle. 
(Instead of all this steps you may also just go to taskschd.msc and click 
on 'Import Task' and select `sch.xml`. The detail of steps
to do this manually is just here for the record)

- Method #2: Go to registry key at 
`HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run` and edit
its values accordingly (Note: This method wont start up the program with 
administrator privileges)

To ease installation, installers have also been made which automatically takes
care of placing the software in the right location and creating the DAEMON_ID.
But it would not schedule it to automatically run on loggon. This must be done
manually as mention in method I (recommended) or method II
