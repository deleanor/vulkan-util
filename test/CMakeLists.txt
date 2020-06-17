
add_executable( test_instance test_instance.cpp )
target_include_directories( test_instance PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src )
target_link_libraries( test_instance vkl )
