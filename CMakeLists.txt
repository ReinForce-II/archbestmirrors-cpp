cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE RELEASE)
project(archbestmirrors)
aux_source_directory(. SRC_LIST)
find_package(OpenMP REQUIRED)
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

include_directories("${CURL_INCLUDE_DIRS}")

add_executable(archbestmirrors ${SRC_LIST})

target_link_libraries(archbestmirrors 
    ${CURL_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)
set_target_properties(archbestmirrors PROPERTIES LINKER_LANGUAGE CXX)
