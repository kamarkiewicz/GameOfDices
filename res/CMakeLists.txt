file (GLOB PROJECT_RES
	"*.ogg"
	"*.ttf"
	"*.png"
	"*.lua"
)

install (FILES ${PROJECT_RES} DESTINATION ${RESOURCE_OUTPUT_PATH})