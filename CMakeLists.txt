cmake_minimum_required(VERSION 3.10)
project(radar_ppi)

set(CMAKE_CXX_STANDARD 11)

if(MINGW)

  if ("${GLUT}" STREQUAL "")
  message(FATAL_ERROR "GLUT libs not defined")
  endif()

  if ("${GLEW}" STREQUAL "")
  message(FATAL_ERROR "GLEW libs not defined")
  endif()

  set(CMAKE_PREFIX_PATH "${MINGW_HOME};${GLUT};${GLEW}")
  set(GLUT_INCLUDE_DIR "${GLUT}/include")
endif()

# Find OpenGL, GLEW and GLUT (prefer freeglut)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

if (MINGW)
  include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
endif()

add_executable(PROJECT_NAME src/main.cpp)

target_link_libraries(PROJECT_NAME
  ${OPENGL_LIBRARIES}
  GLUT::GLUT
  GLEW::GLEW
)


if (MINGW)
  # Get the path to the executable's output directory
  set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
  # get_target_property(OUTPUT_DIR radar_ppi RUNTIME_OUTPUT_DIRECTORY)

  # Find the DLLs
  # For freeglut
  find_file(FREEGLUT_DLL libfreeglut.dll HINTS ${GLUT} PATH_SUFFIXES bin)

  # For GLEW
  find_file(GLEW_DLL glew32.dll HINTS ${GLEW} PATH_SUFFIXES bin)

  # For libstdc++-6.dll from MinGW
  find_file(STDCPP_DLL libstdc++-6.dll HINTS ${CMAKE_CXX_COMPILER} PATH_SUFFIXES ../bin)

  # For libgcc_s_seh-1.dll from MinGW
  find_file(GCC_S_SEH_DLL libgcc_s_seh-1.dll HINTS ${CMAKE_CXX_COMPILER} PATH_SUFFIXES ../bin)
  # For libwinpthread-1.dll from MinGW
  find_file(WIN_PTHREAD_DLL libwinpthread-1.dll HINTS ${CMAKE_CXX_COMPILER} PATH_SUFFIXES ../bin)

  # Add a custom command to copy the DLLs after the build
  if(FREEGLUT_DLL AND GLEW_DLL AND STDCPP_DLL AND GCC_S_SEH_DLL AND WIN_PTHREAD_DLL)
    add_custom_command(
      TARGET PROJECT_NAME POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      ${FREEGLUT_DLL}
      ${OUTPUT_DIR}
      COMMENT "Copying freeglut dll"
    )
    add_custom_command(
      TARGET PROJECT_NAME POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      ${GLEW_DLL}
      ${OUTPUT_DIR}
      COMMENT "Copying glew32 dll"
    )
    add_custom_command(
      TARGET PROJECT_NAME POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      ${STDCPP_DLL}
      ${OUTPUT_DIR}
      COMMENT "Copying libstdc++ dll"
    )
    add_custom_command(
      TARGET PROJECT_NAME POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      ${GCC_S_SEH_DLL}
      ${OUTPUT_DIR}
      COMMENT "Copying libwinpthread dll"
    )
    add_custom_command(
      TARGET PROJECT_NAME POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      ${WIN_PTHREAD_DLL}
      ${OUTPUT_DIR}
      COMMENT "Copying libgcc_s_seh dll"
      )
  else()
    message("One of the runtimes dll not found")
  endif()
endif()