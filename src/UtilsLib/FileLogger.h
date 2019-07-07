//
// Created by sam1203 on 2/7/18.
//

#ifndef UTILSLIB_FILELOGGER_H
#define UTILSLIB_FILELOGGER_H

#include "OLI_Logger.h"
#include <memory>

namespace oli{
    namespace utils {

        struct FileLoggerImpl;

        class FileLogger final : public oli::Logger {
        public:
            FileLogger(const std::string &configFile, const std::string &nodename);
            virtual ~FileLogger();

        public:
            virtual void trace(const std::string &msg) override;
            virtual void debug(const std::string &msg) override;
            virtual void note(const std::string &msg) override;
            virtual void warn(const std::string &msg) override;
            virtual void error(const std::string &msg) override;
            virtual void fatal(const std::string &msg) override;

        private:
            std::unique_ptr<FileLoggerImpl> impl_;
        };

    }

}



#endif //UTILSLIB_FILELOGGER_H
