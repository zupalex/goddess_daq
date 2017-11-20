if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

function(DealWithUserStuffs)

  message(STATUS "Checking for UserAnalysisMacros.cxx and UserEventFilter.cxx")

  if(NOT EXISTS "${CMAKE_SOURCE_DIR}/user/UserAnalysisMacros.cxx")
    message(STATUS "Copying UserAnalysisMacrosTemplate.cxx to UserAnalysisMacros.cxx")
    file(COPY "${CMAKE_SOURCE_DIR}/scripts/UserAnalysisMacrosTemplate.cxx" DESTINATION "${CMAKE_SOURCE_DIR}/user")
    file(RENAME "${CMAKE_SOURCE_DIR}/user/UserAnalysisMacrosTemplate.cxx" "${CMAKE_SOURCE_DIR}/user/UserAnalysisMacros.cxx")
  endif()

  if(NOT EXISTS "${CMAKE_SOURCE_DIR}/user/UserEventFilter.cxx")
    message(STATUS "Copying UserEventFilterTemplate.cxx to UserEventFilter.cxx")
    file(COPY "${CMAKE_SOURCE_DIR}/scripts/UserEventFilterTemplate.cxx" DESTINATION "${CMAKE_SOURCE_DIR}/user")
    file(RENAME "${CMAKE_SOURCE_DIR}/user/UserEventFilterTemplate.cxx" "${CMAKE_SOURCE_DIR}/user/UserEventFilter.cxx")
  endif()

  set(endVer " //")
  
  #--------------------------CHECK OF UserAnalysisMacrosTemplate.cxx-----------------------------------------------
  
  file(STRINGS "${CMAKE_SOURCE_DIR}/scripts/UserAnalysisMacrosTemplate.cxx" versionStr LIMIT_COUNT 1)
  
  set(currUAMVersionBeg "UserAnalysisTemplate version ")
  string(LENGTH ${currUAMVersionBeg} currUAMVerStrLen)
  set(currUEFVersionBeg "UserEventFilterTemplate version ")
  string(LENGTH ${currUEFVersionBeg} currUEFVerStrLen)
  
  string(FIND ${versionStr} ${currUAMVersionBeg} currUAMVerPos)
  string(FIND ${versionStr} ${endVer} endVerPos REVERSE)
  
  
  if((currUAMVerPos LESS 0) OR (endVerPos LESS 0))
    message("${Red}${CMAKE_SOURCE_DIR}/scripts/UserAnalysisMacrosTemplate.cxx version header seems corrupted!${ColourReset}")
  
    return()
  endif()

  
  math(EXPR currUAMVerPos "${currUAMVerPos}+${currUAMVerStrLen}")
  math(EXPR currUAMVerLen "${endVerPos}-${currUAMVerPos}")
  
  string(SUBSTRING ${versionStr} ${currUAMVerPos} ${currUAMVerLen} currUAMVerNum)
  
  message(STATUS "Current UserAnalysisMacrosTemplate version: ${Blue}${currUAMVerNum}${ColourReset}")
  
  #------------------------------CHECK OF UserEventFilterTemplate.cxx--------------------------------------------
  
  file(STRINGS "${CMAKE_SOURCE_DIR}/scripts/UserEventFilterTemplate.cxx" versionStr LIMIT_COUNT 1)
  
  string(FIND ${versionStr} ${currUEFVersionBeg} currUEFVerPos)
  string(FIND ${versionStr} ${endVer} endVerPos REVERSE)
  
  if((currUEFVerPos LESS 0) OR (endVerPos LESS 0))
    message("${Red}${CMAKE_SOURCE_DIR}/scripts/UserEventFilterTemplate.cxx version header seems corrupted!${ColourReset}")
  
    return()
  endif()
  
  math(EXPR currUEFVerPos "${currUEFVerPos}+${currUEFVerStrLen}")
  math(EXPR currUEFVerLen "${endVerPos}-${currUEFVerPos}")
  
  string(SUBSTRING ${versionStr} ${currUEFVerPos} ${currUEFVerLen} currUEFVerNum)
  
  message(STATUS "Current UserEventFiltersTemplate version: ${Blue}${currUEFVerNum}${ColourReset}")
  
  #-----------------------------------CHECK OF UserAnalysisMacros.cxx-------------------------------------------
  
  file(STRINGS "${CMAKE_SOURCE_DIR}/user/UserAnalysisMacros.cxx" versionStr LIMIT_COUNT 1)
  
  string(FIND ${versionStr} ${currUAMVersionBeg} currUAMVerPos)
  string(FIND ${versionStr} ${endVer} endVerPos REVERSE)
  
  
  if((currUAMVerPos LESS 0) OR (endVerPos LESS 0))
  
    message("${Red}${CMAKE_SOURCE_DIR}/user/UserAnalysisMacros.cxx version header seems corrupted!")
    message("${Red}You may want to backup your UserAnalysisMacros.cxx, delete it, do a \"cmake build ..\" and reimplement your changes")

  else()
  
    math(EXPR currUAMVerPos "${currUAMVerPos}+${currUAMVerStrLen}")
    math(EXPR currUAMVerLen "${endVerPos}-${currUAMVerPos}")
    
    string(SUBSTRING ${versionStr} ${currUAMVerPos} ${currUAMVerLen} userUAMVerNum)    
    
    if(NOT (userUAMVerNum EQUAL currUAMVerNum))
      message(STATUS "Current UserAnalysisMacros version: ${Red}${userUAMVerNum}${ColourReset}")
      message("${Red}${CMAKE_SOURCE_DIR}/user/UserAnalysisMacros.cxx version is using an outdated version of the template${ColourReset}")
      message("${Red}This is due to a change in the structure of the root tree or heavy modifications of the tools provided for the analysis.${ColourReset}")
      message("${Red}You may want to backup your UserAnalysisMacros.cxx, delete it, do a \"cmake build ..\" and reimplement your changes.${ColourReset}")
      
    else()
     
      message(STATUS "Current UserAnalysisMacros version: ${Blue}${userUAMVerNum}${ColourReset}")
     
    endif()
    
  endif()
  
    #-----------------------------------CHECK OF UserEventFilter.cxx-------------------------------------------
  
  file(STRINGS "${CMAKE_SOURCE_DIR}/user/UserEventFilter.cxx" versionStr LIMIT_COUNT 1)
  
  string(FIND ${versionStr} ${currUEFVersionBeg} currUEFVerPos)
  string(FIND ${versionStr} ${endVer} endVerPos REVERSE)
  
  
  if((currUEFVerPos LESS 0) OR (endVerPos LESS 0))
    message("${Red}${CMAKE_SOURCE_DIR}/user/UserEventFilter.cxx version header seems corrupted!${ColourReset}")
    message("${Red}You may want to backup your UserEventFilter.cxx, delete it, do a \"cmake build ..\" and reimplement your changes${ColourReset}")

  else()
    
    math(EXPR currUEFVerPos "${currUEFVerPos}+${currUEFVerStrLen}")
    math(EXPR currUEFVerLen "${endVerPos}-${currUEFVerPos}")
    
    string(SUBSTRING ${versionStr} ${currUEFVerPos} ${currUEFVerLen} userUEFVerNum)  
    
    if(NOT (userUEFVerNum EQUAL currUEFVerNum))
      message(STATUS "Current UserEventFilter version: ${Red}${userUEFVerNum}${ColourReset}")
      message("${Red}${CMAKE_SOURCE_DIR}/user/UserEventFilter.cxx version is using an outdated version of the template${ColourReset}")
      message("${Red}This is due to a change in the structure of the root tree or heavy modifications of the tools provided for the analysis.${ColourReset}")
      message("${Red}You may want to backup your UserAnalysisMacros.cxx, delete it, do a \"cmake build ..\" and reimplement your changes.${ColourReset}")
      
    else()
    
      message(STATUS "Current UserEventFilter version: ${Blue}${userUEFVerNum}${ColourReset}")
    
    endif()
    
  endif()

endfunction()