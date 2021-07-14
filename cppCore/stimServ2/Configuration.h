#pragma once

#include <string>
#include <set>

// Holds global configuration settings
struct Configuration {
    // Paths used by EyeTrack and StimService
    struct Paths {
        std::string root;
        std::string data;
        std::string config;
        std::string calibration;
    };


    // NIDAQ water reward for subject
    struct Reward {
        std::string device;
        std::string line;
    };

    /*
    NIDAQ digital logic signals (on or off) on protocol events usually to
    Blackrock to include with neural recording data (effectively as
    timestamps of when protocol events occurred).
    null = don't use
    */
    struct EventSignals {
        std::string device;
        std::string protocolBegin;
        std::string protocolEnd;
        std::string trialBegin;
        std::string trialEnd;
        std::string stimBegin;
        std::string stimEnd;
        std::string postTrialBegin;
        std::string postTrialEnd;
    };

    struct StimService {
        std::string eyeTrackAddress;
        int         eyeTrackPort;
    };

    Paths         path;
    Reward        reward;
    EventSignals  eventSignals;
    StimService   stimService;
    bool          useMatlab;

    void load(const std::string &filename);
    void save(const std::string &filename);
};

// Make it global. It's defined at the top of StimService and EyeTrack
extern Configuration config;
