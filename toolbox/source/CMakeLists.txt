set (GODDESSTOOLBOX_SOURCES GoddessToolbox.cxx)

add_library(GODDESSTOOLBOX_OBJECT OBJECT ${GODDESSTOOLBOX_SOURCES})
add_library(GoddessToolboxLib SHARED $<TARGET_OBJECTS:GODDESSTOOLBOX_OBJECT>)
target_link_libraries(GoddessToolboxLib ${ROOT_LIBRARIES})

install(TARGETS GoddessToolboxLib DESTINATION ${CMAKE_INSTALL_PREFIX})

root_generate_dictionary(GoddessAnalysisDictionnary GoddessAnalysis.h GoddessCalib.h GoddessGeomUtils.h LINKDEF GoddessAnalysisClassesLinkDef.h)

if (${ROOT_VERSION} VERSION_GREATER "6.0")
	#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GoddessCalibDictionnary_rdict.pcm  DESTINATION ${CMAKE_INSTALL_PREFIX})
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GoddessAnalysisDictionnary_rdict.pcm  DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

set (GODDESSANALYSIS_SOURCES GoddessAnalysis.cxx GoddessCalib.cxx GoddessCalibGUI.cxx GoddessGeomUtils.cxx GoddessAnalysisDictionnary.cxx)

add_library(GoddessAnalysis_OBJECT OBJECT ${GODDESSANALYSIS_SOURCES})
add_library(GoddessAnalysis SHARED $<TARGET_OBJECTS:GoddessAnalysis_OBJECT>)
target_link_libraries(GoddessAnalysis ORRUBA GoddessToolboxLib Spectrum)

install(TARGETS GoddessAnalysis DESTINATION ${CMAKE_INSTALL_PREFIX})