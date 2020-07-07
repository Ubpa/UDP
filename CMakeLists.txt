cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(UDP VERSION 0.5.1)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
Ubpa_InitUCMake()

Ubpa_InitProject()

include(cmake/UbpaAutoRefl.cmake)

Ubpa_AddDep(UContainer 0.0.6)
Ubpa_AddDep(UTemplate 0.4.6)
Ubpa_AddDep(UANTLR 0.1.0)

Ubpa_AddSubDirsRec(include)
Ubpa_AddSubDirsRec(src)

Ubpa_Export(DIRECTORIES "include")
