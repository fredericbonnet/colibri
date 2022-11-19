pip install conan

rm -rf build && mkdir build && cd build && conan install .. && cd ..
cmake -S . -B build
cmake --build build

