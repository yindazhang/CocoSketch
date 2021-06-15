#ifndef CUOURS_H
#define CUOURS_H

#include "MultiAbstract.h"

template<typename DATA_TYPE>
class OurSoft : public MultiAbstract<DATA_TYPE>{
public:

    typedef typename MultiAbstract<DATA_TYPE>::Counter Counter;
    typedef std::unordered_map<DATA_TYPE, COUNT_TYPE> HashMap;

    OurSoft(uint32_t _MEMORY, uint32_t _HASH_NUM = 2, std::string _name = "OurSoft"){
        this->name = _name;

        HASH_NUM = _HASH_NUM;
        LENGTH = _MEMORY / _HASH_NUM / sizeof(Counter);

        counter = new Counter* [HASH_NUM];
        for(uint32_t i = 0;i < HASH_NUM;++i){
            counter[i] = new Counter [LENGTH];
            memset(counter[i], 0, sizeof(Counter) * LENGTH);
        }
    }

    ~OurSoft(){
        for(uint32_t i = 0;i < HASH_NUM;++i){
            delete [] counter[i];
        }
        delete [] counter;
    }

    void Insert(const DATA_TYPE& item){
        COUNT_TYPE minimum = 0x7fffffff;
        uint32_t minPos, minHash;

        for(uint32_t i = 0;i < HASH_NUM;++i){
            uint32_t position = hash(item, i) % LENGTH;
            if(counter[i][position].ID == item){
                counter[i][position].count += 1;
                return;
            }
            if(counter[i][position].count < minimum){
                minPos = position;
                minHash = i;
                minimum = counter[i][position].count;
            }
        }

        counter[minHash][minPos].count += 1;
        if(randomGenerator() % counter[minHash][minPos].count == 0){
            counter[minHash][minPos].ID = item;
        }
    }

    HashMap AllQuery(){
        HashMap ret;

        for(uint32_t i = 0;i < HASH_NUM;++i){
            for(uint32_t j = 0;j < LENGTH;++j){
                ret[counter[i][j].ID] = counter[i][j].count;
            }
        }

        return ret;
    }

private:
    uint32_t LENGTH;
    uint32_t HASH_NUM;

    Counter** counter;
};

#endif
