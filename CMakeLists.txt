project (Cox_Ross)

cmake_minimum_required(VERSION 2.6)

file(TO_CMAKE_PATH "$ENV{IMAGINEPP_ROOT}" d)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${d}/CMake")
find_package(Imagine)

ImagineAddExecutable(Cox_Ross main.cpp CR.h Binom.h)
# ImagineAddExecutable(zipimage main.cpp ../quadtree/quadtree.h)

ImagineUseModules(Cox_Ross Graphics)
# ImagineUseModules(zipimage Graphics)
