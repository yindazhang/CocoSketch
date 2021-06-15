#ifndef CMHEAP_H
#define CMHEAP_H

#include "CMSketch.h"
#include "Heap.h"

template<typename DATA_TYPE>
class CMHeap : public SingleAbstract<DATA_TYPE>{
public:
    typedef std::unordered_map<DATA_TYPE, COUNT_TYPE> HashMap;

    CMHeap(uint32_t _MEMORY, std::string _name = "CMHeap"){
        this->name = _name;

        uint32_t LIGHT_MEMORY = _MEMORY * LIGHT_RATIO;
        uint32_t HEAVY_MEMORY = _MEMORY * HEAVY_RATIO;

        sketch = new CMSketch<DATA_TYPE, COUNT_TYPE>(LIGHT_MEMORY);
        heap = new Heap<DATA_TYPE, COUNT_TYPE>(heap->Memory2Size(HEAVY_MEMORY));
    }

    ~CMHeap(){
        delete sketch;
        delete heap;
    }

    void Insert(const DATA_TYPE& item) {
        sketch->Insert(item);
        COUNT_TYPE est = sketch->Query(item);
        heap->Insert(item, est);
    }

    COUNT_TYPE Query(const DATA_TYPE& item){
        COUNT_TYPE ret = heap->Query(item);
        if(ret != 0)
            return ret;
        return sketch->Query(item);
    }

    HashMap AllQuery(){
        return heap->AllQuery();
    }

private:

    const double HEAVY_RATIO = 0.25;
    const double LIGHT_RATIO = 0.75;

    CMSketch<DATA_TYPE, COUNT_TYPE>* sketch;
    Heap<DATA_TYPE, COUNT_TYPE>* heap;
};

#endif
