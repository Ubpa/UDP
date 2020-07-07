Ubpa_AutoRefl(
  "${CMAKE_CURRENT_SOURCE_DIR}/Point.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/_deps/AutoRefl_Point.inl"
)

Ubpa_AddTarget(
  TEST
  MODE EXE
  SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/_deps/AutoRefl_Point.inl"
  LIB
    Ubpa::UDP_core
)
