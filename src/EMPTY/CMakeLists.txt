Ubpa_GlobGroupSrcs(RST sources PATHS
	${CMAKE_CURRENT_SOURCE_DIR}
	"${PROJECT_SOURCE_DIR}/include/UDP"
)

Ubpa_AddTarget(MODE "EXE" SOURCES ${sources} TEST ON LIBS Ubpa::UTemplate_core)
