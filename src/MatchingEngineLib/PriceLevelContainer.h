//
// Created by sam1203 on 11/11/17.
//

#ifndef MATCHINGENGINELIB_PRICELEVELCONTAINER_H
#define MATCHINGENGINELIB_PRICELEVELCONTAINER_H

#include "MatchingEngineDef.h"
#include <list>
#include <map>
#include <algorithm>

namespace oli {

    namespace mengine {

        class PriceLevelContainer;

        class PriceLevelIterator{
        public:
            PriceLevelIterator(PriceLevelContainer *cont, size_t positionIndex);
            ~PriceLevelIterator();

            bool isValid()const;

            void next();
            void nextAtPriceLevel();

            OrderData &data()const;
        private:
            PriceLevelContainer *cont_;
            size_t positionIndex_;
        };

        class PriceLevelContainer {
        public:
            explicit PriceLevelContainer(size_t orderCount = 1024);
            ~PriceLevelContainer();

            void add(const MDLevel2Record &order);
            void remove(const MDLevel2Record &order);
            void update(const MDLevel2Record &order);

            PriceLevelIterator begin();
            PriceLevelIterator end();

        public:
            size_t getNext(size_t currIdx)const;
            OrderData &orderData(size_t currIdx);
        public:
            struct PriceBlockInfo{
                oli::priceT priceLevel_;
                size_t prevBlockIdx_;
                size_t nextBlockIdx_;
                uint32_t orderMask_;
                //oli::Side side_;

                PriceBlockInfo():
                        priceLevel_(0), prevBlockIdx_(0), nextBlockIdx_(0), orderMask_(0)
                {}
                PriceBlockInfo(oli::priceT priceLevel, size_t prevBlockIdx,
                            size_t nextBlockIdx, uint32_t orderMask):
                        priceLevel_(priceLevel), prevBlockIdx_(prevBlockIdx),
                        nextBlockIdx_(nextBlockIdx), orderMask_(orderMask)
                {}
            };
            typedef std::vector<OrderData> PriceLevelsT;

            typedef std::deque<size_t> PriceBlocksT;
            typedef std::unordered_map<oli::idT, size_t> OrderId2IndexT;

            struct PriceLevelInfo {
                size_t firstBlockIdx_;
                size_t lastBlockIdx_;

                PriceLevelInfo(): firstBlockIdx_(0), lastBlockIdx_(0)
                {}
                PriceLevelInfo(size_t fstBlockIdx, size_t lstBlockIdx):
                        firstBlockIdx_(fstBlockIdx), lastBlockIdx_(lstBlockIdx)
                {}
            };
            typedef std::map<oli::priceT, PriceLevelInfo> Price2PriceLevelInfosT;

        private:
            size_t getNextFreeBlockIndex();

        private:
            PriceLevelsT priceLevels_;

            Price2PriceLevelInfosT priceLevelInfos_;
            PriceBlocksT freePriceLevels_;

            OrderId2IndexT order2Index_;
            PriceBlocksT freePriceBlocks_;
        };


        class PriceLevelContainer2Cmp {
        public:
            explicit PriceLevelContainer2Cmp(size_t orderCount = 1024);
            ~PriceLevelContainer2Cmp();

            void add(const MDLevel2Record &order);
            void remove(const MDLevel2Record &order);
            void update(const MDLevel2Record &order);


        public:
            size_t getNext(size_t currIdx)const;
            OrderData &orderData(size_t currIdx);
        public:
            typedef std::list<OrderData> OrderAtPriceLevelT;
            typedef std::map<priceT, OrderAtPriceLevelT, std::greater<priceT>> BidOrderBookT;
            typedef std::map<priceT, OrderAtPriceLevelT> AskOrderBookT;
            typedef std::unordered_map<oli::idT, OrderAtPriceLevelT::iterator> OrderId2PositionT; //for replace/delete operations

            class PriceLevelIterator2Cmp{
            public:
                PriceLevelIterator2Cmp(BidOrderBookT &cont, bool valid = true);
                ~PriceLevelIterator2Cmp();

                bool isValid()const;

                void next();
                void nextAtPriceLevel();

                OrderData &data()const;
            private:
                BidOrderBookT &cont_;
                BidOrderBookT::iterator priceIt_;
                OrderAtPriceLevelT::iterator ordIt_;
                bool valid_;
            };

        public:
            PriceLevelIterator2Cmp begin();
            PriceLevelIterator2Cmp end();

        private:
            BidOrderBookT priceLevels_;
            OrderId2PositionT orderIds_;
        };

    }
}

#endif //MATCHINGENGINELIB_PRICELEVELCONTAINER_H
