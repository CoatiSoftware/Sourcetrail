# # build type needs to be in added as definition because of visual studio
message(STATUS "setting up the buildtype for versioning")
if(MSVC)
    add_definitions(-DBUILD_TYPE="${ConfigurationName}")
else(MSVC)
     add_definitions(-DBUILD_TYPE="${CMAKE_BUILD_TYPE}")
endif(MSVC)
