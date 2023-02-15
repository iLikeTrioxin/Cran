project(Cranel)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(deps)

find_package(libfreenect REQUIRED)
find_package(OpenCV REQUIRED)

File(GLOB_RECURSE SOURCE_FILES src/*.cpp)
File(GLOB_RECURSE HEADER_FILES src/*.h  )

add_library(Cranel ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(Cranel PUBLIC ${OpenCV_LIBS} freenectstatic)
target_include_directories(Cranel PUBLIC ${OpenCV_INCLUDE_DIRS} ${libfreenect_INCLUDE_DIRS} src)