cmake_minimum_required(VERSION 3.10)

# Set the project name
project(LAB04)

# Set C standard (adjust version if needed)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(includes)

# Source files
set(SOURCES
    src/main.c
    src/message.c
    src/message_queue.c
    src/producer.c
    src/consumer.c
    src/semaphore_utils.c
)

# Create the executable
add_executable(lab04 ${SOURCES})

# Optional: if you need any libraries, link them here
# target_link_libraries(lab04 pthread)