/*
 * SecMasterDef.h
 *
 *  Created on: Nov 15, 2016
 *      Author: sam1203
 */
 
#ifndef SECMASTERDEF_H
#define SECMASTERDEF_H

#include "OLI_UtilsDefines.h"
#include "OLI_FinanceDomain.h"

#include <deque>
#include <set>
#include <map>

namespace oli
{

namespace secmaster
{

    /// Content of the Instrument message
    struct InstrumentRecord{
        oli::idT instrId_;
        std::string name_;
        
        oli::priceT lowPrice_; 
        oli::priceT highPrice_;
        oli::priceT priceStep_;
        
        InstrumentRecord(): instrId_(oli::INVALID_ID), name_(), 
                lowPrice_(0), highPrice_(0), priceStep_(0)
        {}
        InstrumentRecord(const oli::idT &instrId, const std::string &name, 
                const oli::priceT &lowPrice, const oli::priceT &highPrice,
                const oli::priceT &priceStep): 
            instrId_(instrId), name_(name), lowPrice_(lowPrice), 
            highPrice_(highPrice), priceStep_(priceStep)
        {}
        
    };
    
    typedef std::set<oli::idT> InstrumentIdsT;
    
    /// interface to process incoming Instrument message 
    class IInstrumentMessageObserver{
    public:
        virtual ~IInstrumentMessageObserver(){}
        
        /// callback is called when new instrument message received
        virtual void onInstrumentData(const InstrumentRecord &instr) = 0;
        
        /// callback is called when new instrumentStatus message received
        virtual void onInstrumentChanged(const oli::idT &instrId, InstrumentStatus status) = 0;
    };


    /// interface to process incoming Instrument events
    class IInstrumentEventObserver{
    public:
        virtual ~IInstrumentEventObserver(){}
        
        /// callback is called when new instrument added/activated 
        virtual void onAddInstrument(const InstrumentRecord &instr) = 0;
        
        /// callback is called when new instrument removed/halted 
        virtual void onRemoveInstrument(const InstrumentRecord &instr) = 0;
    };

    /// interface to provide up-to-date definition of instrument
    class IInstrumentCache{
    public:
        virtual ~IInstrumentCache(){}
        
        /// returns defintion of instrument by id
        virtual InstrumentRecord findInstrument(const oli::idT &instrId)const = 0;
        
        // returns set of ids for defined instruments
        virtual void getAvailableInstruments(InstrumentIdsT &instr)const = 0;
        
        /// attach observer for SecMaster events
        virtual void attach(IInstrumentEventObserver *obs) = 0;
        
        /// remove observer of SecMasater events
        virtual void dettach(IInstrumentEventObserver *obs) = 0;
    };

}
}

#endif //SECMASTERDEF_H