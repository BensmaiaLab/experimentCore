// log.cpp
#include "pch.h"
#include "log.h"

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;


// Windows centric!
unsigned short get_colour(boost::log::trivial::severity_level level) {
    switch (level)
    {
        case logging::trivial::trace: return 0x08;   // FG Gray
        case logging::trivial::debug: return 0x07;   // FG Off-white
        case logging::trivial::info: return 0x0F;    // FG White
        case logging::trivial::warning: return 0x0E; // FG Yellow
        case logging::trivial::error: return 0x0D;   // FG Magenta
        case logging::trivial::fatal: return 0x0C;   // FG Red
        default: return 0x0F;
    }
}

// TODO: Wrap this whole thing in platform specific conditionals
// Custom log line formatter
void my_formatter(logging::record_view const& rec, logging::formatting_ostream& strm)
{

    auto fmtTimeStamp = expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
    auto fmtThreadId  = expr::attr<logging::attributes::current_thread_id::value_type>("ThreadID");
    auto fmtSeverity  = expr::attr<logging::trivial::severity_level>("Severity");
    // auto fmtScope     = expr::format_named_scope("Scope",
    //     keywords::format = "%n(%f:%l)",
    //     keywords::iteration = expr::reverse,
    //     keywords::depth = 2);
    logging::formatter logFmt =
        expr::format("%1%|%2%|%3%: %4%")
        % fmtTimeStamp % fmtThreadId % fmtSeverity % expr::smessage;
    logFmt(rec, strm);
}

// This works in Windows, it colors the output.
void coloured_console_sink::consume(logging::record_view const& rec, string_type const& formatted_string)
{
    auto level = rec.attribute_values()["Severity"].extract<logging::trivial::severity_level>();
    auto hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hstdout, &csbi);

    SetConsoleTextAttribute(hstdout, get_colour(level.get()));   //! This stuff is apparently deprecated and old.
    std::cout << formatted_string << std::endl;
    SetConsoleTextAttribute(hstdout, csbi.wAttributes);
}


void init_logging(bool debug = false, std::string filename="defaultLog.log") {
    typedef sinks::synchronous_sink<coloured_console_sink> coloured_console_sink_t;
    auto coloured_console_sink = boost::make_shared<coloured_console_sink_t>();

    coloured_console_sink->set_formatter(&my_formatter);
    logging::core::get()->add_sink(coloured_console_sink);


    logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");
    logging::add_file_log
    (
        keywords::file_name = filename,
        keywords::format = "%TimeStamp%|%ThreadID%|%Severity%: %Message%",
        keywords::open_mode = std::ios_base::app,  //Append to file, don't just overwrite each time
        keywords::auto_flush = true
    );
    if (debug) {
		logging::core::get()->set_filter( logging::trivial::severity >= logging::trivial::trace);
	} else {
		logging::core::get()->set_filter( logging::trivial::severity >= logging::trivial::info );
    }

    logging::add_common_attributes();
    logInfo << "\n\nLogging started.";
    logDebug << "Debug on.";
}
