set (GEBMerge_SOURCES GEBMerge.cxx MergeManager.cxx)

set (GEBSort_SOURCES GEBSort.cxx GoddessData.cxx DGSProcessor.cxx GODProcessor.cxx ${CMAKE_SOURCE_DIR}/user/UserEventFilter.cxx SortManager.cxx)

#We force the compiler to treat .c files as c++.
# (Seems the original authors used .c as a c++ extension?)
set_source_files_properties(${GEBSort_SOURCES} ${GEBMerge_SOURCES} mkMap.c PROPERTIES LANGUAGE CXX)

add_executable(GEBSort_nogeb ${GEBSort_SOURCES} $<TARGET_OBJECTS:ORRUBA_OBJECT> $<TARGET_OBJECTS:GoddessStruct_OBJECT> $<TARGET_OBJECTS:GODDESSTOOLBOX_OBJECT>)
target_link_libraries(GEBSort_nogeb ${ROOT_LIBRARIES})

add_executable(mkMap mkMap.c $<TARGET_OBJECTS:GODDESSTOOLBOX_OBJECT>)
target_link_libraries(mkMap ${ROOT_LIBRARIES})

add_executable(GEBMerge ${GEBMerge_SOURCES} $<TARGET_OBJECTS:GODDESSTOOLBOX_OBJECT>)
target_link_libraries(GEBMerge ${ZLIB_LIBRARIES} ${ROOT_LIBRARIES})

install(TARGETS GEBSort_nogeb GEBMerge mkMap DESTINATION ${CMAKE_INSTALL_PREFIX})
