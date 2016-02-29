/*
 * TLChannel.h
 *
 *  Created on: Aug 18, 2015
 *      Author: sam1203
 */

#ifndef TLCHANNEL_H
#define TLCHANNEL_H

#include "TransportLayerDef.h"
#include "TransportLayerImplDef.h"

#include <thread>
#include <atomic>

namespace oli
{

namespace tlimpl
{

class TLChannel final : public tl::Channel
{
public:
    explicit TLChannel(const ChannelConfig &cfg);
    ~TLChannel();

    void start();
    void stop();


public:
    virtual tl::ChannelObserver *attach(tl::ChannelObserver* obs) override;
    virtual tl::ChannelObserver *dettach() override;
    virtual void send(int64_t topic, const std::vector<char> &msg) override;         
    virtual void statistics(tl::ChannelStatistics &stat)const override;
    
private:
    ChannelConfig config_;
    
    std::thread execThread_;
    std::atomic_flag started_ = ATOMIC_FLAG_INIT;
};

}

}

#endif // TLCHANNEL_H
