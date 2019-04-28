add_library(gogaman SHARED ${Gogaman_root}/src/Gogaman/application.cpp ${Gogaman_root}/src/Gogaman/application.h)

target_include_directories(gogaman PUBLIC ${Dependencies_root}/AntTweakBar/include)
target_include_directories(gogaman PUBLIC ${Dependencies_root}/assimp/include)
target_include_directories(gogaman PUBLIC ${Dependencies_root}/glm/include)
target_include_directories(gogaman PUBLIC ${Dependencies_root}/glad/include)
target_include_directories(gogaman PUBLIC ${Dependencies_root}/KHR/include)
target_include_directories(gogaman PUBLIC ${Dependencies_root}/stb_image/include)
target_include_directories(gogaman PUBLIC ${Dependencies_root}/GLFW/include)

add_custom_command(TARGET mylibrary POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mylibrary> ${targetfile})