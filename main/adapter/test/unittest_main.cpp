#include <exception>
#include <gtest/gtest.h>

GTEST_API_ int main(int argc, char** argv) {
    int retval = 1;
    try {
        testing::InitGoogleTest(&argc, argv);
        retval = RUN_ALL_TESTS();
    } catch (std::exception& e) {
        std::cerr << "Runtime exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
    }
    return retval;
}
