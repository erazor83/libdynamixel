
include_directories(${CMAKE_CURRENT_LIST_DIR}/../src)


add_executable(ping ping.c)
target_link_libraries(ping dynamixel)


