add_subdirectory(PRT)

add_library(foxtrot_util  $<TARGET_OBJECTS:PRT>)