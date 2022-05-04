
SETLOCAL ENABLEDELAYEDEXPANSION
CD src
@ECHO OFF
FOR %%I IN (*.cpp) DO (
    SET NAME=%%~nI.h
    IF NOT EXIST !NAME! (
        IF EXIST ..\include\!NAME! (
            MKLINK  !NAME! ..\include\!NAME!
        ) ELSE (
            ECHO Skipping %%I
        )
    ) ELSE (
        ECHO Symlink up-to-date: !NAME! 
    )
)

