set(comp_sources ShmAccessor.cpp)
set(libs hrpModel-3.1 hrpUtil-3.1 hrpsysBaseStub)
add_library(ShmAccessor SHARED ${comp_sources})
target_link_libraries(ShmAccessor ${libs})
set_target_properties(ShmAccessor PROPERTIES PREFIX "")

add_executable(ShmAccessorComp ShmAccessorComp.cpp ${comp_sources})
target_link_libraries(ShmAccessorComp ${libs})

set(target ShmAccessor ShmAccessorComp)

install(TARGETS ${target}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
)
