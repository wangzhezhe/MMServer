cmake_minimum_required (VERSION 3.14)
project (MMserver C CXX)
set (CMAKE_CXX_STANDARD 14)
# it is ok to build with debug pattern for drc, not sure why
set (CMAKE_BUILD_TYPE Debug)


# add our cmake module directory to the path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# link shared lib with full rpath
set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# setup cache variables for ccmake
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Release
         CACHE STRING "Choose the type of build." FORCE)
    set_property (CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS 
                  "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif ()
set (CMAKE_PREFIX_PATH "" CACHE STRING "External dependencies path")
set (BUILD_SHARED_LIBS "OFF" CACHE BOOL "Build a shared library")

# packages we depend on (load all the necessary depedency by spack)
include (xpkg-import)
find_package (mercury REQUIRED)
include_directories(${MERCURY_INCLUDE_DIR})
xpkg_import_module (argobots REQUIRED argobots)
xpkg_import_module (margo REQUIRED margo)
xpkg_import_module (abtio REQUIRED abt-io)
find_package (thallium REQUIRED)

option(USE_GNI "use cray gni" OFF)
set (DRC_LIBRARIES " ")



# Build client
add_library(metaclient ./metaclient.cpp)
target_link_libraries(metaclient thallium)

if(USE_GNI)
  add_definitions(-DUSE_GNI)
  find_package(DRC REQUIRED)
  include_directories(${DRC_INCLUDE_DIRS})
  message ("use cray DRC")
  message ("include drc" ${DRC_INCLUDE_DIRS})
  message ("link drc lib" ${DRC_LIBRARIES})

  # Building server
  add_executable(metaserver ./metaserver.cpp ./utils/ipTool.cpp)
  target_link_libraries(metaserver thallium ${DRC_LIBRARIES})

  # Building testclient
  # put the drc libraries at the first position
  add_executable(testclient ./test/testclient.cpp)
  target_link_libraries(testclient ${DRC_LIBRARIES} metaclient thallium)

  add_executable(testhelloserver ./test/testhelloserver.cpp ./utils/ipTool.cpp)
  target_link_libraries(testhelloserver thallium ${DRC_LIBRARIES})

  add_executable(testhelloclient ./test/testhelloclient.cpp)
  target_link_libraries(testhelloclient ${DRC_LIBRARIES} metaclient thallium)

else()

  # Building server
  add_executable(metaserver ./metaserver.cpp ./utils/ipTool.cpp)
  target_link_libraries(metaserver thallium)

  # Building test
  # put the drc libraries at the first position
  add_executable(testclient ./test/testclient.cpp)
  target_link_libraries(testclient metaclient thallium)

  add_executable(testhelloserver ./test/testhelloserver.cpp ./utils/ipTool.cpp)
  target_link_libraries(testhelloserver thallium )

  add_executable(testhelloclient ./test/testhelloclient.cpp)
  target_link_libraries(testhelloclient metaclient thallium)


endif()
