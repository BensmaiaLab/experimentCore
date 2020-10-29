# Lab Wiki

Danielle MacDonald Note: Found this in an old file store. Could be useful.
Seems related to Cuneate Rig (Rig 4).

## Protocol Specification

### Action Execution and Duration

Actions are executed in groups composed of one action per device in the stimulator.  If no action is specified for a particular device in a given action group, a hold action is executed for that device.  Action groups are synchronized, meaning that all actions within the action group must complete before moving to the next action group.  If asynchronous action execution is desired, actions need to be grouped carefully and durations managed to achieve the same result.

The duration of an action group is determined in the following fashion:

1.	If the eve_stim_dur code is set to a value greater than zero, the action group duration will be set accordingly.  Note the following details:
    * This is exclusive, and is not overridden by action execution duration.
    *	If a device is executing an action that has not yet completed at the specified duration, that action will continue until completion, or until other actions are executed that override the current action.  Meanwhile, the next action group will begin execution.  If no action is specified in the next action group for that device, the action will continue through this group.  And so on until the action completes or another action is executed that causes the device to perform a different action.  This can allow for the asynchronous  execution mentioned above.
    * Certain actions will cause some type of event when the specified duration is reached, such as outputting a location value.
1.	If the eve_stim_dur  code is set to a value less than zero (typically -1 by default), the duration of the action group will be driven by the actions being executed.  The action group will not complete until the longest action in the group is done.  Note the following details:
    * If no action is specified for a device within a given action group or if a hold actions is specified, the hold action will complete immediately.
    * Not every action has a specified completion mechanism.  For these actions, they will be considered complete immediately up execution or will enter an unknown state, even if that is not the desired behavior.  If this situation arises, and is believed to be incorrect or suboptimal, the underlying C++ control code will have to modified to change the behavior.
    *	Most movement actions for motors, stages, etc. should have a completion mechanism that allows them to execute until the movement is complete.
    *	Most measurement devices do not have completion mechanisms, since there is no "natural" end to a measurement.  These devices need a duration during which execution of the measurement action takes place. 
1.	Note the an eve_stim_dur code value of 0 is not recommended, since it  can be mistaken as a nonzero value if rounding occurs at some point in the code execution.

## cmo1301 Motion Stimulate Protocol

The cmo1301 series of experiments will examine how motion direction is represented in neural populations. The experimental setup consists of a micromo motor which rotates an aluminum wheel with a textured surface over the animals finger. The animals arm and hand will be mobilized while the bottom of the wheel is rotated across the animal's finger tip. The animal is then presented with a right/left choice on a screen. The correct response is the direction of rotation across the finger tip. The current trial sequence is outlined below:

1.	A fixation cross is displayed in the center of the screen
    1. The animal has 3 seconds (3000 ms)  to fixate before trial is aborted
    1. `eyetrackParams.targetDisplayDuration = 3000`
2.	A 75 millisecond fixation (2.1) is required to initiate stimulus presentation
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

## Spikesorter

### Setting Up Raw Data Recording

1. Locate the raw write configuration file called `rawWriteChannels.conf`.  It should be in the Spikesorter `Input` folder.  For example, if the Spikesorter is located in the `exe` folder in `somlab`, then the full path to this file would be:  `C:\somlab\exe\Spikesorter\input`.  Note: if this file doesn't exist, create it.
2. Edit this configuration file using a text editor.  If the default editor has not been chosen, windows may not know which program to open the file with, so a text editor will have to be manually chosen.
3. The format of the file is as follows:
    1. Each line in the file corresponds to the specification for a single channel
    1. Each line should contain two numbers separated by a space or a tab
    1. The first number should be the channel to raw write data from, e.g. channel 8
    1. The second number should be the sampling rate for that channel, e.g. 20000 for 20kHz
1.	All unnecessary lines should be removed from the file, because raw data can create large files quickly.

### Plugging Analog Data Lines Into the Spikesorter Breakout Box

1. If you need to connect additional analog data lines into the Spikesorter, you will need to locate the breakout box for the Spikesorter. If unknown, this breakout box can be found by tracing the National Instruments cable from the back of the Spikesorter machine. The NI cable has a very distinctive metal enclosure close to the attachment point to the NI card in the Spikesorter. This enclosure should say National Instruments on it.
2. Select a channel to use and plug the BNC analog cable into the input for that channel. Note that if you do not have a BNC termination on your cable, and interface will need to be used and/or built. Note: the Spikesorter uses differential inputs to reduce noise, so only half of the NI channels will be available. This usually results in 8 channels being available.
3. If the analog data comes from a floating source, (e.g. the Vibrometer), you will need to select Floating Source on that channel of the breakout box.  Floating Source is labelled as 'FS', as opposed to 'GS' for Grounded Source.  Note: if the breakout box does not have FS/GS switches, a different breakout will be needed, or resistors will need to be installed.

### Manual Spikesorter Recording
1.	The Spikesorter can be run manually, rather than via the Database or Experimenter Interface.  This can be useful for testing purposes and coupled with short term experiments.
2.	In order to execute manual recording, the Spikesorter will require an SBF file.  If a proper SBF is not needed, then any dummy SBF file can be used.  Otherwise, a proper SBF file with proper trial and stimulus specs for the experimental protocol should be used.  Create this SBF file before beginning manual recording.  The SBF file should be made available to the Spikesorter on the network or by copying the SBF file locally.
3.	To begin manual recording, select 'Start' from the 'Eve Files' menu in the Spikesorter's primary control panel (i.e. not the Oscilloscope or other graphical view).
4.	When prompted for an SBF file, locate the aforementioned SBF file and select it.
5.	When prompted for a place to store the EVE file data to be created, select your desired location.
6.	The Spikesorter will now begin recording.  Spikes and raw channel data will record to an EVE file as specified in the .conf files in the Spikesorter's 'input' folder.
7.	When the desired recording time has elapsed and/or the protocol has finished, the Spikesorter needs to be stopped.  Select 'Stop' form the 'Eve Files' menu in the Spikesorter's primary control panel.
Neural Signal Processor
Recording Spikes and LFP Data
    1. Power on the Blackrock Cerebus Neural Signal Processor (rackmounted black computer case with Neural Signal Processor inscribed on it) and the associated signal amplifier power.
    2. Power on the Blackrock Stim Switch (typically mounted close to the monkey's head).
    3. Power on the Cerebus Control Computer (rackmounted in the middle in Rig2 and Rig3).  This computer should show up on the far left monitor and be controlled by the associated mouse and keyboard.
    4. Fire up the Stim Switch interface software.  Note: if it doesn't connect, a new USB port may need to be specified.
    5. Fire up the Central software, which controls the Neural Signal Processor.
    6. Configure the Stim Switch Interface and Central as described below.
    7. Connect the animal as described below.
    8. In Central, open up the Channel/Hardware Configuration page.  Select all relevant channels (usually 1-96 for a single UEA or 1-128 for a UEA and two FMAs).  Right click and select properties (or may be called setup).  In the properties page, set the spike filter to X-Wide, and the LFP filter to X-Wide.  Set the LFP recording rate to 2kHz.  Select the remaining channels that are not in use, and disable. (Note: this section needs details and clarification).  This setup should remain if Central is closed out properly.  Double check before recording to ensure that setup is correct.
    9. In Central, open up the File Dialog and enter the full path to the data file location, which should be something like c:\somlab\data\raw\cor\csa1011.  If no folder exists, then create an appropriate folder.
10.	Add a file name onto the end of the folder specification.  The file name should have the format neuraldata_date.  For example, neuraldata_01_28_2014.  This will result in a complete path name that should look like this: c:\somlab\data\raw\cor\csa1011\ neuraldata_01_28_2014. This file dialog should remember its state, and start up with the same settings between sessions of Central. Double check before recording to ensure that setup is correct.
11.	Make sure that the automated startup is checked.
12.	Start a protocol as normal from the StimulatorControl computer.
13.	Ensure that the recording starts in the file dialog, or start it manually. If manual start is required, this must be done before starting protocols. If the auto start is not functioning, contact your programmer analyst.
14.	Between protocols the recording should pause.  Ensure that this happens, or pause manually.
15.	When the next protocol starts, the recording should resume automatically.  If a manual resume is required, this needs to be done before starting a new protocol.

## Configuring Stim Switch Interface for Typical Recording
1.	Description coming soon.  Configure as usual for recording.

## Configuring Central for Typical Recording
1.	Description coming soon. Configure as usual for recording. Use auto threshold.

## Connect the Animal for Typical Recording
1.	Description coming soon. Connect as usual for recording. Recording headstages must be connected, or use patient cable.
Eye Track
Parameters
2.	Primary Control
a.	Target Radius - specifies the radius of a circular area in mm surrounding a point where an eye gaze will be registered 
b.	Target Separation - the mm separation between the choice targets in the screen. Center to Center distance.
c.	Offset X - shifts the eye signal window in the positive/right (e.g. 12 mm) or negative /left (e.g. -12 mm) direction
d.	Offset Y - same as X Offset along the Y axis (up-down)
e.	Incorrect Target Alpha - dims the fixation cross-hair of the incorrect response where (alpha = 0) is totally dimmed.
f.	Multiple Target Alpha - if there are multiple stimuli per trial the incorrect fixation cross-hair is dimmed where (alpha = 0) is totally dimmed.
i.	NOTE: set both alpha values to 1 if you do not want the monkey to receive any visual cues.
g.	 Gaze Point Radius - radius of eye gaze point
h.	Acceptance Zone Radius - target acceptance zone radius
i.	Gaze Trace Point Count - number of past gaze points on experimenter display (gaze point persistence)
j.	Target Display - duration of fixation target presentation (while awaiting fixation)
k.	Fixation Duration - duration of required visual fixation (to initiate trial, not used with tactile)
l.	Response Delay - time before onset of synched visual response (after tactile presentation)
m.	Response Duration - duration of response target presentation (awaiting response)
n.	Reward Fix Dur - duration of required fixation on correct target to receive award
o.	Incorrect Fix Dur - duration of required fixation on incorrect target to end trial or receive punishment if set
p.	Reward Delay - amount time before monkey receives reward after successful fixation on choice target
q.	Intertrial Duration - visual intertrial duration (before monkey can initialize next trial)
r.	Punishment Duration - punishment timeout duration
s.	Auditory Reward - auditory reward on/off state (0=off, 1=on)
t.	Record Eye Position Data - record eye position on/off state (0=off, 1=on)
u.	Advance on correct only - advance to the next trial on correct response only (0=off, 1=on)

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
    10.	If you have 4GB RAM or less set min and max to 512 MB
    11. OR you can move the page file onto a second HDD (if one's available)
        1.	disable the page file as detailed above
        2.	click on the HDD and select 'System managed size' and click 'Set'
4.	Set power options to "High Performance power plan" (for desktops)
    1. open start menu and type "Power Options" into the search bar and press enter
    2. select the High Performance power plan
    3. click "change plan settings"
    4. click "change advanced power settings"
    5. expand the hard disk option and change setting to "never"
    6. click OK
5.	Check Defragmentation Schedule
    1. windows will turn off defrag schedule if it automatically detects a SSD but you still must check. NEVER DEFRAG A SSD
    2. Open Disk Defragmenter (type into start menu search bar)
    3. Under "Current Status:" the SSD drive should display "Never Run" under the Last Run tab
    4. For Windows 8 users:
        1. enter 'Defragment' in the search box and click 'Defragment and optimize your drives'
            1. you must have the settings box highlighted
        2. click the SSD drive and check that defragment and optimize is turned off
6.	Disable GUI on boot (optional)
    1. open the start menu and type "msconfig" into the search bar and press enter
    2. click the "boot" tab in the system configuration window
    3. check the checkbox for "No GUI boot"
    4. This disables the windows startup screen where the four colored orbs fly together. Decreases boot time by a few seconds.
7.	Enable write back caching and turn off windows write-caching buffer flush
    1. open start menu and type "Disk Management" into the search bar and press enter
    2. Right click C: drive (or whatever your SSD is assigned to)
    3. click properties
    4. go to the "hardware" tab
    5. select the drive
    6. click properties
    7. go to the "Policies" tab
    8. The "Enable write caching" box should be checked by default
    9. tick the checkmark for "turnoff windows write-caching buffer flush"
    10.	click the OK button
8. Disable Prefetch and Superfetch
    1. open the start menu and type "Services.msc" into the search bar and press enter
    2. scroll down until you see the "Superfetch" entry
    3. double-click on it and choose disabled from the list
    4. Exit this
    5. open the start menu and type "regedit" and press enter
    6. Go to: HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Memory Management\PrefetchParameters
    7. double-click on "EnablePrefetcher" and enter 0
    8. double-click on "EnableSuperfetch" and enter 0
9.	Disable Windows Search
    1. open start menu, type "services" into the search bar and press enter
    2. scroll down to "Windows Search" and double click
    3. Set "disabled" from the drop down menu and press the "Stop" button
    4. press apply and enter
10.	Check to make sure TRIM is enabled
    1.	open the command window as an administrator
        1.	type 'cmd' into the search bar, right click and select 'Run as Administrator'
    2.	type the command: fsutil behavior query disabledeletenotify
    3.	If the output reads:  DisableDeleteNotify = 0   then TRIM is enabled

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
