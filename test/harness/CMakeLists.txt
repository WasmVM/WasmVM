add_library(harness
    helper.c
    md5.c
    hashmap_t.c
    utils.c
)

add_library(harness_cpp STATIC
    harness.cpp
    main.cpp
)