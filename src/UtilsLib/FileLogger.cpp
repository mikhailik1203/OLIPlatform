//
// Created by sam1203 on 2/7/18.
//

#include "FileLogger.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wterminate"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#pragma GCC diagnostic pop


using namespace oli::utils;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

namespace oli {
    namespace utils {
        struct FileLoggerImpl{
            src::severity_logger< logging::trivial::severity_level > logger_;
        };
    }
}

std::unique_ptr<oli::Logger> oli::Logger::create(const std::string &configName, const std::string &nodeName)
{
    return std::unique_ptr<Logger>(new FileLogger(configName, nodeName));
}

FileLogger::FileLogger(const std::string &configFile, const std::string &nodename):
        impl_(new FileLoggerImpl)
{
    logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");

    logging::add_file_log(
        keywords::file_name = nodename + "_%N.log",
        keywords::rotation_size = 10 * 1024 * 1024,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::format = "%TimeStamp% [%Severity%]: %Message%",
        keywords::auto_flush = true
    );

    logging::trivial::severity_level sevFilter = logging::trivial::trace;
    if(this->traceEnabled())
        sevFilter = logging::trivial::trace;
    else if(this->debugEnabled())
        sevFilter = logging::trivial::debug;
    else if(this->noteEnabled())
        sevFilter = logging::trivial::info;
    else if(this->warnEnabled())
        sevFilter = logging::trivial::warning;

    logging::core::get()->set_filter(logging::trivial::severity >= sevFilter);

    logging::add_common_attributes();
}

FileLogger::~FileLogger()
{
}

void FileLogger::trace(const std::string &msg)
{
    BOOST_LOG_SEV(impl_->logger_, logging::trivial::trace) << msg;
}

void FileLogger::debug(const std::string &msg)
{
    BOOST_LOG_SEV(impl_->logger_, logging::trivial::debug) << msg;
}

void FileLogger::note(const std::string &msg)
{
    BOOST_LOG_SEV(impl_->logger_, logging::trivial::info) << msg;
}

void FileLogger::warn(const std::string &msg)
{
    BOOST_LOG_SEV(impl_->logger_, logging::trivial::warning) << msg;
}

void FileLogger::error(const std::string &msg)
{
    BOOST_LOG_SEV(impl_->logger_, logging::trivial::error) << msg;
}

void FileLogger::fatal(const std::string &msg)
{
    BOOST_LOG_SEV(impl_->logger_, logging::trivial::fatal) << msg;
}
