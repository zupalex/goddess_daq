set (GEBSort_SOURCES GEBSort.cxx GTPrint.c time_stamp.c GEBHeader.c
get_a_seed.c bin_mode1.c bin_mode3.c bin_mode2.c bin_template.c bin_dgs.c
bin_dgod.c bin_dfma.c bin_gtcal.c spe_fun.c str_decomp.c)
set (GEBMerge_SOURCES GEBMerge.c spe_fun.c get_a_seed.c time_stamp.c printEvent.c)
#We force the compiler to treat .c files as c++.
# (Seems the original authors used .c as a c++ extension?)
set_source_files_properties(${GEBSort_SOURCES} ${GEBMerge_SOURCES} ag2mode2.c PROPERTIES LANGUAGE CXX)

add_executable(GEBSort_nogeb ${GEBSort_SOURCES} $<TARGET_OBJECTS:ORRUBA_OBJECT>)
target_link_libraries(GEBSort_nogeb ${ROOT_LIBRARIES})

add_executable(mkMap mkMap.c)

#add_executable(GEBFilter GEBFilter.c GF_veto_cube.c)

add_executable(GEBMerge ${GEBMerge_SOURCES})
target_link_libraries(GEBMerge ${ZLIB_LIBRARIES})

#add_executable(ag2mode2 ag2mode2.c spe_fun.c)

#install(TARGETS GEBSort_nogeb mkMap GEBFilter GEBMerge ag2mode2 DESTINATION ${CMAKE_INSTALL_PREFIX})
install(TARGETS GEBSort_nogeb GEBMerge mkMap DESTINATION ${CMAKE_INSTALL_PREFIX})
