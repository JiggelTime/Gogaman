include_directories(${Dependencies_root}/AntTweakBar/include)
include_directories(${Dependencies_root}/assimp/include)
include_directories(${Dependencies_root}/glm/include)
include_directories(${Dependencies_root}/glad/include)
include_directories(${Dependencies_root}/KHR/include)
include_directories(${Dependencies_root}/stb_image/include)
include_directories(${Dependencies_root}/GLFW/include)



add_custom_command(TARGET mylibrary POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mylibrary> ${targetfile})