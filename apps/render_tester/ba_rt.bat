
@echo off

set CURRPATH=%cd%
cd ..\..\proj\proj_cmd
call b_crcc.bat
call b_gscc.bat
call b_gsgl.bat
cd %CURRPATH%

if exist render_tester.exe del render_tester.exe

set RT_SOURCES="%GLAD_LIB%" ..\appbase\scene.cc ..\appbase\glfw_loop.cc render_tester.cc

"%VC_BIN64_PATH%\CL" %COMPILER_OPTIONS% /DGLFW_DLL /DGLFW_INCLUDE_NONE %GLAD_INC% %GLF_INC% /I"%BASE_PATH%" /I"%THIRD_PATH%" %RT_SOURCES%

set RT_OBJECTS=glad.obj scene.obj glfw_loop.obj render_tester.obj

"%VC_BIN64_PATH%\LINK" /OUT:render_tester.exe %LINKER_OPTIONS% "%CRCC_LIB%" "%GSCC_LIB%" "%GSGL_LIB%" %GLF_LIB% %RT_OBJECTS%

del .\*.obj
