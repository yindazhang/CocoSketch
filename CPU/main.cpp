#include "BenchMark.h"

int main(int argc, char *argv[]) {
    for(uint32_t i = 1;i < argc;++i) {
        std::cout << argv[i] << std::endl;
        BenchMark dataset(argv[i], "Dataset");
        dataset.HHMultiBench(500000, 0.0001);
    }
    return 0;
}