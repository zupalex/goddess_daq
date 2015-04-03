set(DGSSort_SOURCES DGSSort.cxx dgsHeader.c GTPrint.c time_stamp.c)
set(GTMerge3_SOURCES GTMerge3.c GTPrint.c GTUtils.c 
	dgsHeader.c get_a_seed.c spe_fun.c time_stamp.c)
#We force the compiler to treat .c files as c++.
# (Seems the original authors used .c as a c++ extension?)
set_source_files_properties(${GTMerge3_SOURCES} ${DGSSort_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(DGSSort ${DGSSort_SOURCES})
target_link_libraries(DGSSort ${ZLIB_LIBRARIES} ${ROOT_LIBRARIES})

add_executable(GTMerge3 ${GTMerge3_SOURCES})
target_link_libraries(GTMerge3 ${ZLIB_LIBRARIES})

add_executable(mkMap mkMap.c)

add_executable(zzip zzip.c)
target_link_libraries(zzip ${ZLIB_LIBRARIES})

add_executable(zunzip zunzip.c)
target_link_libraries(zunzip ${ZLIB_LIBRARIES})

install(TARGETS GTMerge3 mkMap zzip zunzip DESTINATION ${CMAKE_INSTALL_PREFIX})
