Ubpa_GetTargetName(tools "${PROJECT_SOURCE_DIR}/src/tools")
Ubpa_AddTarget(
  TEST
  MODE EXE
  LIB
    ${tools}
)
