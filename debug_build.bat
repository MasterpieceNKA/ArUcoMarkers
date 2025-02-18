if exist build\bin\Release\app.exe del build\bin\Release\app.exe

cmake -S . -B build -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang  -DBUILD_TYPE=Release
cmake --build build --config Release

.\build\bin\Release\app.exe 
