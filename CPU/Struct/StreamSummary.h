#ifndef STREAMSUMMARY_H
#define STREAMSUMMARY_H

#include "CuckooMap.h"

template<typename ID_TYPE>
class Node{
public:
    ID_TYPE ID;
    Node* prev;
    Node* next;

    Node(ID_TYPE _ID): ID(_ID), prev(nullptr), next(nullptr){}

    void Delete(){
        Connect(prev, next);
    }

    void Connect(Node* prev, Node* next){
        if(prev)
            prev->next = next;
        if(next)
            next->prev = prev;
    }
};

template<typename DATA_TYPE, typename COUNT_TYPE>
class StreamSummary{
public:
    typedef std::unordered_map<DATA_TYPE, COUNT_TYPE> HashMap;

    class DataNode;
    class CountNode;

    class DataNode : public Node<DATA_TYPE>{
    public:
        CountNode* pCount;
        DataNode(DATA_TYPE _ID):Node<DATA_TYPE>(_ID),pCount(nullptr){}
    };

    class CountNode : public Node<COUNT_TYPE>{
    public:
        DataNode* pData;
        CountNode(COUNT_TYPE _ID = 0):Node<COUNT_TYPE>(_ID),pData(nullptr){}
    };

    typedef CuckooMap<DATA_TYPE, DataNode*> Cuckoo;

    StreamSummary(uint32_t _SIZE){
	    SIZE = _SIZE;
        mp = new Cuckoo(SIZE);
        min = new CountNode();
    }

    ~StreamSummary(){
        delete mp;
        CountNode* pCount = min;
        while(pCount){

            DataNode* pData = pCount->pData;
            while(pData){
                DataNode* nextData = (DataNode*)pData->next;
                delete pData;
                pData = nextData;
            }

            CountNode* nextCount = (CountNode*)pCount->next;
            delete pCount;
            pCount = nextCount;
        }
    }

    static uint32_t Size2Memory(uint32_t size){
        return size * ((sizeof(DATA_TYPE) + sizeof(DataNode*)) / LOAD
                       + sizeof(DATA_TYPE) + sizeof(COUNT_TYPE) + 4 * sizeof(void*));
    }

    static uint32_t Memory2Size(uint32_t memory){
        return memory / ((sizeof(DATA_TYPE) + sizeof(DataNode*)) / LOAD
                         + sizeof(DATA_TYPE) + sizeof(COUNT_TYPE) + 4 * sizeof(void*));
    }

    uint32_t SIZE;
    Cuckoo* mp;
    CountNode* min;


    inline COUNT_TYPE getMin() {
        return min->next->ID;
    }

    COUNT_TYPE Query(const DATA_TYPE& item){
        return mp->Lookup(item)?  (*mp)[item]->pCount->ID : 0;
    }

    inline bool isFull(){
        return mp->size() >= SIZE;
    }

    HashMap AllQuery(){
        HashMap ret;
        CountNode* pCount = min;
        while(pCount){
            DataNode* pData = pCount->pData;
            while(pData){
                ret[pData->ID] = pCount->ID;
                pData = (DataNode*)pData->next;
            }
            pCount = (CountNode*)pCount->next;
        }
        return ret;
    }

    inline void New_Data(const DATA_TYPE& data){
        DataNode* pData = new DataNode(data);
        Add_Count(min, pData);
        mp->Insert(data, pData);
    }

    inline void Add_Min(){
        Add_Data(((CountNode*)min->next)->pData->ID);
    }

    void Add_Data(const DATA_TYPE& data){
        DataNode* pData = (*mp)[data];
        CountNode* pCount = pData->pCount;

        bool del = false;
        pData->Delete();
        if(pCount->pData == pData){
            pCount->pData = (DataNode*)pData->next;
            del = !pData->next;
        }

        Add_Count(pCount, pData);

        if(del){
            pCount->Delete();
            delete pCount;
        }
    }

    void Add_Count(CountNode* pCount, DataNode* pData){
        if(!pCount->next)
            pCount->Connect(pCount, new CountNode(pCount->ID + 1));
        else if(pCount->next->ID - pCount->ID > 1){
            CountNode* add = new CountNode(pCount->ID + 1);
            pCount->Connect(add, pCount->next);
            pCount->Connect(pCount, add);
        }

        pData->prev = nullptr;
        pData->pCount = (CountNode*)pCount->next;
        pData->Connect(pData, pData->pCount->pData);
        pData->pCount->pData = pData;
    }

    void SS_Replace(const DATA_TYPE& data){
        CountNode* pCount = (CountNode*)min->next;
        DataNode* pData = new DataNode(data);

        mp->Insert(data, pData);
        Add_Count(pCount, pData);
        pData = pCount->pData;
        pCount->pData = (DataNode*)pData->next;
        pData->Delete();
        if(!pData->next){
            pCount->Delete();
            delete pCount;
        }
        mp->Delete(pData->ID);
        delete pData;
    }
};

#endif
