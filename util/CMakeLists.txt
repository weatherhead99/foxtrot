add_subdirectory(PRT)
add_subdirectory(ArdenBuck)

add_library(foxtrot_util  $<TARGET_OBJECTS:PRT> $<TARGET_OBJECTS:ardenbuck>)
