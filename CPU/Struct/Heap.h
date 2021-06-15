#ifndef HEAP_H
#define HEAP_H

#include <algorithm>

#include "CuckooMap.h"

template<typename DATA_TYPE, typename COUNT_TYPE>
class Heap{
public:

    typedef std::pair<COUNT_TYPE, DATA_TYPE> KV;
    typedef CuckooMap<DATA_TYPE, uint32_t> Cuckoo;
    typedef std::unordered_map<DATA_TYPE, COUNT_TYPE> HashMap;

    Heap(uint32_t _SIZE){
	    SIZE = _SIZE;
        mp = new Cuckoo(SIZE);
        heap = new KV[SIZE];
        memset(heap, 0, sizeof(KV) * SIZE);
    }

    ~Heap(){
        delete mp;
        delete [] heap;
    }

    static uint32_t Size2Memory(uint32_t size){
        return size * ((sizeof(DATA_TYPE) + sizeof(uint32_t)) / LOAD
        + sizeof(DATA_TYPE) + sizeof(COUNT_TYPE));
    }

    static uint32_t Memory2Size(uint32_t memory){
        return memory / ((sizeof(DATA_TYPE) + sizeof(uint32_t)) / LOAD
                         + sizeof(DATA_TYPE) + sizeof(COUNT_TYPE));
    }

    void Insert(const DATA_TYPE item, const COUNT_TYPE frequency){
        if(mp->Lookup(item))
            this->Add_Data(item);
        else{
            if(this->isFull()){
                if(frequency > heap[0].first){
                    mp->Delete(heap[0].second);
                    heap[0].first = frequency;
                    heap[0].second = item;
                    mp->Insert(item, 0);
                    this->Heap_Down(0);
                }
            }
            else
                this->New_Data(item);
        }
    }

    COUNT_TYPE Query(const DATA_TYPE& item){
        return mp->Lookup(item)?  heap[(*mp)[item]].first : 0;
    }

    HashMap AllQuery(){
        HashMap ret;
        uint32_t size = mp->size();
        for(uint32_t i = 0;i < size;++i){
            ret[heap[i].second] = heap[i].first;
        }
        return ret;
    }

protected:
    uint32_t SIZE;
    Cuckoo* mp;
    KV* heap;

    inline bool isFull(){
        return mp->size() >= SIZE;
    }

    void Add_Data(const DATA_TYPE& data){
        uint32_t pos = (*mp)[data];
        heap[pos].first += 1;
        Heap_Down(pos);
    }

    void New_Data(const DATA_TYPE& data){
        uint32_t pos = mp->size();
        heap[pos].first = 1;
        heap[pos].second = data;
        mp->Insert(data, pos);
        Heap_Up(pos);
    }

    void Heap_Down(uint32_t pos){
        uint32_t upper = mp->size();

        while (pos < upper / 2) {
            uint32_t left = 2 * pos + 1, right = 2 * pos + 2;
            uint32_t replace = pos;

            if (left < upper && heap[left].first < heap[replace].first)
                replace = left;
            if (right < upper && heap[right].first < heap[replace].first)
                replace = right;

            if (replace != pos) {
                KV temp = heap[pos];
                heap[pos] = heap[replace];
                heap[replace] = temp;
                mp->Replace(heap[pos].second, pos);
                mp->Replace(heap[replace].second, replace);
                Heap_Down(replace);
            }
            else break;
        }
    }

    void Heap_Up(uint32_t pos) {
        while (pos > 1) {
            uint32_t parent = (pos - 1) / 2;
            if (heap[parent].first <= heap[pos].first)
                break;

            KV temp = heap[pos];
            heap[pos] = heap[parent];
            heap[parent] = temp;
            mp->Replace(heap[pos].second, pos);
            mp->Replace(heap[parent].second, parent);
            pos = parent;
        }
    }

};

#endif
