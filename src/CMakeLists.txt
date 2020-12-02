
set(SOURCES main.c
        promodularSOEM.h
        promodularSOEM.c)

add_executable(promodularSOEM ${SOURCES})
target_link_libraries(promodularSOEM soem)
install(TARGETS promodularSOEM DESTINATION bin)
