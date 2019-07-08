#ifndef INSTRUMENTDISPATCHER_H
#define INSTRUMENTDISPATCHER_H

#include "SecMasterDef.h"

#include <mutex>

namespace oli
{

namespace secmaster
{

/// process Instrument events and keeps Instrument parameters
class InstrumentDispatcher final: public IInstrumentMessageObserver
{
public:
    InstrumentDispatcher();
    virtual ~InstrumentDispatcher();
    
public:
    /// implementation of IInstrumentMessageObserver
    virtual void onInstrumentData(const InstrumentRecord &instr) override;
    virtual void onInstrumentChanged(const oli::idT &instrId, InstrumentStatus status) override;

private:
    mutable std::mutex lock_;
    typedef std::map<oli::idT, InstrumentRecord> InstrumentDefsT;
    InstrumentDefsT instruments_;

};

}

}

#endif // INSTRUMENTDISPATCHER_H
