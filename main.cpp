
#include <MainApplication.hpp>

#include <iostream>
#include <stdexcept>

int main( int argc, char * argv[] )
{
    MainApplication app;
    try {
        app.run();
        return EXIT_SUCCESS;
    } catch ( const std::exception& exception ) {
        std::cerr << exception.what();
        return EXIT_FAILURE;
    }
}
