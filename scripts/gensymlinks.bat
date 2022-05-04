
SETLOCAL ENABLEDELAYEDEXPANSION
@ECHO OFF
FOR %%I IN (*.cpp) DO (
    SET NAME=%%~nI.h
    IF NOT EXIST !NAME! (
        MKLINK  !NAME! ..\include\!NAME!
    )
)


