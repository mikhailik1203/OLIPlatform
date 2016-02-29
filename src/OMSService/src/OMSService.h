#ifndef OMSSERVICE_H
#define OMSSERVICE_H

namespace oli
{

namespace oms
{

/// constructs OMS service, knows how to start and stop execution, etc
class OMSService
{
public:
    OMSService();
    ~OMSService();

    void construct();
    void start();
    void stop();
};

}

}

#endif // OMSSERVICE_H
