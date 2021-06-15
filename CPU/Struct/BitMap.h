#ifndef BITMAP_H
#define BITMAP_H

struct BitMap{
    uint8_t* bitset;

    BitMap(uint32_t length){
        uint32_t size = ((length + 7) >> 3);
        bitset = new uint8_t[size];
        memset(bitset, 0, size * sizeof(uint8_t));
    }

    ~BitMap(){
        delete [] bitset;
    }

    inline void Set(uint32_t index){
        uint32_t position = (index >> 3);
        uint32_t offset = (index & 0x7);
        bitset[position] |= (1 << offset);
    }

    inline bool Get(uint32_t index){
        uint32_t position = (index >> 3);
        uint32_t offset = (index & 0x7);
        return (bitset[position] & (1 << offset));
    }

    inline void Clear(uint32_t index){
        uint32_t position = (index >> 3);
        uint32_t offset = (index & 0x7);
        bitset[position] &= (~(1 << offset));
    }
};

#endif
