#ifndef HHBENCH_H
#define HHBENCH_H

#include <vector>
#include <fstream>

#include "Univmon.h"
#include "Elastic.h"
#include "CMHeap.h"
#include "CountHeap.h"
#include "SpaceSaving.h"

#include "OurHard.h"
#include "OurSoft.h"
#include "USS.h"

#include "MMap.h"


class BenchMark{
public:

    BenchMark(std::string PATH, std::string name){
        fileName = name;

        result = Load(PATH.c_str());
        dataset = (TUPLES*)result.start;
        length = result.length / sizeof(TUPLES);

        for(uint64_t i = 0;i < length;++i){
            tuplesMp[dataset[i]] += 1;

            mp[0][dataset[i].srcIP_dstIP()] += 1;
            mp[1][dataset[i].srcIP_srcPort()] += 1;
            mp[2][dataset[i].dstIP_dstPort()] += 1;
            mp[3][dataset[i].srcIP()] += 1;
            mp[4][dataset[i].dstIP()] += 1;
        }
    }

    ~BenchMark(){
        UnLoad(result);
    }

    void HHSingleBench(uint32_t MEMORY, double alpha){
#define HHOtherSketch CMHeap

        SingleAbstract<TUPLES>* tupleSketch;
        SingleAbstract<uint64_t>* sketch[5];


        for(uint32_t i = 1;i <= MAX_TRAFFIC;++i){
            uint32_t mem = MEMORY / i;

            tupleSketch = new HHOtherSketch<TUPLES>(mem);

            for(uint32_t j = 0;j < i - 1;++j){
                sketch[j] = new HHOtherSketch<uint64_t>(mem);
            }
            for(uint32_t j = 0;j < length;++j){
                switch(i){
                    case 6: sketch[4]->Insert(dataset[j].dstIP());
                    case 5: sketch[3]->Insert(dataset[j].srcIP());
                    case 4: sketch[2]->Insert(dataset[j].dstIP_dstPort());
                    case 3: sketch[1]->Insert(dataset[j].srcIP_srcPort());
                    case 2: sketch[0]->Insert(dataset[j].srcIP_dstIP());
                    default: tupleSketch->Insert(dataset[j]);
                }
            }

            std::unordered_map<TUPLES, COUNT_TYPE> estTuple = tupleSketch->AllQuery();
            std::unordered_map<uint64_t, COUNT_TYPE> estMp[5];

            for(uint32_t j = 0;j < i - 1;++j){
                estMp[j] = sketch[j]->AllQuery();
            }

            COUNT_TYPE threshold = alpha * length;

            CompareHH(estTuple, tuplesMp, threshold, 1);

            for(uint32_t j = 0;j < i - 1;++j){
                CompareHH(estMp[j], mp[j], threshold, j + 2);
            }

            delete tupleSketch;
            for(uint32_t j = 0;j < i - 1;++j){
                delete sketch[j];
            }
        }
    }

    void HHMultiBench(uint32_t MEMORY, double alpha){
        MultiAbstract<TUPLES>* sketch = new OurSoft<TUPLES>(MEMORY);

        for(uint32_t i = 0;i < length;++i){
            sketch->Insert(dataset[i]);
        }

        std::unordered_map<TUPLES, COUNT_TYPE> estTuple = sketch->AllQuery();
        std::unordered_map<uint64_t, COUNT_TYPE> estMp[5];

        for(auto it = estTuple.begin();it != estTuple.end();++it){
            estMp[0][(it->first).srcIP_dstIP()] += it->second;
            estMp[1][(it->first).srcIP_srcPort()] += it->second;
            estMp[2][(it->first).dstIP_dstPort()] += it->second;
            estMp[3][(it->first).srcIP()] += it->second;
            estMp[4][(it->first).dstIP()] += it->second;
        }

        COUNT_TYPE threshold = alpha * length;

        CompareHH(estTuple, tuplesMp, threshold, 1);

        for(uint32_t i = 0;i < 5;++i){
            CompareHH(estMp[i], mp[i], threshold, i + 2);
        }

        delete sketch;
    }

private:
    std::string fileName;

    LoadResult result;

    TUPLES* dataset;
    uint64_t length;

    std::unordered_map<TUPLES, COUNT_TYPE> tuplesMp;
    std::unordered_map<uint64_t, COUNT_TYPE> mp[5];

    template<class T>
    void CompareHH(T mp, T record, COUNT_TYPE threshold, uint32_t key_type){
        double realHH = 0, estHH = 0, bothHH = 0, aae = 0, are = 0;

        for(auto it = record.begin();it != record.end();++it){
            bool real, est;
            double realF = it->second, estF = mp[it->first];

            real = (realF > threshold);
            est = (estF > threshold);

            realHH += real;
            estHH += est;

            if(real && est){
                bothHH += 1;
                aae += abs(realF - estF);
                are += abs(realF - estF) / realF;
            }
        }

        std::cout << "key-type," << key_type << std::endl;
        std::cout << "threshold," << threshold << std::endl;

        std::cout << "Recall," << bothHH / realHH << std::endl;
        std::cout << "Precision," << bothHH / estHH << std::endl;

        std::cout << "aae," << aae / bothHH << std::endl;
        std::cout << "are," << are / bothHH << std::endl;
        std::cout << std::endl;
    }
};

#endif
