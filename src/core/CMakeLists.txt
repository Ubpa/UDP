set(c_options "")
if(MSVC)
  list(APPEND c_options "/wd5030")
endif()

Ubpa_AddTarget(
  MODE HEAD
  LIB
    Ubpa::UTemplate_core
	Ubpa::UContainer_core
  INC
    "include"
  C_OPTION
    ${c_options}
)
