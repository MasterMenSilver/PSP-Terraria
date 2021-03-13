for /f "delims=" %%i in ('dir input\ /b /a:-d') do (
takeown /f %%~ni >nul
echo y|>nul ttf2pgf input/%%~ni.ttf output/%%~ni.pgf
)
pause