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

SET(KDATA2_SOURCE_DIR ${PROZUBI_SOURCE_DIR}/kdata2)
add_subdirectory(${KDATA2_SOURCE_DIR})
SET(CYANDEXDISK_SOURCE_DIR ${KDATA2_SOURCE_DIR}/cYandexDisk)

if(APPLE)
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -framework Foundation")
endif()

if(WIN32)
  include_directories(${PROZUBI_SOURCE_DIR}/include/tiff)
 #pthread
 add_library(pthread SHARED IMPORTED)
 set_target_properties(pthread PROPERTIES 
   IMPORTED_LOCATION
   ${CYANDEXDISK_SOURCE_DIR}/win32/pthreads-win32/lib/pthreadGC1.dll
    IMPORTED_IMPLIB ${CYANDEXDISK_SOURCE_DIR}/win32/pthreads-win32/lib/libpthreadGC1.a)	
  include_directories(${CYANDEXDISK_SOURCE_DIR}/win32/pthreads-win32/include)
  list(APPEND ADDLIBS pthread)
#curl
  add_library(curl SHARED IMPORTED)		
  set_target_properties(curl PROPERTIES 
   IMPORTED_LOCATION
   ${CYANDEXDISK_SOURCE_DIR}/win32/curl/bin/libcurl.dll	
    IMPORTED_IMPLIB ${CYANDEXDISK_SOURCE_DIR}/win32/curl/lib/libcurl.dll.a)	
   include_directories(${CYANDEXDISK_SOURCE_DIR}/win32/curl/include)	
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

target_link_libraries(${TARGET} kdata2 ${ADDLIBS})

if(${PROZUBI_BUILD_TEST})
	add_executable(${TARGET}_test test.c)
	target_link_libraries(${TARGET}_test ${TARGET})
endif()
