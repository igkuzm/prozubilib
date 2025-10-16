cmake_minimum_required(VERSION 3.9)

set(TARGET prozubilib)

#To include prozubi to your project use:
#set(PROZUBI_SOURCE_DIR ${CMAKE_SOURCE_DIR}/path/to/prozubi)
#add_subdirectory(${PROZUBI_SOURCE_DIR})

if (NOT DEFINED PROZUBI_SOURCE_DIR)
	SET(PROZUBI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

project(
	${TARGET} VERSION 1.2 
	DESCRIPTION "C library to handle dental clinic"
	HOMEPAGE_URL ""
	LANGUAGES C 
)
set (CMAKE_C_STANDARD 99)

if(APPLE)
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -framework Foundation")
endif()

SET(KDATA2_SOURCE_DIR ${PROZUBI_SOURCE_DIR}/kdata2)
add_subdirectory(${KDATA2_SOURCE_DIR})
SET(CYANDEXDISK_SOURCE_DIR ${KDATA2_SOURCE_DIR}/cYandexDisk)

include_directories(${KDATA2_SOURCE_DIR}/sqlite3)

if (NOT WIN32)

find_package(TIFF)
include_directories(${TIFF_INCLUDE_DIR})
list(APPEND ADDLIBS ${TIFF_LIBRARIES})

find_package(SQLite3)
include_directories(${SQLite3_INCLUDE_DIRS})
list(APPEND ADDLIBS ${SQLite3_LIBRARIES})

endif()

if(WIN32)
#pthread
 add_library(pthread SHARED IMPORTED)
 set_target_properties(pthread PROPERTIES 
	 IMPORTED_LOCATION
	 ${CYANDEXDISK_SOURCE_DIR}/win32/pthreads-win32/lib/pthreadGC1.dll
		IMPORTED_IMPLIB ${CYANDEXDISK_SOURCE_DIR}/win32/pthreads-win32/lib/libpthreadGC1.a)	
  include_directories(${CYANDEXDISK_SOURCE_DIR}/win32/pthreads-win32/include)
  list(APPEND ADDLIBS pthread)
#tiff
	add_library(tiff STATIC IMPORTED)		
	set_target_properties(tiff PROPERTIES 
		IMPORTED_LOCATION
		${PROZUBI_SOURCE_DIR}/win32/tiff/lib/libtiff.dll.a)
	include_directories(${PROZUBI_SOURCE_DIR}/win32/tiff/include)
elseif(APPLE)
else()
	if (NOT ANDROID)
		LIST(APPEND ADDLIBS pthread dl)
	endif()
endif()

add_library(${TARGET} STATIC
	src/bill.c
	src/cases.c
	src/diagnosis.c
	src/doctors.c
	src/documents.c
	src/images.c
	src/mkb.c
	src/nomenklatura.c
	src/passport.c
	src/planlecheniya.c
	src/prices.c
	src/template.c
	src/rtf.c
	src/prozubilib.c
	src/text_on_bitmap.c
	src/fm.c
	src/stb_image.c
	${ADDSRC}
)

target_link_libraries(${TARGET} kdata2 ${ADDLIBS} tiff)

if(${BUILD_TEST})
	add_executable(${TARGET}_test test.c)
	target_link_libraries(${TARGET}_test ${TARGET} ${ADDLIBS})
endif()

#copy files
if (WIN32)
	#curl libs
	#Copy DLLs
	file(GLOB files 
		"${PROZUBI_SOURCE_DIR}/win32/tiff/bin/*.dll"
		"${PROZUBI_SOURCE_DIR}/*.sqlite"
		"${PROZUBI_SOURCE_DIR}/*.ttf"
		"${PROZUBI_SOURCE_DIR}/*.png"
		"${PROZUBI_SOURCE_DIR}/Templates"
		)	
	#Copy
	foreach(_file ${files})
			file(INSTALL
				DESTINATION "${CMAKE_BINARY_DIR}"
				FILES "${_file}"
			)
	endforeach()	
endif()
