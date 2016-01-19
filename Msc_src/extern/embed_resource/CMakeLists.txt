# Disable stupid MSVC warnings.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(embed_resource
    embed_resource.cpp
)

set_target_properties(embed_resource PROPERTIES FOLDER "External Dependencies")
