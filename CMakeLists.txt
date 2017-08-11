project( myProject )

# set your include directories (if you have any)
include_directories( include )

# tell cmake what what are your source files
set( MY_CPP_FILES 
    src/source1.cpp
    src/source2.cpp
    src/srcfolder1/source11.cpp
    src/srcfolder1/source12.cpp
    src/srcfolder2/source21.cpp
    src/srcfolder2/source22.cpp )

# if you are creating an executable then do so like this
# note that ${MY_CPP_FILES} will replace it with its content
add_executable( myExec ${MY_CPP_FILES} )

# if you instead want a library, do it like so
add_library( myStaticLib STATIC ${MY_CPP_FILES} ) # create a static library
add_library( mySharedLib SHARED ${MY_CPP_FILES} ) # create a shared library
