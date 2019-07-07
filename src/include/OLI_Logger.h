#ifndef __OLI_LOGGER__H__
#define __OLI_LOGGER__H__

#include "string"
#include "sstream"
#include "memory"

namespace oli{
    
    /**
     * @class Logger
     * @author Sergey A Mikhailik
     * @date 10/03/16
     * @file OLI_Logger.h
     * @brief Wrapper for the logger 
     */
    class Logger{
    public:
        Logger():
                traceEnabled_(true), debugEnabled_(true), noteEnabled_(true), warnEnabled_(true)
        {}

        virtual ~Logger(){}
        
        virtual void trace(const std::string &msg) = 0;
        virtual void debug(const std::string &msg) = 0;
        virtual void note(const std::string &msg) = 0;
        virtual void warn(const std::string &msg) = 0;
        virtual void error(const std::string &msg) = 0;
        virtual void fatal(const std::string &msg) = 0;

        static std::unique_ptr<Logger> create(const std::string &configName, const std::string &nodeName);
    public:
        inline bool traceEnabled()const{return traceEnabled_;}
        inline bool debugEnabled()const{return debugEnabled_;}
        inline bool noteEnabled()const{return noteEnabled_;}
        inline bool warnEnabled()const{return warnEnabled_;}

        void setTrace(bool enabled){traceEnabled_ = enabled;}
        void setDebug(bool enabled){debugEnabled_ = enabled;}
        void setNote(bool enabled){noteEnabled_ = enabled;}
        void setWarn(bool enabled){warnEnabled_ = enabled;}

    private:
        bool traceEnabled_;
        bool debugEnabled_;
        bool noteEnabled_;
        bool warnEnabled_;
    };
}

#define LOGR_TRACE(LOGGER, MESSAGE) \
{ if((LOGGER).traceEnabled()) { std::stringstream msg4Log; msg4Log<< std::fixed<< MESSAGE; (LOGGER).trace(msg4Log.str()); } }

#define LOG_TRACE(LOGGER, MESSAGE) \
{if(nullptr != (LOGGER) && (LOGGER)->traceEnabled()) { LOGR_TRACE(*(LOGGER), MESSAGE) } }

#define LOGR_DEBUG(LOGGER, MESSAGE) \
{ if((LOGGER).debugEnabled()) { std::stringstream msg4Log; msg4Log<< std::fixed<< MESSAGE; (LOGGER).debug(msg4Log.str()); } }

#define LOG_DEBUG(LOGGER, MESSAGE) \
{if(nullptr != (LOGGER) && (LOGGER)->debugEnabled()) { LOGR_DEBUG(*(LOGGER), MESSAGE) } }

#define LOGR_NOTE(LOGGER, MESSAGE) \
{ if((LOGGER).noteEnabled()) { std::stringstream msg4Log; msg4Log<< std::fixed<< MESSAGE; (LOGGER).note(msg4Log.str()); } }

#define LOG_NOTE(LOGGER, MESSAGE) \
{if(nullptr != (LOGGER) && (LOGGER)->noteEnabled()) { LOGR_NOTE(*(LOGGER), MESSAGE) } }

#define LOGR_WARN(LOGGER, MESSAGE) \
{ if((LOGGER).warnEnabled()) { std::stringstream msg4Log; msg4Log<< std::fixed<< MESSAGE; (LOGGER).warn(msg4Log.str()); } }

#define LOG_WARN(LOGGER, MESSAGE) \
{if(nullptr != (LOGGER) && (LOGGER)->warnEnabled()) { LOGR_WARN(*(LOGGER), MESSAGE) } }

#define LOGR_ERROR(LOGGER, MESSAGE) \
{std::stringstream msg4Log; msg4Log<< std::fixed<< MESSAGE; (LOGGER).error(msg4Log.str()); }

#define LOG_ERROR(LOGGER, MESSAGE) \
{if(nullptr != (LOGGER)) { LOGR_ERROR(*(LOGGER), MESSAGE ); } }

#define LOGR_FATAL(LOGGER, MESSAGE) \
{std::stringstream msg4Log; msg4Log<< std::fixed<< MESSAGE; (LOGGER).fatal(msg4Log.str()); }

#define LOG_FATAL(LOGGER, MESSAGE) \
{if(nullptr != (LOGGER)) { LOGR_FATAL(*(LOGGER), MESSAGE) }}


#define LOG_TRACE_CTX(CTX, MESSAGE) \
{LOG_TRACE(((CTX).logger_), MESSAGE)}

#define LOG_DEBUG_CTX(CTX, MESSAGE) \
{LOG_DEBUG(((CTX).logger_), MESSAGE)}

#define LOG_NOTE_CTX(CTX, MESSAGE) \
{LOG_NOTE(((CTX).logger_), MESSAGE)}

#define LOG_WARN_CTX(CTX, MESSAGE) \
{LOG_WARN(((CTX).logger_), MESSAGE)}

#define LOG_ERROR_CTX(CTX, MESSAGE) \
{LOG_ERROR(((CTX).logger_), MESSAGE)}

#define LOG_FATAL_CTX(CTX, MESSAGE) \
{LOG_FATAL(((CTX).logger_), MESSAGE)}

#endif //__OLI_LOGGER__H__
