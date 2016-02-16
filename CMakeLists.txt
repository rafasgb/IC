cmake_minimum_required(VERSION 3.3)
project(Watershed)

set(Qt5Core_DIR /Users/zulli/Qt/5.5/clang_64/lib/cmake/Qt5Core)
set(Qt5OpenGL_DIR /Users/zulli/Qt/5.5/clang_64/lib/cmake/Qt5OpenGL)
set(Qt5Widgets_DIR /Users/zulli/Qt/5.5/clang_64/lib/cmake/Qt5Widgets)
set(Qt5Sql_DIR /Users/zulli/Qt/5.5/clang_64/lib/cmake/Qt5Sql)


FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5OpenGL REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Sql REQUIRED)
#FIND_PACKAGE(Qt5WebKitWidgets REQUIRED)
FIND_PACKAGE(VTK REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE(${VTK_USE_FILE})

link_directories(/Users/zulli/Documents/IC/PhD-Thesis-build/lib)
INCLUDE_DIRECTORIES(/Users/zulli/Documents/IC/PhD-Thesis/Sources)

find_package( OpenCV REQUIRED )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(INCLUDE_FILES WatershedImage.cpp WatershedImage.h SegmentedImage.cpp SegmentedImage.h SegmentationEvaluator.cpp SegmentationEvaluator.h FireflyOptimizator.cpp FireflyOptimizator.h )

add_executable(Watershed main.cpp ${INCLUDE_FILES})
add_executable(Interpolation1 Tests/Interpolation1/main.cpp ${INCLUDE_FILES} )
add_executable(Interpolation2 Tests/Interpolation2/main.cpp ${INCLUDE_FILES})
TARGET_LINK_LIBRARIES(Watershed FeatureExtractors ComplexNetwork Utilities Qt5::Core Qt5::Widgets Qt5::OpenGL  ${OpenCV_LIBS}  ${VTK_LIBRARIES})
TARGET_LINK_LIBRARIES(Interpolation1 FeatureExtractors ComplexNetwork Utilities Qt5::Core Qt5::Widgets Qt5::OpenGL  ${OpenCV_LIBS}  ${VTK_LIBRARIES})
TARGET_LINK_LIBRARIES(Interpolation2 FeatureExtractors ComplexNetwork Utilities Qt5::Core Qt5::Widgets Qt5::OpenGL  ${OpenCV_LIBS}  ${VTK_LIBRARIES})