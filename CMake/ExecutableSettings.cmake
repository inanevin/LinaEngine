if(MSVC)
   set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${LINA_WORKING_DIRECTORY})
endif()

if(APPLE)
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    XCODE_GENERATE_SCHEME TRUE
    XCODE_SCHEME_WORKING_DIRECTORY ${LINA_WORKING_DIRECTORY}
)
endif()

# Copy Core resources to working directory
# add_custom_command(
# TARGET ${PROJECT_NAME}
# POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaCore/_Resources" "${LINA_WORKING_DIRECTORY}/Resources/")
# 
# # Copy Project resources to working directory
# add_custom_command(
# TARGET ${PROJECT_NAME}
# POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/_Resources" "${LINA_WORKING_DIRECTORY}/Resources/")
# 
# # Copy Core resources to binary directory
# add_custom_command(
# TARGET ${PROJECT_NAME}
# POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaCore/_Resources" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/")
# 
# # Copy Project resources to binary directory
# add_custom_command(
# TARGET ${PROJECT_NAME}
# POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/_Resources" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/Resources/")