
cmake_minimum_required(VERSION 3.0)


include("gate/cmake/HunterGate.cmake")
HunterGate(
        URL "https://github.com/ruslo/hunter/archive/v0.16.24.tar.gz"
        SHA1 "00ab627d3d4fe5151e15b6ab2b1b1da17650984a"
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)


project(BaseApp)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Async++                                                         │
# └──────────────────────────────────────────────────────────────────┘
hunter_add_package(Async++)
find_package(Async++ CONFIG REQUIRED)


# ┌──────────────────────────────────────────────────────────────────┐
# │  Poco                                                            │
# └──────────────────────────────────────────────────────────────────┘
hunter_add_package(PocoCpp)
find_package(Poco REQUIRED Foundation Util XML Net Zip  CONFIG)


# ┌──────────────────────────────────────────────────────────────────┐
# │  Qt                                                              │
# └──────────────────────────────────────────────────────────────────┘
# CMAKE_PREFIX_PATH
set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
#hunter_add_package(Qt)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
# Generate code from ui files
#qt5_wrap_ui(UI_HEADERS mainwindow.ui)
# Generate rules for building source files from the resources
#qt5_add_resources(QRCS resources.qrc)

# ┌──────────────────────────────────────────────────────────────────┐
# │  main                                                            │
# └──────────────────────────────────────────────────────────────────┘
set( headers
        QtApplication.h
        QtScheduler.h
        AppWidget.h)

set( srcs main.cpp
        QtApplication.cpp
        AppWidget.cpp
        )

add_executable(baseapp ${srcs})
target_link_libraries(baseapp
        Async++::Async++
        Poco::Foundation Poco::Util Poco::XML Poco::Net
        Qt5::Core Qt5::Gui Qt5::Widgets
        )
