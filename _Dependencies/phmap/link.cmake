macro(link_phmap)

   set(DIRNAME "phmap")

   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/_Dependencies/${DIRNAME}/include)

   message("${PROJECT_NAME} -> ${DIRNAME} has been linked.")
endmacro()