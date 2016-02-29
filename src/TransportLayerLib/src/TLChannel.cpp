#include "TLChannel.h"

using namespace oli::tlimpl;

namespace{

void process(TLChannel *channel){
    
}    
    
}

TLChannel::TLChannel(const ChannelConfig &cfg): 
    config_(cfg), execThread_(), started_(true)
{
}

TLChannel::~TLChannel()
{
}

void TLChannel::start()
{
    execThread_ = std::thread(process, this);
}

void TLChannel::stop()
{
    execThread_.join();    
}
