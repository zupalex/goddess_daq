#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

install(PROGRAMS gebmerge.sh gebsort.sh gebsort_old.sh copyRun.sh DESTINATION ${CMAKE_INSTALL_PREFIX})

install(FILES PrintPlots.C calibstrips.cxx MakeEventLists.cxx goddess_analysis_macro.cxx DESTINATION ${CMAKE_INSTALL_PREFIX})