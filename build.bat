set INCLUDES=-Isrc/ -Isrc/thirdparty
set BUILD=build/

mkdir build

gcc src/main.c src/renderers/cli_renderer.c %INCLUDES% -o ./build/chess