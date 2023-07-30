macro(link_alut)

   set(DIRNAME "alut")
   set(LIBNAME "alut")

   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/include)

   if(WIN32)
      target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/Win64/$<CONFIGURATION>/${LIBNAME}.lib)
   endif(WIN32)

   if(APPLE)
      target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/Apple/$<CONFIGURATION>/${LIBNAME}.a)
   endif(APPLE)

   if(LINUX)
      target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/Linux64/$<CONFIGURATION>/${LIBNAME}.a)
   endif(LINUX)

   message("${PROJECT_NAME} -> ${LIBNAME} has been linked.")
endmacro()