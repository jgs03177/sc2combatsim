#add_library(lodepng STATIC lodepng)
add_executable(SC2CombatSim
   main.cpp
   player.cpp       player.h
   bots.cpp         bots.h
   simulator.cpp    simulator.h
   combinator.cpp   combinator.h
   recorder.cpp     recorder.h
   configurator.cpp configurator.h
   util.cpp         util.h
   lodepng.cpp      lodepng.h
   jsoncpp.cpp      json/json.h     json/json-forwards.h
)

#target_link_libraries(SC2CombatSim lodepng)

set(LIBRARY_PATH ${PROJECT_SOURCE_DIR}/lib)
set(LIBRARY_NAMES_REL sc2lib sc2api sc2utils sc2protocol libprotobuf civetweb)
set(LIBRARY_NAMES_DEBUG sc2libd sc2apid sc2utilsd sc2protocold libprotobufd civetweb)

#link_directories(${LIBRARY_PATH})

if (NOT APPLE)
  list(APPEND LIBRARY_NAMES_REL "sc2renderer")
  list(APPEND LIBRARY_NAMES_DEBUG "sc2rendererd")
endif()

foreach (SC2LIB IN LISTS LIBRARY_NAMES_DEBUG)
  find_library(SC2LIB_${SC2LIB}
    NAMES "${SC2LIB}"
    PATHS "${LIBRARY_PATH}" "/opt/local/lib" "usr/local/lib"
  )
  if (SC2LIB_${SC2LIB})
    mark_as_advanced(SC2LIB_${SC2LIB})
    #add_library(SC2LIB_${SC2LIB} STATIC IMPORTED)
    list(APPEND LIBRARY_DEBUGS "${SC2LIB_${SC2LIB}}")
  else ()
    message(WARNING "${SC2LIB} not found. Get s2client-api first.")
  endif ()
endforeach()

foreach (SC2LIB IN LISTS LIBRARY_NAMES_REL)
  find_library(SC2LIB_${SC2LIB}
    NAMES "${SC2LIB}"
    PATHS "${LIBRARY_PATH}" "/opt/local/lib" "usr/local/lib"
  )
  if (SC2LIB_${SC2LIB})
    mark_as_advanced(SC2LIB_${SC2LIB})
    #add_library(SC2LIB_${SC2LIB} STATIC IMPORTED)
    list(APPEND LIBRARY_RELS "${SC2LIB_${SC2LIB}}")
  else ()
    message(WARNING "${SC2LIB} not found. Get s2client-api first.")
  endif ()
endforeach()

target_link_libraries(SC2CombatSim
  debug ${LIBRARY_DEBUGS}
  optimized ${LIBRARY_RELS}
  general ${LIBRARY_RELS}
)

#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

if (APPLE)
  target_link_libraries(SC2CombatSim "-framework Carbon")
endif ()

if (UNIX AND NOT APPLE)
  target_link_libraries(SC2CombatSim pthread dl)
endif ()

target_include_directories(SC2CombatSim PUBLIC ${PROJECT_SOURCE_DIR}/include)
