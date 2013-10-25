CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(examples C)

INCLUDE_DIRECTORIES(../src ${CMAKE_CURRENT_BINARY_DIR}/../src) # second one is for generated config.h

ADD_EXECUTABLE(ping ping.c)
TARGET_LINK_LIBRARIES(ping dynamixel)


ADD_EXECUTABLE(search search.c)
TARGET_LINK_LIBRARIES(search dynamixel)

ADD_EXECUTABLE(read_data read_data.c)
TARGET_LINK_LIBRARIES(read_data dynamixel)

ADD_EXECUTABLE(read_write_data read_write_data.c)
TARGET_LINK_LIBRARIES(read_write_data dynamixel)


ADD_EXECUTABLE(read_write_data2 read_write_data2.c)
TARGET_LINK_LIBRARIES(read_write_data2 dynamixel)

ADD_EXECUTABLE(write_sync write_sync.c)
TARGET_LINK_LIBRARIES(write_sync dynamixel)

ADD_EXECUTABLE(write_sync_words write_sync_words.c)
TARGET_LINK_LIBRARIES(write_sync_words dynamixel)

