macro(link_phmap)

   set(DIRNAME "phmap")

   target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/Dependencies/${DIRNAME}/include)

   message("${PROJECT_NAME} -> ${DIRNAME} has been linked.")
endmacro()