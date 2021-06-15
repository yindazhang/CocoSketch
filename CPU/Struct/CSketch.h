#ifndef CSKETCH_H
#define CSKETCH_H

#include "Util.h"

template<typename DATA_TYPE,typename COUNT_TYPE>
class CSketch{
public:

    CSketch(uint32_t _MEMORY){
        LENGTH = _MEMORY / sizeof(COUNT_TYPE) / HASH_NUM;

        sketch = new COUNT_TYPE* [HASH_NUM];
        for(uint32_t i = 0;i < HASH_NUM; ++i){
            sketch[i] = new COUNT_TYPE[LENGTH];
            memset(sketch[i], 0, sizeof(COUNT_TYPE) * LENGTH);
        }
    }

    ~CSketch(){
        for(uint32_t i = 0;i < HASH_NUM;++i)
            delete [] sketch[i];
        delete [] sketch;
    }

    void Insert(const DATA_TYPE item) {
        for(uint32_t i = 0; i < HASH_NUM; ++i) {
            uint32_t position = hash(item, i) % LENGTH;
            uint32_t polar = hash(item, i + HASH_NUM) & 1;

            sketch[i][position] += delta[polar];
        }
    }

    COUNT_TYPE Query(const DATA_TYPE item){
        std::vector<COUNT_TYPE> result(HASH_NUM);

        for(uint32_t i = 0; i < HASH_NUM; ++i) {
            uint32_t position = hash(item, i) % LENGTH;
            uint32_t polar = hash(item, i + HASH_NUM) & 1;

            result[i] = sketch[i][position] * delta[polar];
        }

        return Median(result, HASH_NUM);
    }

private:
    const int32_t delta[2] = {+1, -1};

    uint32_t LENGTH;
    const uint32_t HASH_NUM = 3;

    COUNT_TYPE** sketch;
};

#endif
