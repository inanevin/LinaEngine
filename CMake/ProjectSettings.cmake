
# Language standard

if(MSVC)
   add_definitions("/MP")
endif()

if(APPLE)
   set_target_properties(${PROJECT_NAME} PROPERTIES
       MACOSX_BUNDLE TRUE
   )

endif()

#--------------------------------------------------------------------
# Properties
#--------------------------------------------------------------------
   
# To be retrieved from git later.
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_BUILD=0)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_MAJOR=2)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_MINOR=0)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_PATCH=0)
add_definitions(-DLINA_CONFIGURATION="$<CONFIGURATION>")

