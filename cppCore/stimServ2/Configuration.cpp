#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <exception>
#include "Log.h"
#include "Configuration.h"

namespace pt = boost::property_tree;

/* 
 * Standardized settings format
 * Standard paths that all apps need to know about to inter-operate, etc
*/
void Configuration::load(const std::string &filename) {

    pt::ptree tree; // Create empty property tree object

    try {
        // Parse the XML into the property tree.
        pt::read_json(filename, tree);
        // tree.get() will throw if the value isn't available
        path.root        = tree.get<std::string>("path.root", "C:\\somlab");
        path.data        = tree.get<std::string>("path.data", "data");
        path.config      = tree.get<std::string>("path.config", "config");
        path.calibration = tree.get<std::string>("path.calibration", "calibration");

        eventSignals.device         = tree.get<std::string>("eventSignals.device", "Dev1");
        eventSignals.protocolBegin  = tree.get<std::string>("eventSignals.protocolBegin", "0");
        eventSignals.protocolEnd    = tree.get<std::string>("eventSignals.protocolEnd",   "1");

        // Stimservice connects to this address to command eyeTrack
        stimService.eyeTrackAddress = tree.get<std::string>("stimService.EyeTrackAddress", "localhost");
        stimService.eyeTrackPort    = tree.get<int>        ("stimService.EyeTrackPort",    5003);

        reward.device    = tree.get<std::string>("eyeTrackReward.device", "Dev1");
        reward.line      = tree.get<std::string>("eyeTrackReward.line",   "line0");

        useMatlab        = tree.get<bool>("useMatlab", false);

    } catch (boost::property_tree::ptree_error& e) {
        logError << "loading config file: " << e.what() << "\n";
    } catch (...) {
        logError << "Generic exception in Configuration::load()";
    }


    // m_file = tree.get<std::string>("debug.filename");

    // 0 here is a default value that will be provided if debug.level isn't found:
    // use this for optional values
    // m_level = tree.get("debug.level", 0);

    // Use get_child to find the node containing the modules, and iterate over
    // its children. If the path cannot be resolved, get_child throws.
    // A C++11 for-range loop would also work.
    /*BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("debug.modules")) {
        // The data function is used to access the data stored in a node.
        m_modules.insert(v.second.data());
    }
    */
}

/*
This is a placeholder and not used presently, included as example
*/
void Configuration::save(const std::string &filename)
{
    // Create an empty property tree object.
    pt::ptree tree;

    // Put the simple values into the tree. The integer is automatically
    // converted to a string. Note that the "debug" node is automatically
    // created if it doesn't exist.
    tree.put("path.root", path.root);
    tree.put("path.data", path.data);
    tree.put("path.config", path.config);
    tree.put("path.calibration", path.calibration);


    // Add all the modules. Unlike put, which overwrites existing nodes, add
    // adds a new node at the lowest level, so the "modules" node will have
    // multiple "module" children.
    /*BOOST_FOREACH(const std::string &name, m_modules)
        tree.add("debug.modules.module", name);*/

    // Write property tree to XML file
    pt::write_json(filename, tree);
}
