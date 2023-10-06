#pragma once
#include <iostream>




// empty function to print newlines
inline void print() {
    std::cout << std::endl;
}

// base case function to end the recursion
inline void print(auto one, bool last = true) {
    std::cout << one;
    if (last) {
        std::cout << std::endl;
    }
}

// recursive variadic template function
inline void print(auto first, auto... args) {
    print(first, false);  // call the appropriate print function

    if constexpr (sizeof...(args) > 0) {
        std::cout << ' ';  // print a space only if there are more arguments
        print(args...);    // call print with remaining arguments
    } else {
        std::cout << std::endl;
    }
}
