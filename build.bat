@echo off
setlocal
where cl >nul 2>nul
if errorlevel 1 (
  echo Microsoft C++ Build Tools nao encontrado.
  echo Abra "x64 Native Tools Command Prompt for VS" e execute este arquivo novamente.
  pause
  exit /b 1
)
cl /nologo /EHsc /O2 /DUNICODE /D_UNICODE main.cpp /link /SUBSYSTEM:WINDOWS comdlg32.lib shell32.lib /OUT:CS16_Legit_Training_Panel.exe
if errorlevel 1 (
  echo Falha na compilacao.
) else (
  echo Compilado: CS16_Legit_Training_Panel.exe
)
pause
