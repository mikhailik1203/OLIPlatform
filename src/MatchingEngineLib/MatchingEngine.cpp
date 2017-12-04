//
// Created by sam1203 on 9/18/17.
//

#include "MatchingEngine.h"
#include <cassert>

using namespace oli::mengine;

namespace{

}

MatchingEngine::MatchingEngine()
{}

MatchingEngine::~MatchingEngine()
{}

void MatchingEngine::attach(IOutMEObserver *obs)
{
    obs_ = obs;
}

void MatchingEngine::attach(IOrderBookProcessor *proc)
{
    proc_ = proc;
}

void MatchingEngine::onNewOrderAggressor(const AggressorOrderData &order)
{
    aggrOrderMatcher_.init(&order);
    (*proc_).process(&aggrOrderMatcher_);
    (*obs_).onOrdersMatched(OrderMatchEvent(aggrOrderMatcher_.matchedOrders()));
}

void MatchingEngine::onNewMarketOrder(const MarketOrderData &order)
{
    marketOrderMatcher_.init(&order);
    (*proc_).process(&marketOrderMatcher_);
    (*obs_).onOrdersMatched(OrderMatchEvent(marketOrderMatcher_.matchedOrders()));
}

