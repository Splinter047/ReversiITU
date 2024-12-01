cmake_minimum_required(VERSION 3.29)
project(ReversiITU)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(ReversiITU src/main.cpp
)
target_link_libraries(ReversiITU PRIVATE sfml-graphics)
target_compile_features(ReversiITU PRIVATE cxx_std_17)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/board_init.txt
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/board_init.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/saved_board.txt
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/saved_board.txt COPYONLY)

if(WIN32)
    add_custom_command(
            TARGET ReversiITU
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:ReversiITU>
            VERBATIM)
endif()