#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

root_generate_dictionary(GoddessAnalysisDictionnary GoddessAnalysis.h GoddessCalib.h LINKDEF GoddessAnalysisClassesLinkDef.h)

if (${ROOT_VERSION} VERSION_GREATER "6.0")
	#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GoddessCalibDictionnary_rdict.pcm  DESTINATION ${CMAKE_INSTALL_PREFIX})
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GoddessAnalysisDictionnary_rdict.pcm  DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

set (GODDESSANALYSIS_SOURCES GoddessAnalysis.cxx GoddessCalib.cxx GoddessAnalysisDictionnary.cxx)

add_library(GoddessAnalysis_OBJECT OBJECT ${GODDESSANALYSIS_SOURCES})
add_library(GoddessAnalysis SHARED $<TARGET_OBJECTS:GoddessAnalysis_OBJECT>)
target_link_libraries(GoddessAnalysis ${ROOT_LIBRARIES} GoddessStruct)

install(TARGETS GoddessAnalysis DESTINATION ${CMAKE_INSTALL_PREFIX})

install(PROGRAMS gebmerge.sh gebsort.sh gebsort_old.sh copyRun.sh DESTINATION ${CMAKE_INSTALL_PREFIX})

install(FILES PrintPlots.C calibstrips.cxx MakeEventLists.cxx goddess_analysis_macro.cxx DESTINATION ${CMAKE_INSTALL_PREFIX})
