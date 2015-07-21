#The ORRUBA dictionary must be Position Independent Code
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
#root_generate_dictionary(
root_generate_dictionary(ORRUBADictionary ORRUBA.h superX3.h BB10.h LINKDEF ORRUBALinkDef.h)

if (${ROOT_VERSION} VERSION_GREATER "6.0")
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ORRUBADictionary_rdict.pcm DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

set (ORRUBA_SOURCES siDet.cxx ORRUBA.cxx superX3.cxx BB10.cxx ORRUBADictionary.cxx)

add_library(ORRUBA_OBJECT OBJECT ${ORRUBA_SOURCES})

add_library(ORRUBA SHARED $<TARGET_OBJECTS:ORRUBA_OBJECT>)
target_link_libraries(ORRUBA ${ROOT_LIBRARIES})
install(TARGETS ORRUBA DESTINATION ${CMAKE_INSTALL_PREFIX})

set (GEBSort_SOURCES GEBSort.cxx GTPrint.c time_stamp.c GEBHeader.c get_a_seed.c bin_mode1.c bin_mode3.c bin_mode2.c bin_template.c bin_dgs.c spe_fun.c bin_gtcal.c str_decomp.c)
set (GEBMerge_SOURCES GEBMerge.c spe_fun.c get_a_seed.c time_stamp.c printEvent.c)
#We force the compiler to treat .c files as c++.
# (Seems the original authors used .c as a c++ extension?)
set_source_files_properties(${GEBSort_SOURCES} ${GEBMerge_SOURCES} ag2mode2.c PROPERTIES LANGUAGE CXX)

add_executable(GEBSort ${GEBSort_SOURCES} $<TARGET_OBJECTS:ORRUBA_OBJECT>)
target_link_libraries(GEBSort ${ROOT_LIBRARIES})

add_executable(mkMap mkMap.c)

add_executable(GEBFilter GEBFilter.c GF_veto_cube.c)

add_executable(GEBMerge ${GEBMerge_SOURCES})
target_link_libraries(GEBMerge ${ZLIB_LIBRARIES})

add_executable(ag2mode2 ag2mode2.c spe_fun.c)

install(TARGETS GEBSort mkMap GEBFilter GEBMerge ag2mode2 DESTINATION ${CMAKE_INSTALL_PREFIX})
