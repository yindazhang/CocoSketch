#ifndef CUCKOOMAP_H
#define CUCKOOMAP_H

#include "Util.h"
#include "BitMap.h"

#define LOAD 0.5

#define CUCKOOSEED 201

#define MAX_KICK 50
#define ARRAY_NUM 2
#define SLOT_PER_BUCKET 4


template<typename KEY_TYPE, typename VALUE_TYPE>
class CuckooMap{
public:
    struct Bucket{
        KEY_TYPE keys[SLOT_PER_BUCKET];
        VALUE_TYPE values[SLOT_PER_BUCKET];
    };

    CuckooMap(uint32_t SIZE){
        inserted = 0;
        length = SIZE / LOAD / ARRAY_NUM / SLOT_PER_BUCKET + 1;

        for(uint32_t i = 0;i < ARRAY_NUM;++i){
            bitmaps[i] = new BitMap(length * SLOT_PER_BUCKET);
            buckets[i] = new Bucket[length];
            memset(buckets[i], 0, length * sizeof(Bucket));
        }
    }

    ~CuckooMap(){
        for(uint32_t i = 0;i < ARRAY_NUM;++i){
            delete bitmaps[i];
            delete [] buckets[i];
        }
    }

    static uint32_t Size2Memory(uint32_t size){
        return size / LOAD * (sizeof(KEY_TYPE) + sizeof(VALUE_TYPE));
    }

    static uint32_t Memory2Size(uint32_t memory){
        return memory * LOAD / (sizeof(KEY_TYPE) + sizeof(VALUE_TYPE));
    }

    inline uint32_t size(){
        return inserted;
    }

    void Insert(KEY_TYPE key, VALUE_TYPE value){
        inserted += 1;
        uint32_t pos[ARRAY_NUM];

        for(uint32_t i = 0;i < ARRAY_NUM;++i){
            pos[i] = hash(key, i + CUCKOOSEED) % length;
            uint32_t start = pos[i] * SLOT_PER_BUCKET;
            for(uint32_t slot = 0;slot < SLOT_PER_BUCKET;++slot){
                if(!bitmaps[i]->Get(start + slot)){
                    bitmaps[i]->Set(start + slot);
                    buckets[i][pos[i]].keys[slot] = key;
                    buckets[i][pos[i]].values[slot] = value;
                    return;
                }
            }
        }

        uint32_t choice = 0;
        for(uint32_t kick_num = 0;kick_num < MAX_KICK;++kick_num){
            uint32_t slot = rd() % SLOT_PER_BUCKET;

            KEY_TYPE tempKey = buckets[choice][pos[choice]].keys[slot];
            VALUE_TYPE tempValue = buckets[choice][pos[choice]].values[slot];

            buckets[choice][pos[choice]].keys[slot] = key;
            buckets[choice][pos[choice]].values[slot] = value;

            key = tempKey;
            value = tempValue;

            choice = 1 - choice;
            pos[choice] = hash(key, choice + CUCKOOSEED) % length;

            uint32_t start = pos[choice] * SLOT_PER_BUCKET;
            for(slot = 0;slot < SLOT_PER_BUCKET;++slot){
                if(!bitmaps[choice]->Get(start + slot)){
                    bitmaps[choice]->Set(start + slot);
                    buckets[choice][pos[choice]].keys[slot] = key;
                    buckets[choice][pos[choice]].values[slot] = value;
                    return;
                }
            }
        }

        std::cerr << "Hash Insert Error" << std::endl;
        throw;
    }

    void Replace(KEY_TYPE key, VALUE_TYPE value){
        for(uint32_t i = 0;i < ARRAY_NUM;++i){
            uint32_t pos = hash(key, i + CUCKOOSEED) % length;
            uint32_t start = pos * SLOT_PER_BUCKET;
            for(uint32_t slot = 0;slot < SLOT_PER_BUCKET;++slot){
                if(bitmaps[i]->Get(start + slot) &&
                    buckets[i][pos].keys[slot] == key){
                    buckets[i][pos].values[slot] = value;
                    return;
                }
            }
        }
        std::cerr << "Hash Replace Error" << std::endl;
        throw;
    }

    bool Lookup(KEY_TYPE key){
        for(uint32_t i = 0;i < ARRAY_NUM;++i){
            uint32_t pos = hash(key, i + CUCKOOSEED) % length;
            uint32_t start = pos * SLOT_PER_BUCKET;
            for(uint32_t slot = 0;slot < SLOT_PER_BUCKET;++slot){
                if(bitmaps[i]->Get(start + slot) &&
                   buckets[i][pos].keys[slot] == key){
                    return true;
                }
            }
        }
        return false;
    }

    VALUE_TYPE operator [] (KEY_TYPE key){
        for(uint32_t i = 0;i < ARRAY_NUM;++i){
            uint32_t pos = hash(key, i + CUCKOOSEED) % length;
            uint32_t start = pos * SLOT_PER_BUCKET;
            for(uint32_t slot = 0;slot < SLOT_PER_BUCKET;++slot){
                if(bitmaps[i]->Get(start + slot) &&
                   buckets[i][pos].keys[slot] == key){
                    return buckets[i][pos].values[slot];
                }
            }
        }
        std::cerr << "Hash Find Error" << std::endl;
        throw;
    }

    void Delete(KEY_TYPE key){
        inserted -= 1;

        for(uint32_t i = 0;i < ARRAY_NUM;++i){
            uint32_t pos = hash(key, i + CUCKOOSEED) % length;
            uint32_t start = pos * SLOT_PER_BUCKET;
            for(uint32_t slot = 0;slot < SLOT_PER_BUCKET;++slot){
                if(bitmaps[i]->Get(start + slot) &&
                   buckets[i][pos].keys[slot] == key){
                    bitmaps[i]->Clear(start + slot);
                    return;
                }
            }
        }
        std::cerr << "Hash Delete Error" << std::endl;
        throw;
    }

protected:
    uint32_t length;
    uint32_t inserted;

    BitMap* bitmaps[ARRAY_NUM];
    Bucket* buckets[ARRAY_NUM];
};


#endif
