
find_package(Boost REQUIRED)
find_package(Vulkan REQUIRED)

add_executable( test_instance test_instance.cpp )
target_include_directories( test_instance PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src )
target_link_libraries( test_instance vkl )

add_executable( test_vulkan_hpp test_vulkan_hpp.cpp )
target_include_directories(test_vulkan_hpp PRIVATE ${BOOST_INCLUDE_DIRS})
target_link_libraries(test_vulkan_hpp PRIVATE vkl)
set_target_properties(test_vulkan_hpp PROPERTIES CXX_STANDARD 20)
