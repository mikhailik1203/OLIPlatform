/*
 * OLI_FinanceDomain.h
 *
 *  Created on: Oct 31, 2016
 *      Author: sam1203
 */
 
#ifndef OLI_FINANCEDOMAIN_H
#define OLI_FINANCEDOMAIN_H

#include <string>
#include <vector>

#include "OLI_UtilsDefines.h"

namespace oli
{

    enum class Side{
        invalid_Side = 0,
        buy_Side,
        sell_Side
    };
    
    enum class OrderType{
        invalid_orderType = 0,
        market_orderType,
        limit_orderType,
        stop_orderType
    };    
    
    enum class InstrumentStatus{
        invalid_instrumentStatus = 0,
        trading_instrumentStatus, /// status for active instrument
        prepared_instrumentStatus,/// status for going to trade instrument
        stopped_instrumentStatus  /// status for terminated/halted instrument
    };

    enum class InstrumentType{
        invalid_instrumentType = 0,
        stock_instrumentType
    };

    enum class OrderState{
        invalid_orderState = 0,
        pendingNew_orderState,
        pendingCancel_orderState,
        pendingReplace_orderState,
        new_orderState,
        partFill_orderState,
        filled_orderState,
        rejected_orderState,
        canceled_orderState,
        replaced_orderState,
        expired_orderState,
        
        total_orderState
    };
    
}

#endif