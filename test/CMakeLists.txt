
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(CPPUTEST REQUIRED cpputest)

add_executable(FlowTest)
add_test(NAME FlowTest COMMAND FlowTest)

target_include_directories(FlowTest
PRIVATE
    include/
)

target_sources(FlowTest
PRIVATE
    source/main.cpp
    source/data.cpp
    source/component_combine_tests.cpp
    source/component_invert_tests.cpp
    source/component_toggle_tests.cpp
    source/inoutport_tests.cpp
    source/trigger_tests.cpp
    source/component_convert_tests.cpp
    source/component_split_tests.cpp
    source/component_updowncounter_tests.cpp
    source/reactor_tests.cpp
    source/component_counter_tests.cpp
    source/component_timer_tests.cpp
    source/connection_tests.cpp
    source/port_tests.cpp
    source/testreactor_tests.cpp
    source/waitfor_tests.cpp
    source/platform_cpputest.cpp
)

target_link_libraries(FlowTest 
    Flow
    ${CPPUTEST_LDFLAGS}
)

# add_executable(FlowCoverage)

# target_compile_options(FlowCoverage
# PRIVATE
#     -fprofile-arcs
#     -ftest-coverage
# )

# target_compile_definitions(FlowCoverage
# PRIVATE
#     -DNDEBUG
# )

# target_link_options(FlowCoverage
# PRIVATE
#     -fprofile-arcs
#     -ftest-coverage
#     -lgcov
# )

# target_include_directories(FlowCoverage
# PRIVATE
#     "${PROJECT_SOURCE_DIR}/../include"
#     "${PROJECT_SOURCE_DIR}/include"
# )

# target_sources(FlowCoverage
# PRIVATE
#     ../source/flow/platform_cpputest.cpp
#     ../source/flow/components.cpp
#     ../source/flow/flow.cpp
#     ../source/flow/reactor.cpp
#     source/main.cpp
#     source/data.cpp
#     source/check.cpp
#     source/component_combine_tests.cpp
#     source/component_invert_tests.cpp
#     source/component_toggle_tests.cpp
#     source/inoutport_tests.cpp
#     source/queue_tests.cpp
#     source/trigger_tests.cpp
#     source/component_convert_tests.cpp
#     source/component_split_tests.cpp
#     source/component_updowncounter_tests.cpp
#     source/pool_tests.cpp
#     source/reactor_tests.cpp
#     source/component_counter_tests.cpp
#     source/component_timer_tests.cpp
#     source/connection_tests.cpp
#     source/port_tests.cpp
#     source/testreactor_tests.cpp
#     source/waitfor_tests.cpp
# )

# target_link_libraries(FlowCoverage 
#     Threads::Threads
#     CONAN_PKG::cpputest
# )
