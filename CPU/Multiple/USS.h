#ifndef USS_H
#define USS_H

#include "MultiAbstract.h"
#include "StreamSummary.h"

template<typename DATA_TYPE>
class USS : public MultiAbstract<DATA_TYPE>{
public:

    typedef std::unordered_map<DATA_TYPE, COUNT_TYPE> HashMap;

    USS(uint32_t _MEMORY, std::string _name = "USS"){
        this->name = _name;

        summary = new StreamSummary<DATA_TYPE, COUNT_TYPE>(summary->Memory2Size(_MEMORY));
    }

    ~USS(){
        delete summary;
    }

    void Insert(const DATA_TYPE& item){
        if(summary->mp->Lookup(item))
            summary->Add_Data(item);
        else{
            if(summary->isFull()){
                if(randomGenerator() % (summary->getMin() + 1) == 0)
                    summary->SS_Replace(item);
                else
                    summary->Add_Min();
            }
            else
                summary->New_Data(item);
        }
    }

    HashMap AllQuery(){
        return summary->AllQuery();
    }

private:
    StreamSummary<DATA_TYPE, COUNT_TYPE>* summary;
};

#endif
