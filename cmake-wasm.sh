#!/bin/sh
rm -rf build/wasm32-wasi
cmake -DCMAKE_TOOLCHAIN_FILE="../../wasi-sdk/wasi-sdk.cmake" -G "Unix Makefiles" \
    -S . -B build/wasm32-wasi \
    -DBUILD_SHARED_LIBS=OFF \
    -DUSE_THREADS=OFF \
    -DCMAKE_MODULE_PATH=../picotest
cmake --build build/wasm32-wasi
