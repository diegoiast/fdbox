@ECHO OFF

echo. Welcome to the EPIC fdbox DOS environment distro
echo. Available tools in %%PATH%%
echo.    TC 2.01
echo.    RHIDE+DJGPP (gcc 9.3)
echo.    PacificC : C free compiler
echo.    4DOS
echo.    NDN - Necromancer DOS Navigator

SET DJGPP=c:\djgpp\djgpp.envd
SET PATH=%PATH%;c:\freedos\bin;c:\ndn\;c:\tc;c:\pacific\bin;c:\djgpp\bin
