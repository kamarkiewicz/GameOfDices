file (GLOB PROJECT_SRC
    "*.c"
    "*.h"
    "DryEngine/*.c"
    "DryEngine/*.h"
)

add_executable (${EXECUTABLE_NAME} ${PROJECT_SRC})

if (CSFML_FOUND)
	include_directories (${CSFML_INCLUDE_DIR})
	target_link_libraries (${EXECUTABLE_NAME} ${CSFML_LIBRARIES})
endif ()

if (LUA_FOUND)
  include_directories(${LUA_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${LUA_LIBRARIES})
endif ()

# install (PROGRAMS ${EXECUTABLE_NAME} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
