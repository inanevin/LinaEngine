
# Make sure Resources directory exist.
add_custom_command(
	TARGET ${PROJECT_NAME}
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory "${LINA_WORKING_DIRECTORY}/Resources")

# Copy Core resources to working directory
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/LinaCore/_Resources" "${LINA_WORKING_DIRECTORY}/Resources/")

# Copy Project resources to working directory
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/_Resources" "${LINA_WORKING_DIRECTORY}/Resources/")

# Copy dlls to binary directory
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/_Dependencies/bin/${PLATFORM}/$<CONFIGURATION>" "${LINA_WORKING_DIRECTORY}/")

