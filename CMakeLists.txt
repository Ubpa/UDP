cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(UDP VERSION 0.5.0)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
Ubpa_InitUCMake()

Ubpa_InitProject()

Ubpa_AddDep(UContainer 0.0.5)
Ubpa_AddDep(UTemplate 0.4.5)

Ubpa_AddSubDirsRec(include)
Ubpa_AddSubDirsRec(src)

Ubpa_Export(DIRECTORIES "include")
