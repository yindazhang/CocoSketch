#ifndef SS_H
#define SS_H

#include "SingleAbstract.h"
#include "StreamSummary.h"

template<typename DATA_TYPE>
class SpaceSaving : public SingleAbstract<DATA_TYPE>{
public:
    typedef std::unordered_map<DATA_TYPE, COUNT_TYPE> HashMap;

    SpaceSaving(uint32_t _MEMORY, std::string _name = "SpaceSaving"){
        this->name = _name;

        summary = new StreamSummary<DATA_TYPE, COUNT_TYPE>(summary->Memory2Size(_MEMORY));
    }

    ~SpaceSaving(){
        delete summary;
    }

    void Insert(const DATA_TYPE& item){
        if(summary->mp->Lookup(item))
            summary->Add_Data(item);
        else{
            if(summary->isFull())
                summary->SS_Replace(item);
            else
                summary->New_Data(item);
        }
    }

    COUNT_TYPE Query(const DATA_TYPE& item){
        COUNT_TYPE temp = summary->Query(item);
        if(temp > 0)
            return temp;
        else
            return summary->getMin() - 1;
    }

    HashMap AllQuery(){
        return summary->AllQuery();
    }

private:
    StreamSummary<DATA_TYPE, COUNT_TYPE>* summary;
};

#endif
