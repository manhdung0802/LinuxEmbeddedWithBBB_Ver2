#include "engine.h"

namespace internal {
    void inject_fuel(int amount) {
        std::cout << "  -> [Engine-PRIVATE]: Dang phun " << amount << "ml nhien lieu vao buong dot...\n";
    }

    void ignite_spark_plug() {
        std::cout << "  -> [Engine-PRIVATE]: Bugi danh lua! Piston sinh cong!\n";
    }
}
