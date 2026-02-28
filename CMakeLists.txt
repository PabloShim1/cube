#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.16...3.21)
project(cube LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#----------------------------------------------------------------------------
# Find Geant4 package
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and project includes
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers
# GLOB автоматически подхватит твои новые Messenger.cc и Messenger.hh
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable
# Основной файл cube.cc + все найденные исходники
add_executable(cube cube.cc ${sources} ${headers})
target_link_libraries(cube ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
# Copy macro files to the build directory
# Добавляем только те три файла, которые ты просил оставить
set(CUBE_SCRIPTS
  init_vis.mac
  run.mac
  vis.mac
)

foreach(_script ${CUBE_SCRIPTS})
  if(EXISTS ${PROJECT_SOURCE_DIR}/${_script})
    configure_file(
      ${PROJECT_SOURCE_DIR}/${_script}
      ${PROJECT_BINARY_DIR}/${_script}
      COPYONLY
    )
  endif()
endforeach()