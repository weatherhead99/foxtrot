#version from git revision

include(GetGitRevisionDescription)

git_describe(TAG --tags --dirty=-d)
get_git_head_revision(REFSPEC HASH)

if(TAG STREQUAL "-128-NOTFOUND")
    message(WARNING "no git tag found, using hash as version") 
    set(TAGGED "false")
else()
    message(STATUS "git tag found: ${TAG}")
    set(TAGGED "true")
endif()

string(SUBSTRING ${HASH} 0 8 SHORTVERS)


if(TAGGED STREQUAL "true")
    set(VERSION ${TAG})
else()
    set(VERSION "g${SHORTVERS}")
endif()

#parse the version information into pieces.
# string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
# string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
# string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
# string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")
# set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

