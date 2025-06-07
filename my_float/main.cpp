#include <iostream>
#include "my_float.hpp"

int main() {
    std::cout << "Hello from main\n";

    my_float<200, 55> v1(1.23123456712345673456345645), v2(3.146345456456334563456), v3;
        std::cout << "Hello from main 2\n";

    std::cout << "v1 = " << static_cast<double>(v1) << "\n";
    std::cout << "v2 = " << static_cast<double>(v2) << "\n";

    v3 = v1 + v2;

    std::cout << "v3 = v1 + v2 = " << static_cast<double>(v3) << "\n";

    v1 *= v3;

    std::cout << "v1 *= v3 = " << static_cast<double>(v1) << "\n";

    return 0;
}