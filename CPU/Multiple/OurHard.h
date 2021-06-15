#ifndef CMOURS_H
#define CMOURS_H

#include "MultiAbstract.h"

template<typename DATA_TYPE>
class OurHard : public MultiAbstract<DATA_TYPE>{
public:

    typedef typename MultiAbstract<DATA_TYPE>::Counter Counter;
    typedef std::unordered_map<DATA_TYPE, COUNT_TYPE> HashMap;

    OurHard(uint32_t _MEMORY, uint32_t _HASH_NUM = 1, std::string _name = "OurHard"){
        this->name = _name;

        HASH_NUM = _HASH_NUM;
        LENGTH = _MEMORY / HASH_NUM / sizeof(Counter);

        counter = new Counter*[HASH_NUM];
        for(uint32_t i = 0;i < HASH_NUM;++i){
            counter[i] = new Counter[LENGTH];
            memset(counter[i], 0, sizeof(Counter) * LENGTH);
        }
    }

    ~OurHard(){
        for(uint32_t i = 0;i < HASH_NUM;++i)
            delete [] counter[i];
        delete [] counter;
    }

    void Insert(const DATA_TYPE& item){
        for(uint32_t i = 0;i < HASH_NUM;++i){
            uint32_t position = hash(item, i) % LENGTH;
            counter[i][position].count += 1;
            if(randomGenerator() % counter[i][position].count == 0){
                counter[i][position].ID = item;
            }
        }
    }

    HashMap AllQuery(){
        HashMap ret;

        for(uint32_t i = 0;i < HASH_NUM;++i){
            for(uint32_t j = 0;j < LENGTH;++j){
                if(ret.find(counter[i][j].ID) == ret.end()){
                    std::vector<COUNT_TYPE> estVec(HASH_NUM);
                    for(uint32_t k = 0;k < HASH_NUM;++k){
                        estVec[k] = counter[k][hash(counter[i][j].ID, k) % LENGTH].count;
                    }
                    ret[counter[i][j].ID] = Median(estVec, HASH_NUM);
                }
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
