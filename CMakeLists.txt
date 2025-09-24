cmake_minimum_required(VERSION 3.15)

set(TARGET prozubilib)

#To include prozubi to your project use:
#set(PROZUBI_SOURCE_DIR ${CMAKE_SOURCE_DIR}/path/to/prozubi)
#add_subdirectory(${PROZUBI_SOURCE_DIR})

if (NOT DEFINED PROZUBI_SOURCE_DIR)
	SET(PROZUBI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

project(
	${TARGET} VERSION 1.0 
	DESCRIPTION "C library to handle dental clinic"
	HOMEPAGE_URL ""
	LANGUAGES C 
)

SET(KDATA2_SOURCE_DIR ${PROZUBI_SOURCE_DIR}/kdata2)
add_subdirectory(${KDATA2_SOURCE_DIR})

if(APPLE)
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -framework Foundation")
endif()

if(WIN32)
	set(ADDLIBS stbimage)
	INCLUDE_DIRECTORIES(${PROZUBI_SOURCE_DIR}/win32)		
	INCLUDE_DIRECTORIES(${KDATA2_SOURCE_DIR}/cYandexDisk/pthreads-win32/include)	
elseif(APPLE)
	set(ADDSRC stb_image.c)
else()
	if (NOT ANDROID)
		LIST(APPEND ADDLIBS pthread dl)
	endif()
endif()

add_library(${TARGET} STATIC
	prozubilib.c
	text_on_bitmap.c
)

target_link_libraries(${TARGET} kdata2 ${ADDLIBS})

if(${PROZUBI_BUILD_TEST})
	add_executable(${TARGET}_test test.c)
	target_link_libraries(${TARGET}_test ${TARGET})
endif()
