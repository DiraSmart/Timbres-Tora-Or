@echo off
echo Configurando PlatformIO en PATH temporal...
set PATH=%PATH%;C:\Users\jbran\.platformio\penv\Scripts
echo.
echo PATH configurado. Ahora puedes usar comandos 'pio'
echo.
pio --version
echo.
echo Para compilar:
echo   pio run
echo   pio run --target buildfs
echo.
cmd /k
