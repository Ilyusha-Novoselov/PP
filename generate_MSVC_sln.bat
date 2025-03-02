@echo off
set VS_VERSION=Visual Studio 17 2022
set BUILD_DIR=build

if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

cd %BUILD_DIR%

cmake -G "%VS_VERSION%" -A x64 ..
if %errorlevel% neq 0 (
    echo Ошибка при генерации Visual Studio Solution файла
    exit /b 1
)

echo Сборка завершена успешно