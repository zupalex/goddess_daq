set (GEBMerge_SOURCES GEBMerge.cxx spe_fun.c get_a_seed.c printEvent.c MergeManager.cxx)

set (GEBSort_SOURCES GEBSort.cxx time_stamp.c bin_dgs.c bin_god.c bin_dgod.c bin_agod.c GoddessData.cxx)

#We force the compiler to treat .c files as c++.
# (Seems the original authors used .c as a c++ extension?)
set_source_files_properties(${GEBSort_SOURCES} ${GEBMerge_SOURCES} ag2mode2.c mkMap.c PROPERTIES LANGUAGE CXX)

root_generate_dictionary(ProcessManagersDictionnary SortManager.h LINKDEF PMLinkDefs.h)

if (${ROOT_VERSION} VERSION_GREATER "6.0")
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ProcessManagersDictionnary_rdict.pcm  DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

set (SortManager_SOURCES ${CMAKE_SOURCE_DIR}/scripts/UserEventFilter.cxx SortManager.cxx ProcessManagersDictionnary.cxx)

add_library(ProcessManagers_OBJECT OBJECT ${SortManager_SOURCES})
add_library(ProcessManagers SHARED $<TARGET_OBJECTS:ProcessManagers_OBJECT>)
target_link_libraries(ProcessManagers ${ROOT_LIBRARIES})

install(TARGETS ProcessManagers DESTINATION ${CMAKE_INSTALL_PREFIX})

add_executable(GEBSort_nogeb ${GEBSort_SOURCES} $<TARGET_OBJECTS:ProcessManagers_OBJECT> $<TARGET_OBJECTS:ORRUBA_OBJECT> $<TARGET_OBJECTS:GoddessStruct_OBJECT>)
target_link_libraries(GEBSort_nogeb ${ROOT_LIBRARIES})

add_executable(mkMap mkMap.c)

#add_executable(GEBFilter GEBFilter.c GF_veto_cube.c)

add_executable(GEBMerge ${GEBMerge_SOURCES})
target_link_libraries(GEBMerge ${ZLIB_LIBRARIES})

#add_executable(ag2mode2 ag2mode2.c spe_fun.c)

#install(TARGETS GEBSort_nogeb mkMap GEBFilter GEBMerge ag2mode2 DESTINATION ${CMAKE_INSTALL_PREFIX})
install(TARGETS GEBSort_nogeb GEBMerge mkMap DESTINATION ${CMAKE_INSTALL_PREFIX})
