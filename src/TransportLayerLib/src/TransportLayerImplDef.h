/*
 * TransportLayerImplDef.h
 *
 *  Created on: Aug 19, 2015
 *      Author: sam1203
 */
 
#ifndef __TRANSPORTLAYER_IMPL_DEF__H__
#define __TRANSPORTLAYER_IMPL_DEF__H__

#include <set>
#include <map>
#include <string>
#include <vector>

namespace oli{
    namespace tlimpl{

        enum ChannelRole{
            invalid_ChannelRole = 0,
            publisher_ChannelRole,
            subscriber_ChannelRole,
        };
        
        enum StorageType{
            invalid_StorageType = 0,
            persistent_StorageType,
            memory_StorageType,
            none_StorageType
        };
        
        struct StorageConfig{
        public:
            StorageConfig();
            ~StorageConfig();
        public:
            StorageType type_;
            std::string path_;
            size_t rotateSize_;
        };
        
        struct ChannelConfig{
        public:
            ChannelConfig();
            ~ChannelConfig();
        public:
            std::string name_;
            ChannelRole role_;
            std::string connection_;
            StorageConfig inStorage_;
            StorageConfig outStorage_;
        };      
        typedef std::map<std::string, ChannelConfig> ChannelsConfigT;
        
        struct NodeConfig{
        public:
            NodeConfig();
            ~NodeConfig();
            
        public:
            std::string name_;
            ChannelsConfigT channels_;
        };
        
        typedef std::map<std::string, NodeConfig> NodesConfigT;
        
        
        
    }
}

#endif //__TRANSPORTLAYER_IMPL_DEF__H__