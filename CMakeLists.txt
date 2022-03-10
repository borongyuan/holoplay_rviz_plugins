cmake_minimum_required(VERSION 3.5)
project(holoplay_rviz_plugins)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(HOLOPLAY_CORE_BASE_PATH "${PROJECT_SOURCE_DIR}/HoloPlayCoreSDK/HoloPlayCore")

set(DLL_DIR "linux")

if(WIN32)
  set(DLL_DIR "Win64")
  if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(DLL_DIR "Win32")
  endif()
  set(HOLOPLAY_CORE_DLL_LOCATION "${HOLOPLAY_CORE_BASE_PATH}/dylib/${DLL_DIR}/HoloPlayCore.dll")
  add_custom_command(TARGET main POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${HOLOPLAY_CORE_DLL_LOCATION}" $<TARGET_FILE_DIR:main>)
endif(WIN32)

if(APPLE)
  set(DLL_DIR "macos")
endif(APPLE)

# find dependencies
find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

find_package(Qt5 REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)

set(holoplay_rviz_plugins_headers_to_moc
  include/holoplay_rviz_plugins/looking_glass_display.hpp
)

foreach(header "${holoplay_rviz_plugins_headers_to_moc}")
  qt5_wrap_cpp(holoplay_rviz_plugins_moc_files "${header}")
endforeach()

ament_auto_add_library(${PROJECT_NAME} SHARED
  ${holoplay_rviz_plugins_moc_files}
  src/looking_glass_display.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
  "${HOLOPLAY_CORE_BASE_PATH}/include"
)

find_library(HOLOPLAY_CORE_LOCATION HoloPlayCore PATHS "${HOLOPLAY_CORE_BASE_PATH}/dylib" PATH_SUFFIXES ${DLL_DIR})
target_link_libraries(${PROJECT_NAME}
  ${HOLOPLAY_CORE_LOCATION}
)

install(
  FILES ${HOLOPLAY_CORE_LOCATION}
  DESTINATION lib
)

pluginlib_export_plugin_description_file(rviz_common plugins_description.xml)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_auto_package(INSTALL_TO_SHARE icons)
