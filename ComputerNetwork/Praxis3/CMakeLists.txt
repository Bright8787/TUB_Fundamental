cmake_minimum_required(VERSION 3.5)
project(RN-Praxis)

# Set C standard
set(CMAKE_C_STANDARD 11)
find_library(ZeroMQ zmq REQUIRED)
# Add the zmq_distributor executable
add_executable(zmq_distributor zmq_distributor.c)
target_compile_options(zmq_distributor PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(zmq_distributor PRIVATE m zmq pthread)

# Add the zmq_worker executable
add_executable(zmq_worker zmq_worker.c)
target_compile_options(zmq_worker PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(zmq_worker PRIVATE m zmq pthread)



# Packaging
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES ${CMAKE_BINARY_DIR} /\\..*$)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)