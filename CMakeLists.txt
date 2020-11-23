cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(UDP VERSION 0.7.4)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
Ubpa_InitUCMake()

Ubpa_InitProject()

Ubpa_AddDep(UContainer 0.0.9)
Ubpa_AddDep(UTemplate 0.4.9)
Ubpa_AddDep(USTL 0.2.0)

Ubpa_AddSubDirsRec(include)
Ubpa_AddSubDirsRec(src)

Ubpa_Export(
  TARGET
  DIRECTORIES
    "include"
)
