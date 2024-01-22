project(ChunkList)

set(SOURCE_FILES ChunkList.hpp)

add_library(ChunkList STATIC ${SOURCE_FILES})

set_target_properties(
        ChunkList
        PROPERTIES
        LINKER_LANGUAGE CXX
)