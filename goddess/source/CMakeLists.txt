#The ORRUBA dictionary must be Position Independent Code
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
root_generate_dictionary(ORRUBADictionary ORRUBA.h Detector.h siDet.h
orrubaDet.h superX3.h BB10.h QQQ5.h IonChamber.h LiquidScint.h LINKDEF ORRUBALinkDef.h)

if (${ROOT_VERSION} VERSION_GREATER "6.0")
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ORRUBADictionary_rdict.pcm DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

set (ORRUBA_SOURCES SolidVector.cxx Detector.cxx siDet.cxx orrubaDet.cxx
ORRUBA.cxx superX3.cxx BB10.cxx QQQ5.cxx GoddessConfig.cxx IonChamber.cxx
LiquidScint.cxx ORRUBADictionary.cxx)

add_library(ORRUBA_OBJECT OBJECT ${ORRUBA_SOURCES})

add_library(ORRUBA SHARED $<TARGET_OBJECTS:ORRUBA_OBJECT>)
target_link_libraries(ORRUBA ${ROOT_LIBRARIES})
install(TARGETS ORRUBA DESTINATION ${CMAKE_INSTALL_PREFIX})

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")

	add_executable(goddessTest goddessTest.cxx $<TARGET_OBJECTS:ORRUBA_OBJECT>)
	target_link_libraries(goddessTest ${ROOT_LIBRARIES})
	install(TARGETS goddessTest DESTINATION ${CMAKE_INSTALL_PREFIX})

endif()
