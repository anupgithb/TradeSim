cmake_minimum_required(VERSION 3.20)
project(TradeSimulator VERSION 1.0 LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg integration (if used)
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

# Find packages
find_package(Boost REQUIRED COMPONENTS system)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Source files
file(GLOB_RECURSE SRC_FILES
    "src/*.cpp"
    "core/*.cpp"
    "network/*.cpp"
    "models/*.cpp"
    "utils/*.cpp"
    "ui/*.cpp"
)

# UI forms and resource handling
qt6_wrap_ui(UI_HEADERS ui/MainWindow.ui)
qt6_add_resources(RES_SOURCES ui/resources.qrc)

# Executable
add_executable(TradeSimulator
    ${SRC_FILES}
    ${UI_HEADERS}
    ${RES_SOURCES}
)

target_link_libraries(TradeSimulator PRIVATE
    Boost::system
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Output directory
set_target_properties(TradeSimulator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
