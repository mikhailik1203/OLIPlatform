//
// Created by sam1203 on 11/11/17.
//

#include "PriceLevelContainer.h"

using namespace oli::mengine;

namespace{
    const size_t ORDER_COUNT_IN_BLOCK = 32;

    const size_t INVALID_PRICELEVEL_BLOCK = 0;
    const size_t PRICELEVEL_INFO_INDEX_MASK = (size_t)(-1) << 5;
    const size_t ORDERS_INDEX_MASK = ~(~0 << 5);
    const size_t INVALID_ORDER_INDEX = 0;

    const size_t PRICEBLOCKINFO_POSITIONS_COUNT = sizeof(PriceLevelContainer::PriceBlockInfo)/sizeof(OrderData) + 1;
    const uint32_t PRICEBLOCKINFO_BITS_MASK = ~(~0 << (PRICEBLOCKINFO_POSITIONS_COUNT));
    const size_t FIRST_ORDER_INDEX_IN_BLOCK = PRICEBLOCKINFO_POSITIONS_COUNT + 1;
    const size_t LAST_ORDER_INDEX_IN_BLOCK = ORDER_COUNT_IN_BLOCK - 1;
    const uint32_t EMPTY_ORDER_BIT_MASK = PRICEBLOCKINFO_BITS_MASK;
    const uint32_t FIRST_ORDER_BIT_MASK = 1 << PRICEBLOCKINFO_POSITIONS_COUNT;
    const uint32_t FULLFILLED_ORDER_BIT_MASK = -1;

    PriceLevelContainer::PriceBlockInfo *getPriceBlockInfo(PriceLevelContainer::PriceLevelsT &prices, size_t index){
        return reinterpret_cast<PriceLevelContainer::PriceBlockInfo *>(&prices[index]);
    }
    const PriceLevelContainer::PriceBlockInfo *getPriceBlockInfo(const PriceLevelContainer::PriceLevelsT &prices, size_t index){
        return reinterpret_cast<const PriceLevelContainer::PriceBlockInfo *>(&prices[index]);
    }

    PriceLevelContainer::PriceBlockInfo *setPriceBlockInfo(PriceLevelContainer::PriceLevelsT &prices,
                                                           size_t index,
                                                           const PriceLevelContainer::PriceBlockInfo &val){
        PriceLevelContainer::PriceBlockInfo *info =
                reinterpret_cast<PriceLevelContainer::PriceBlockInfo *>(&prices[index]);
        *info = val;
        return info;
    }


    void setOrderData(PriceLevelContainer::PriceLevelsT &prices, size_t index, const OrderData &val){
        prices[index] = val;
    }

    bool isBlockComplete(const PriceLevelContainer::PriceBlockInfo &val){
        return FULLFILLED_ORDER_BIT_MASK == val.orderMask_;
    }

    bool isBlockEmpty(const PriceLevelContainer::PriceBlockInfo &val){
        return 0 == val.orderMask_;
    }

    char getFirstUsedIndex(uint32_t val){
        uint32_t mask = val;
        char index = FIRST_ORDER_INDEX_IN_BLOCK - 1;
        while((index < LAST_ORDER_INDEX_IN_BLOCK + 1) && (mask > 0))
        {
            if(0 != (mask & FIRST_ORDER_BIT_MASK))
                return index;
            mask >>= 1;
            ++index;
        }
        return 0;
    }
    char getLastUsedIndex(uint32_t val){
        int res = LAST_ORDER_INDEX_IN_BLOCK - __builtin_clz(val);
        return res;
    }

    char getNextUsedIndex(uint32_t val, char currIdx){
        if(ORDER_COUNT_IN_BLOCK <= currIdx + 1)
            return 0;
        uint32_t mask = val >> (currIdx + 1);
        char index = currIdx + 1;
        while((index < LAST_ORDER_INDEX_IN_BLOCK + 1) && (mask > 0))
        {
            if(0 != (mask & 1))
                return index;
            mask >>= 1;
            ++index;
        }
        return 0;
    }

    bool isUsedIndex(uint32_t mask, uint32_t index){
        uint32_t val = 1 << index;
        return 0 != (mask & val);
    }

    uint32_t nextBitMaskIndex(uint32_t mask){
        return getLastUsedIndex(mask) + 1;
    }

    size_t nextIndex(size_t blockIndex, uint32_t mask){
        size_t val = blockIndex + nextBitMaskIndex(mask);
        return val;
    }

}


PriceLevelIterator::PriceLevelIterator(PriceLevelContainer *cont, size_t positionIndex):
        cont_(cont), positionIndex_(positionIndex)
{}

PriceLevelIterator::~PriceLevelIterator()
{
    cont_ = nullptr;
}

bool PriceLevelIterator::isValid()const
{
    return 0 < positionIndex_;
}

void PriceLevelIterator::next()
{
    assert(nullptr != cont_);
    positionIndex_ = cont_->getNext(positionIndex_);
}

OrderData &PriceLevelIterator::data()const
{
    assert(nullptr != cont_);
    return cont_->orderData(positionIndex_);
}

PriceLevelContainer::PriceLevelContainer(size_t orderCount):
        priceLevels_(((orderCount + orderCount*0.2)/64 + 1)*64, OrderData()),
        freePriceLevels_(((orderCount + orderCount*0.2)/64 + 1)*64/ORDER_COUNT_IN_BLOCK - 1, 0)
{
    size_t ordCount = ((orderCount + orderCount*0.2)/64 + 1)*64;
    order2Index_.reserve(ordCount);
    size_t v = INVALID_ORDER_INDEX;
    std::generate (freePriceLevels_.begin(), freePriceLevels_.end(), [&]()->size_t{
        v += ORDER_COUNT_IN_BLOCK;
        return v;
    });
}

PriceLevelContainer::~PriceLevelContainer()
{
}

size_t PriceLevelContainer::getNextFreeBlockIndex()
{
    assert(!freePriceLevels_.empty());
    size_t freeBlockIndex = freePriceLevels_.front();
    freePriceLevels_.pop_front();
    return freeBlockIndex;
}

void PriceLevelContainer::add(const MDLevel2Record &order)
{
    auto pxLevel = priceLevelInfos_.find(order.price_);
    if(priceLevelInfos_.end() == pxLevel){
        size_t freeBlockIndex = getNextFreeBlockIndex();

        pxLevel = priceLevelInfos_.insert(
                Price2PriceLevelInfosT::value_type(order.price_,
                                                   PriceLevelInfo(freeBlockIndex, freeBlockIndex))).first;

        setPriceBlockInfo(priceLevels_, freeBlockIndex,
                          PriceBlockInfo(order.price_, freeBlockIndex, freeBlockIndex, EMPTY_ORDER_BIT_MASK));
    }

    size_t currBlockIndex = pxLevel->second.lastBlockIdx_;
    PriceBlockInfo *info = getPriceBlockInfo(priceLevels_, currBlockIndex);
    assert(nullptr != info);

    /// if last block is full - allocate new block
    if(isBlockComplete(*info)){
        size_t freeBlockIndex = getNextFreeBlockIndex();

        info->nextBlockIdx_ = freeBlockIndex;
        pxLevel->second.lastBlockIdx_ = freeBlockIndex;

        info = setPriceBlockInfo(priceLevels_, freeBlockIndex,
                          PriceBlockInfo(order.price_, currBlockIndex, freeBlockIndex, EMPTY_ORDER_BIT_MASK));
    }

    /// add order at this index into priceLevels_
    uint32_t nextBitMaskIdx = nextBitMaskIndex(info->orderMask_);
    size_t  orderIndex = info->nextBlockIdx_ + nextBitMaskIdx;
    OrderData orderData = {order.id_, order.qty_, order.price_};
    setOrderData(priceLevels_, orderIndex, orderData);
    info->orderMask_ |= (1 << nextBitMaskIdx);

    /// add order to the orderId->index map
    order2Index_.emplace(order.id_, orderIndex);
}

void PriceLevelContainer::remove(const MDLevel2Record &order)
{
    /// find order index in orderId->index map
    OrderId2IndexT::iterator ordIdIt = order2Index_.find(order.id_);
    if(order2Index_.end() == ordIdIt)
        return;

    size_t orderDataIndex = ordIdIt->second;
    size_t priceLvlBlockIndex = orderDataIndex & PRICELEVEL_INFO_INDEX_MASK;

    /// find PriceBlockInfo for this block
    PriceBlockInfo *info = getPriceBlockInfo(priceLevels_, priceLvlBlockIndex);
    assert(nullptr != info);

    /// remove order from priceLevels_
    /// update price block mask
    info->orderMask_ &= ~(1 << (orderDataIndex - priceLvlBlockIndex));

    OrderData &oldOrderVal = priceLevels_[orderDataIndex];

    /// free price block, if mask is empty
    if(EMPTY_ORDER_BIT_MASK == info->orderMask_){
        if((priceLvlBlockIndex == info->prevBlockIdx_) && (priceLvlBlockIndex == info->nextBlockIdx_)){
            /// this is the last block for this price - erase it
            priceLevelInfos_.erase(order.price_);
        }else if((priceLvlBlockIndex != info->prevBlockIdx_) && (priceLvlBlockIndex == info->nextBlockIdx_)){
            PriceBlockInfo *prevBlockInfo = getPriceBlockInfo(priceLevels_, info->prevBlockIdx_);
            assert(nullptr != prevBlockInfo);
            prevBlockInfo->nextBlockIdx_ = info->prevBlockIdx_;
            auto plIt = priceLevelInfos_.find(order.price_);
            assert(priceLevelInfos_.end() != plIt);
            plIt->second.lastBlockIdx_ = info->prevBlockIdx_;
        }else if((priceLvlBlockIndex == info->prevBlockIdx_) && (priceLvlBlockIndex != info->nextBlockIdx_)){
            PriceBlockInfo *nextBlockInfo = getPriceBlockInfo(priceLevels_, info->nextBlockIdx_);
            assert(nullptr != nextBlockInfo);
            nextBlockInfo->prevBlockIdx_ = info->nextBlockIdx_;
            auto plIt = priceLevelInfos_.find(order.price_);
            assert(priceLevelInfos_.end() != plIt);
            plIt->second.firstBlockIdx_ = info->nextBlockIdx_;
        }else{
            PriceBlockInfo *prevBlockInfo = getPriceBlockInfo(priceLevels_, info->prevBlockIdx_);
            assert(nullptr != prevBlockInfo);
            prevBlockInfo->nextBlockIdx_ = info->nextBlockIdx_;
            PriceBlockInfo *nextBlockInfo = getPriceBlockInfo(priceLevels_, info->nextBlockIdx_);
            assert(nullptr != nextBlockInfo);
            nextBlockInfo->prevBlockIdx_ = info->prevBlockIdx_;
        }
        freePriceLevels_.push_front(priceLvlBlockIndex);
    }

    order2Index_.erase(ordIdIt);
}

void PriceLevelContainer::update(const MDLevel2Record &order)
{
    /// find order index in orderId->index map
    OrderId2IndexT::iterator ordIdIt = order2Index_.find(order.id_);
    if(order2Index_.end() == ordIdIt)
        return;

    size_t orderDataIndex = ordIdIt->second;
    size_t priceLvlBlockIndex = orderDataIndex & PRICELEVEL_INFO_INDEX_MASK;

    /// find PriceBlockInfo for this block
    PriceBlockInfo *info = getPriceBlockInfo(priceLevels_, priceLvlBlockIndex);
    assert(nullptr != info);

    /// if qty was changed - move order back to the last PriceBlock
    /// if price was changed - remove order from PriceBlock, find another PriceLevel and add to the last PriceBlock

    /// remove order from priceLevels_
    /// update price block mask
/*    info.orderMask_ &= ~(1 << orderDataIndex);

    /// free price block, if mask is empty
    if(EMPTY_ORDER_BIT_MASK == info.orderMask_){
        if((priceLvlBlockIndex == info.prevBlockIdx_) && (priceLvlBlockIndex == info.nextBlockIdx_)){
            /// this is the last block for this price - erase it
            priceLevelInfos_.erase(order.price_);
        }else if((priceLvlBlockIndex != info.prevBlockIdx_) && (priceLvlBlockIndex == info.nextBlockIdx_)){
            PriceBlockInfo &prevBlockInfo = getPriceBlockInfo(priceLevels_, info.prevBlockIdx_);
            prevBlockInfo.nextBlockIdx_ = info.prevBlockIdx_;
            auto plIt = priceLevelInfos_.find(order.price_);
            assert(priceLevelInfos_.end() != plIt);
            plIt->second.lastBlockIdx_ = info.prevBlockIdx_;
        }else if((priceLvlBlockIndex == info.prevBlockIdx_) && (priceLvlBlockIndex != info.nextBlockIdx_)){
            PriceBlockInfo &nextBlockInfo = getPriceBlockInfo(priceLevels_, info.nextBlockIdx_);
            nextBlockInfo.prevBlockIdx_ = info.nextBlockIdx_;
            auto plIt = priceLevelInfos_.find(order.price_);
            assert(priceLevelInfos_.end() != plIt);
            plIt->second.firstBlockIdx_ = info.nextBlockIdx_;
        }else{
            PriceBlockInfo &prevBlockInfo = getPriceBlockInfo(priceLevels_, info.prevBlockIdx_);
            prevBlockInfo.nextBlockIdx_ = info.nextBlockIdx_;
            PriceBlockInfo &nextBlockInfo = getPriceBlockInfo(priceLevels_, info.nextBlockIdx_);
            nextBlockInfo.prevBlockIdx_ = info.prevBlockIdx_;
        }
    }*/

}

PriceLevelIterator PriceLevelContainer::begin()
{
    if(priceLevelInfos_.empty())
        return end();
    auto pxLvlInfoIt = priceLevelInfos_.begin();
    size_t startIdx = pxLvlInfoIt->second.firstBlockIdx_;
    PriceBlockInfo *blockInfo = getPriceBlockInfo(priceLevels_, startIdx);
    assert(nullptr != blockInfo);
    assert(EMPTY_ORDER_BIT_MASK != blockInfo->orderMask_);
    return PriceLevelIterator(this, startIdx + getFirstUsedIndex(blockInfo->orderMask_));
}

PriceLevelIterator PriceLevelContainer::end()
{
    return PriceLevelIterator(this, INVALID_ORDER_INDEX);
}

size_t PriceLevelContainer::getNext(size_t currIdx)const
{
    do{
        uint32_t currBlockIndex = (currIdx & PRICELEVEL_INFO_INDEX_MASK);
        auto blockInfo = getPriceBlockInfo(priceLevels_, currBlockIndex);
        assert(nullptr != blockInfo);
        assert(EMPTY_ORDER_BIT_MASK != blockInfo->orderMask_);
        uint32_t indexInBlock = (currIdx & ORDERS_INDEX_MASK);
        uint32_t nextIndexInBlock = getNextUsedIndex(blockInfo->orderMask_, indexInBlock);

        if(0 != nextIndexInBlock)
            return (currIdx & PRICELEVEL_INFO_INDEX_MASK) + nextIndexInBlock;
        if(currBlockIndex == blockInfo->nextBlockIdx_){
            /// choose next price level
            auto pxLevel = priceLevelInfos_.upper_bound(blockInfo->priceLevel_);
            if(priceLevelInfos_.end() == pxLevel)
                return INVALID_ORDER_INDEX;
            auto nextBlockInfo = getPriceBlockInfo(priceLevels_, pxLevel->second.firstBlockIdx_);
            assert(nullptr != nextBlockInfo);
            return pxLevel->second.firstBlockIdx_ + getFirstUsedIndex(nextBlockInfo->orderMask_);
        }else{
            auto nextBlockInfo = getPriceBlockInfo(priceLevels_, blockInfo->nextBlockIdx_);
            assert(nullptr != nextBlockInfo);
            return blockInfo->nextBlockIdx_ + getFirstUsedIndex(nextBlockInfo->orderMask_);
        }
    }while(true);
    return 0;
}

OrderData &PriceLevelContainer::orderData(size_t currIdx)
{
    PriceBlockInfo *blockInfo = getPriceBlockInfo(priceLevels_, (currIdx & PRICELEVEL_INFO_INDEX_MASK));
    assert(nullptr != blockInfo);
    assert(EMPTY_ORDER_BIT_MASK != blockInfo->orderMask_);
    uint32_t indexInBlock = currIdx & ORDERS_INDEX_MASK;
    if(!isUsedIndex(blockInfo->orderMask_, indexInBlock))
        throw std::runtime_error("PriceLevelContainer::orderData: OrderData is not used at index!");
    return priceLevels_[currIdx];
}


PriceLevelContainer2Cmp::PriceLevelContainer2Cmp(size_t orderCount)
{
    orderIds_.reserve(orderCount);
}

PriceLevelContainer2Cmp::~PriceLevelContainer2Cmp()
{

}

void PriceLevelContainer2Cmp::add(const MDLevel2Record &order)
{
    auto pxLevel = priceLevels_.find(order.price_);
    if(priceLevels_.end() == pxLevel){
        OrderAtPriceLevelT lst;
        lst.push_back({order.id_, order.qty_, order.price_});
        auto plIt = priceLevels_.insert({order.price_, lst}).first;
        orderIds_[order.id_] = plIt->second.begin();
        return;
    }
    auto lstIt = pxLevel->second.insert(pxLevel->second.end(), {order.id_, order.qty_, order.price_});
    orderIds_[order.id_] = lstIt;
}

void PriceLevelContainer2Cmp::remove(const MDLevel2Record &order)
{
    auto ordIt = orderIds_.find(order.id_);
    if(orderIds_.end() == ordIt)
        return;
    auto pxLevel = priceLevels_.find(ordIt->second->price_);
    if(priceLevels_.end() == pxLevel)
        return;
    pxLevel->second.erase(ordIt->second);
    if(pxLevel->second.empty())
        priceLevels_.erase(pxLevel);
    orderIds_.erase(ordIt);
}

void PriceLevelContainer2Cmp::update(const MDLevel2Record &order)
{
}

PriceLevelContainer2Cmp::PriceLevelIterator2Cmp PriceLevelContainer2Cmp::begin()
{
    if(priceLevels_.empty())
        return end();
    auto it = priceLevels_.begin();
    return PriceLevelContainer2Cmp::PriceLevelIterator2Cmp(priceLevels_);
}

PriceLevelContainer2Cmp::PriceLevelIterator2Cmp PriceLevelContainer2Cmp::end()
{
    auto it = priceLevels_.end();
    return PriceLevelContainer2Cmp::PriceLevelIterator2Cmp(priceLevels_, false);
}

size_t PriceLevelContainer2Cmp::getNext(size_t currIdx)const
{

}
OrderData &PriceLevelContainer2Cmp::orderData(size_t currIdx)
{

}

PriceLevelContainer2Cmp::PriceLevelIterator2Cmp::PriceLevelIterator2Cmp(
        BidOrderBookT &cont, bool valid):
            cont_(cont), priceIt_(cont_.begin()), valid_(valid)
{
    if(valid_)
        ordIt_ = priceIt_->second.begin();
}

PriceLevelContainer2Cmp::PriceLevelIterator2Cmp::~PriceLevelIterator2Cmp()
{

}

bool PriceLevelContainer2Cmp::PriceLevelIterator2Cmp::isValid()const
{
    return valid_;
}

void PriceLevelContainer2Cmp::PriceLevelIterator2Cmp::next()
{
    if(!valid_)
        return;
    ++ordIt_;
    if(ordIt_ == priceIt_->second.end()){
        ++priceIt_;
        valid_ = (priceIt_ != cont_.end());
        if(!valid_)
            return;
        ordIt_ = priceIt_->second.begin();
    }
}

void PriceLevelContainer2Cmp::PriceLevelIterator2Cmp::nextAtPriceLevel()
{
    if(!valid_)
        return;
    ++ordIt_;
    valid_ = (ordIt_ != priceIt_->second.end());
}

OrderData &PriceLevelContainer2Cmp::PriceLevelIterator2Cmp::data()const
{
    return *ordIt_;
}

