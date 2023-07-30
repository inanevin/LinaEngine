macro(link_eastl)

   set(DIRNAME "eastl")
   set(LIBNAME "EASTL")

   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/include/EASTL/include)
   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/include/EASTL/test/packages/EAAssert/include)
   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/include/EASTL/test/packages/EABase/include/Common)
   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/include/EASTL/test/packages/EAMain/include)
   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/include/EASTL/test/packages/EAStdC/include)
   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/include/EASTL/test/packages/EATest/include)
   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/include/EASTL/test/packages/EAThread/include)

   if(WIN32)
      target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/Win64/$<CONFIGURATION>/${LIBNAME}.lib)
   endif(WIN32)

   if(LINUX)
      target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/Linux64/$<CONFIGURATION>/${LIBNAME}.a)
   endif(LINUX)

   if(APPLE)
      target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/Apple/$<CONFIGURATION>/${LIBNAME}.a)
   endif(APPLE)

   add_custom_target(NatVis SOURCES ${CMAKE_SOURCE_DIR}/_Dependencies/${DIRNAME}/EASTL.natvis)
   set_target_properties(NatVis PROPERTIES FOLDER "CustomTargets")

   message("${PROJECT_NAME} -> ${LIBNAME} has been linked.")
endmacro()