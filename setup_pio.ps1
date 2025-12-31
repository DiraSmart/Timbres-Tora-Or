# Script para configurar PlatformIO en PATH temporal (solo esta sesión)
Write-Host "Configurando PlatformIO en PATH temporal..." -ForegroundColor Cyan
$env:Path += ";C:\Users\jbran\.platformio\penv\Scripts"

Write-Host ""
Write-Host "PATH configurado. Ahora puedes usar comandos 'pio'" -ForegroundColor Green
Write-Host ""

# Verificar versión
pio --version

Write-Host ""
Write-Host "Para compilar:" -ForegroundColor Yellow
Write-Host "  pio run" -ForegroundColor White
Write-Host "  pio run --target buildfs" -ForegroundColor White
Write-Host ""
Write-Host "Esta configuración es temporal y solo aplica a esta sesión de PowerShell" -ForegroundColor Gray
