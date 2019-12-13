cmake_minimum_required( VERSION 3.13 )

project( Vulkan )

find_package( Vulkan REQUIRED )
message( STATUS "${Vulkan_INCLUDE_DIRS}" )
message( STATUS "${Vulkan_LIBRARIES}" )

# TODO: FindGLFW
set( GLFW_LIBRARIES -lglfw )

file( GLOB ${PROJECT_NAME}_SOURCES *.hpp *.cpp )

add_executable( ${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} )
target_include_directories( ${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Vulkan_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} ${Vulkan_LIBRARIES} ${GLFW_LIBRARIES} )
set_target_properties( ${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 )

