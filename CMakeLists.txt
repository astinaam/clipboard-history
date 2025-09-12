cmake_minimum_required(VERSION 3.16)
project(clipboard-history VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Gui
    Test
)

# Enable automatic MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

# Include directories
include_directories(src)

# Platform-specific libraries for global hotkey
if(UNIX AND NOT APPLE)
    # Linux X11/Wayland support
    find_package(X11)
    if(X11_FOUND)
        set(PLATFORM_LIBRARIES ${X11_LIBRARIES})
        set(PLATFORM_INCLUDES ${X11_INCLUDE_DIR})
        set(PLATFORM_DEFINITIONS HAVE_X11)
    endif()
endif()

# Function to collect source files
function(collect_sources directory sources_var headers_var)
    file(GLOB_RECURSE ${sources_var} "${directory}/*.cpp")
    file(GLOB_RECURSE ${headers_var} "${directory}/*.h")
    set(${sources_var} ${${sources_var}} PARENT_SCOPE)
    set(${headers_var} ${${headers_var}} PARENT_SCOPE)
endfunction()

# Collect all source files
collect_sources(src SOURCES HEADERS)

# Separate library sources from main.cpp
set(LIB_SOURCES ${SOURCES})
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Only create main executable if main.cpp exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    add_executable(clipboard-manager src/main.cpp ${LIB_SOURCES} ${HEADERS})
    target_link_libraries(clipboard-manager Qt6::Core Qt6::Widgets Qt6::Gui)
    
    if(X11_FOUND)
        target_link_libraries(clipboard-manager ${PLATFORM_LIBRARIES})
        target_include_directories(clipboard-manager PRIVATE ${PLATFORM_INCLUDES})
        target_compile_definitions(clipboard-manager PRIVATE ${PLATFORM_DEFINITIONS})
    endif()
endif()

# Enable testing
enable_testing()

# Function to create test executables
function(create_test_executable test_file)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test_file}")
        get_filename_component(test_name ${test_file} NAME_WE)
        add_executable(${test_name} ${test_file} ${LIB_SOURCES})
        target_link_libraries(${test_name} Qt6::Test Qt6::Core Qt6::Widgets Qt6::Gui)
        
        if(X11_FOUND)
            target_link_libraries(${test_name} ${PLATFORM_LIBRARIES})
            target_include_directories(${test_name} PRIVATE ${PLATFORM_INCLUDES})
            target_compile_definitions(${test_name} PRIVATE ${PLATFORM_DEFINITIONS})
        endif()
        
        add_test(NAME ${test_name} COMMAND ${test_name})
    endif()
endfunction()

# Test files to create when they exist
set(POTENTIAL_TEST_FILES
    tests/contract/test_clipboard_manager.cpp
    tests/contract/test_clipboard_window.cpp
    tests/contract/test_tray_icon.cpp
    tests/contract/test_argument_parser.cpp
    tests/contract/test_global_hotkey.cpp
    tests/integration/test_clipboard_monitoring.cpp
    tests/integration/test_global_hotkey.cpp
    tests/integration/test_item_pinning.cpp
    tests/integration/test_system_tray.cpp
    tests/integration/test_persistence.cpp
    tests/unit/test_clipboard_item.cpp
    tests/performance/test_performance.cpp
)

# Create test executables for existing files
foreach(test_file ${POTENTIAL_TEST_FILES})
    create_test_executable(${test_file})
endforeach()

# Installation (only if main executable exists)
if(TARGET clipboard-manager)
    install(TARGETS clipboard-manager DESTINATION bin)
    
    # Create desktop entry
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/clipboard-manager.desktop.in"
        "${CMAKE_CURRENT_BINARY_DIR}/clipboard-manager.desktop"
        @ONLY
    )
    
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/clipboard-manager.desktop"
        DESTINATION share/applications
    )
endif()

# Print status
message(STATUS "Found source files: ${SOURCES}")
message(STATUS "Found header files: ${HEADERS}")
if(X11_FOUND)
    message(STATUS "X11 support enabled")
endif()
