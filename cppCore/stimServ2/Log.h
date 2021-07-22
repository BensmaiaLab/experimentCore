// log.h
// Boost logging which should be available everywhere
#pragma once
#include <iostream>
#include <string>
#pragma warning( push )
#pragma warning( disable : 4244 26495 26812 26498 26451)
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/support/date_time.hpp>
#pragma warning( pop )

void init_logging(bool debug, std::string filename);

#define logError BOOST_LOG_TRIVIAL(error)   << __FILE__ << ":" << std::to_string(__LINE__) << "|" << __func__ << "(): "
#define logWarn  BOOST_LOG_TRIVIAL(warning) << __FILE__ << ":" << std::to_string(__LINE__) << "|" << __func__ << "(): "
#define logDebug BOOST_LOG_TRIVIAL(debug)   << __FILE__ << ":" << std::to_string(__LINE__) << "|" << __func__ << "(): "
#define logTrace BOOST_LOG_TRIVIAL(trace)   << __FILE__ << ":" << std::to_string(__LINE__) << "|" << __func__ << "(): "
#define logInfo  BOOST_LOG_TRIVIAL(info) << __func__ << "(): "

class coloured_console_sink:
    public boost::log::sinks::basic_formatted_sink_backend<
        char, boost::log::sinks::synchronized_feeding>
{
public:
    static void consume(boost::log::record_view const& rec, string_type const& formatted_string);
};
