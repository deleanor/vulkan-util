#include <iostream>

#include <vkl/Instance.hpp>

int main( int argc, char * argv[] ) {

    vkl::Instance instance;

    for ( auto&& instance : instance.physicalDevices() ) {
        std::cout << instance;
    }

    return 0;
}
