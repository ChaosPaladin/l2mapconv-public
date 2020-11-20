# GLEW.
option(glew-cmake_BUILD_SHARED "Build shared glew library" ON)
set(glew-cmake_BUILD_SHARED OFF)
option(glew-cmake_BUILD_MULTI_CONTEXT "Build the multi-context glew library" ON)
set(glew-cmake_BUILD_MULTI_CONTEXT OFF)
add_subdirectory(glew)

# Recast.
option(RECASTNAVIGATION_DEMO "Build demo" ON)
set(RECASTNAVIGATION_DEMO OFF)
option(RECASTNAVIGATION_TESTS "Build tests" ON)
set(RECASTNAVIGATION_TESTS OFF)
option(RECASTNAVIGATION_EXAMPLES "Build examples" ON)
set(RECASTNAVIGATION_EXAMPLES OFF)
add_subdirectory(recast)

# Other libs.
add_subdirectory(llvm)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(imgui)
add_subdirectory(stb)
