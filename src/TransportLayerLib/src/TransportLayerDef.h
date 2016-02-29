/*
 * TransportLayerDef.h
 *
 *  Created on: Aug 15, 2015
 *      Author: sam1203
 */
 
#ifndef __TRANSPORTLAYER_DEF__H__
#define __TRANSPORTLAYER_DEF__H__

#include <set>
#include <map>
#include <string>
#include <vector>

namespace oli{
    namespace tl{
    
        class ChannelObserver{
        public:    
            virtual ~ChannelObserver(){}
            
            virtual void onMessage(int64_t topic, const std::vector<char> &msg) = 0;
            virtual void onResendRequest() = 0;
        };
        
        class ChannelStatistics{
        public:
            ChannelStatistics();
            ~ChannelStatistics();
            
        public:
            
        };
        
        class Channel{
        public:
            virtual ~Channel(){}
            
        public:
            const std::string &name()const;
            
            virtual ChannelObserver *attach(ChannelObserver* obs) = 0;
            virtual ChannelObserver *dettach() = 0;
            
            virtual void send(int64_t topic, const std::vector<char> &msg) = 0;
            
            virtual void statistics(ChannelStatistics &stat)const = 0;
        };
        
        typedef std::set<std::string> ChannelNamesT;
        typedef std::map<std::string, Channel*> ChannelsT;
    }    
    
}

#endif