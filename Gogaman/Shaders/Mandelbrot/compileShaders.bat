%VK_SDK_PATH%/Bin32/glslc.exe mandelbrot.vert -o v_Mandelbrot.spv

%VK_SDK_PATH%/Bin32/glslc.exe postprocess.vert -o v_Postprocess.spv
%VK_SDK_PATH%/Bin32/glslc.exe postprocess.frag -o p_Postprocess.spv

pause