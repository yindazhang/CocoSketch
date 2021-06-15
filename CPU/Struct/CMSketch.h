#ifndef CMSKETCH_H
#define CMSKETCH_H

#include "Util.h"

template<typename DATA_TYPE,typename COUNT_TYPE>
class CMSketch{
public:

    CMSketch(uint32_t _MEMORY){
        LENGTH = _MEMORY / sizeof(COUNT_TYPE) / HASH_NUM;

        sketch = new COUNT_TYPE* [HASH_NUM];
        for(uint32_t i = 0;i < HASH_NUM; ++i){
            sketch[i] = new COUNT_TYPE[LENGTH];
            memset(sketch[i], 0, sizeof(COUNT_TYPE) * LENGTH);
        }
    }

    ~CMSketch(){
        for(uint32_t i = 0;i < HASH_NUM;++i)
            delete [] sketch[i];
        delete [] sketch;
    }

    void Insert(const DATA_TYPE item) {
        for(uint32_t i = 0; i < HASH_NUM; ++i) {
            uint32_t position = hash(item, i) % LENGTH;
            sketch[i][position] += 1;
        }
    }

    COUNT_TYPE Query(const DATA_TYPE item){
        COUNT_TYPE ret = 0x7fffffff;

        for(uint32_t i = 0; i < HASH_NUM; ++i) {
            uint32_t position = hash(item, i) % LENGTH;
            ret = MIN(ret, sketch[i][position]);
        }

        return ret;
    }

private:
    uint32_t LENGTH;
    const uint32_t HASH_NUM = 3;

    COUNT_TYPE** sketch;
};

#endif
