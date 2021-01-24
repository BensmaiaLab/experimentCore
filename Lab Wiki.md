# Lab Wiki

Danielle MacDonald Note: Found this in an old file store. Could be useful.
Seems related to Cuneate Rig (Rig 4).

## cmo1301 Motion Stimulate Protocol

The cmo1301 series of experiments will examine how motion direction is represented in neural populations. The experimental setup consists of a micromo motor which rotates an aluminum wheel with a textured surface over the animals finger. The animals arm and hand will be mobilized while the bottom of the wheel is rotated across the animal's finger tip. The animal is then presented with a right/left choice on a screen. The correct response is the direction of rotation across the finger tip. The current trial sequence is outlined below:

1. A fixation cross is displayed in the center of the screen
    1. The animal has 3 seconds (3000 ms)  to fixate before trial is aborted
    1. `eyetrackParams.targetDisplayDuration = 3000`
2. A 75 millisecond fixation (2.1) is required to initiate stimulus presentation
    1. `eyetrackParams.fixationDuration = 75`
3. stimulus initiates -> wheel rotates over animals finger at specified rotational velocity
4. the stimulus wheel rotates for a specified interval (4.1) -> right/left choice is presented on screen
    1. dur   = 0.400   (this parameter is in secs) --> time before choice is presented
5. the stimulus wheel continues to rotate until the animal makes a choice
    1. the animal has 3 seconds (5.2) to make a choice (saccade left or right)
    2. `eyetrackParams.responseDuration  = 3000`
6. a 50 millisecond fixation (6.1) is required to complete trial
    1. `eyetrackParams.rewardFixationDuration = 50`
7. a correct response is rewarded
    1. `eyetrackParams.wateRewardDuration = 700` (time solenoid is open in milliseconds)

## SSD Optimization

### Optimizing SSD Performance

1. Turn off the hibernation file
    1. open command window as administrator
    2. type  powercfg -h off and press enter
2. Turn off drive indexing
    1. open start menu and click computer
    2. Right click the C: drive and select properties
    3. uncheck drive indexing at the bottom --> "allow files on this drive to have contents indexed..."
    4. allow all checks and select "drive and all contents"
3. Turn off or shrink page file
    1. open start menu and right click computer
    2. select properties
    3. click advanced system settings
    4. under Performance click settings
    5. go to the Advanced tab
    6. under virtual memory click Change
    7. Uncheck the "automatically manage paging file box" at the top
    8. select the C: drive
    9. click "no paging file" (if you have > than 4 GB of RAM) and click 'Set'
    10. If you have 4GB RAM or less set min and max to 512 MB
    11. OR you can move the page file onto a second HDD (if one's available)
        1. disable the page file as detailed above
        2. click on the HDD and select 'System managed size' and click 'Set'
4. Set power options to "High Performance power plan" (for desktops)
    1. open start menu and type "Power Options" into the search bar and press enter
    2. select the High Performance power plan
    3. click "change plan settings"
    4. click "change advanced power settings"
    5. expand the hard disk option and change setting to "never"
    6. click OK
5. Check Defragmentation Schedule
    1. windows will turn off defrag schedule if it automatically detects a SSD but you still must check. NEVER DEFRAG A SSD
    2. Open Disk Defragmenter (type into start menu search bar)
    3. Under "Current Status:" the SSD drive should display "Never Run" under the Last Run tab
    4. For Windows 8 users:
        1. enter 'Defragment' in the search box and click 'Defragment and optimize your drives'
            1. you must have the settings box highlighted
        2. click the SSD drive and check that defragment and optimize is turned off
6. Disable GUI on boot (optional)
    1. open the start menu and type "msconfig" into the search bar and press enter
    2. click the "boot" tab in the system configuration window
    3. check the checkbox for "No GUI boot"
    4. This disables the windows startup screen where the four colored orbs fly together. Decreases boot time by a few seconds.
7. Enable write back caching and turn off windows write-caching buffer flush
    1. open start menu and type "Disk Management" into the search bar and press enter
    2. Right click C: drive (or whatever your SSD is assigned to)
    3. click properties
    4. go to the "hardware" tab
    5. select the drive
    6. click properties
    7. go to the "Policies" tab
    8. The "Enable write caching" box should be checked by default
    9. tick the checkmark for "turnoff windows write-caching buffer flush"
    10. click the OK button
8. Disable Prefetch and Superfetch
    1. open the start menu and type "Services.msc" into the search bar and press enter
    2. scroll down until you see the "Superfetch" entry
    3. double-click on it and choose disabled from the list
    4. Exit this
    5. open the start menu and type "regedit" and press enter
    6. Go to: HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Memory Management\PrefetchParameters
    7. double-click on "EnablePrefetcher" and enter 0
    8. double-click on "EnableSuperfetch" and enter 0
9. Disable Windows Search
    1. open start menu, type "services" into the search bar and press enter
    2. scroll down to "Windows Search" and double click
    3. Set "disabled" from the drop down menu and press the "Stop" button
    4. press apply and enter
10. Check to make sure TRIM is enabled
    1. open the command window as an administrator
        1. type 'cmd' into the search bar, right click and select 'Run as Administrator'
    2. type the command: fsutil behavior query disabledeletenotify
    3. If the output reads:  DisableDeleteNotify = 0   then TRIM is enabled

## Software prereqs for a fully functioning rig computer

* NIDAQmx
  * get from national instruments website
* Java JDK with Netbeans IDE
  * get from Oracle website
* DirectX SDK (WINDOWS 7 only)
  * Get from Microsoft website
  * Not required when compiling on Windows 10 (VS2015). DirectX SDK is part of the Windows SDK from 10 on up.
  * Make sure WINDOWS7 is set as a preprocessor directive when compiling on Windows 7.
* Intel OpenCL SDK
  * Get from intel website
  * This is required to run OpenCV used in the pupil tracker
  * Sometimes this is not required. You can pull tbb.dll from the OpenCV 2.8.1 distribution package and sometimes other dll’s as required.
* Vernier GoIO SDK
  * Get from Vernier website
  * This is required if you want to compile the Eyetrack

## Visual Studio 2008 to 2015 (Windows 10) portability issues

There was one bug that came up which I still don’t understand. In libEVE – CodeMapVector::extractString() the while loop at line 92 runs forever once it finishes reading in all characters from eveCodesC++Map.txt. This did not happen in VS2008 (in either Win7 or Win10) but causes all executable programs that depend on libEVE (StimControl, DataToolbox, Spikesorter) to hang at the beginning while it tries to parse the eve code map past the input stream when compiled in VS2015. The quick and dirty solution: add another tokenizer string (*****) onto the last line of the eve codes text file eveCodesC++Map.txt.

## Pupil Tracker hardware

Here is a list of the camera and lens configuration that’s been tested and verified.

* Camera:
  * Point Grey Chameleon USB 3.0 Camera 1/2" CMOS CS-Mount - $350
  * Point Grey part number: CM3-U3-13Y3M-CS
* Cable:
  * 5 METER USB3.0 CABLE, TYPE-A to MICRO-B (LOCKING) - $15
  * Point Grey part number: ACC-01-2301
* Lens:
  * Computar CS-mount 10-30mm varifocal lens - $134
  * B&H photo part number: COH3Z1014CS
* Camera Mount:
  * Kupo Vision articulating arm (10”) - $75
  * B&H photo part number: KUKG100711
