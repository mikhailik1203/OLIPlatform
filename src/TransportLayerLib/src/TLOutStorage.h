/*
 * TLOutStorage.h
 *
 *  Created on: Aug 15, 2015
 *      Author: sam1203
 */
 
#ifndef TLOUTSTORAGE_H
#define TLOUTSTORAGE_H

#include <vector>

namespace oli
{

namespace tlimpl
{

class TLOutStorage
{
public:
    TLOutStorage();
    ~TLOutStorage();

    void putMessage(const std::vector<char> &msg);
    
};

}

}

#endif // TLOUTSTORAGE_H
