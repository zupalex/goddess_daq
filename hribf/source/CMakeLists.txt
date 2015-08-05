add_executable(hribfConvert ldfConvert.cxx hribfBuffer.cxx mainBuffer.cxx)
install(TARGETS hribfConvert DESTINATION ${CMAKE_INSTALL_PREFIX})
