# New Model

Service model



* Control Plane - Responds to experimenter input, and acts as a state machine to transition the experiment from one state to the next during trials.
  * G/UI (RPC offers flexibility here)
  * World/Protocol State
    * Optionally, seperate from UI for higher precision
    * Experimental protocol definition in Psychopy:
      * Experiments &ni; Trials && Experiments &ni; Routines
      * Trials may vary routines
      * Routines are specified with "flows"

* Stimulus - each section here could be a seperate service, or one monolith, commanded by the control plane
  * Visual
    * [Psychopy](https://www.psychopy.org/)
  * Motion
    * Tactile stimulus
    * Calibration to match to protocol
  * Reward

* Sensors
  * Gaze Tracking - Probably needs to be tightly coupled to visual stimuli
    * OpenCV - Screen space transformation for gaze estimation
        * [Overview](https://www.learnopencv.com/gaze-tracking/)
  * logging - But this can be seperate
    * Proximate/Local daemon near sensors - Redis Cache or other in-memory cache, for fastest possible recording
    * Remote Long term database - Data gets dumped here for collation/sorting/querying

* Global components
  * gRPC
