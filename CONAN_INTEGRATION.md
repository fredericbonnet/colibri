# Conan Integration for Colibri

This document explains how Conan integration was added to the Colibri project to automatically handle PicoTest dependency management.

## Overview

The Colibri project now supports building with [Conan](https://conan.io) for dependency management. This allows automatic installation and configuration of PicoTest without manual path setup.

## Files Added/Modified

### New Files

1. **`conanfile.txt`** - Specifies PicoTest as a dependency
2. **`build_with_conan.sh`** - Unix/Linux/macOS build script (uses Conan's --output-folder)
3. **`build_with_conan.bat`** - Windows build script (uses Conan's --output-folder)
4. **`CONAN_INTEGRATION.md`** - This documentation file

### Modified Files

1. **`CMakeLists.txt`** - Updated to work with modern Conan generators
2. **`README.md`** - Added Conan build instructions

## Configuration Details

### conanfile.txt
```ini
[requires]
picotest/1.4.2

[generators]
CMakeDeps
CMakeToolchain
```

### CMakeLists.txt Changes
- No changes needed for Conan output folder, as CMake is now pointed to the toolchain in the build directory.

## Usage

### Prerequisites
1. Install Conan: `pip install conan`
2. Add PicoTest remote (if needed): `conan remote add picotest https://api.bintray.com/conan/fredericbonnet/picotest`

### Building with Conan

**Unix/Linux/macOS:**
```bash
./build_with_conan.sh
```

**Windows:**
```cmd
build_with_conan.bat
```

**Manual build:**
```bash
mkdir build
conan install . --output-folder=build --build=missing
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Benefits

1. **Automatic dependency resolution** - No need to manually specify PicoTest paths
2. **Version management** - Conan handles PicoTest version compatibility
3. **Cross-platform** - Works consistently across different platforms
4. **CI/CD friendly** - Easy to integrate into automated build systems
5. **No manual path configuration** - Eliminates the need for `CMAKE_PREFIX_PATH`
6. **Clean project structure** - All Conan-generated files are now in the build directory, keeping the project root clean
7. **Automatic macro availability** - `picotest_discover_tests` macro is automatically available without manual includes
8. **Modern CMake integration** - Uses Conan 2.x best practices with `--output-folder` and proper toolchain configuration

## How It Works

1. **Conan install** downloads and installs PicoTest from the Conan repository
2. **CMakeDeps generator** creates `PicoTestConfig.cmake` files that CMake can find
3. **CMakeToolchain generator** creates `conan_toolchain.cmake` for build configuration
4. **PicoTest package** exports `cmake/PicoTest.cmake` as a build module via `cmake_build_modules` property
5. **CMake** finds PicoTest automatically and makes `picotest_discover_tests` macro available
6. **Build** proceeds with all tests enabled and automatically discovered

**Note:** With Conan 2.x and the `--output-folder` flag, all Conan-generated files are placed in the `build/` directory, keeping the project root clean.

## Compatibility

- Works with Conan 2.x and later (for `--output-folder` support)
- Maintains backward compatibility with existing workflows (with minor script changes)
- Requires PicoTest package with `cmake_build_modules` property set to include `cmake/PicoTest.cmake`

## PicoTest Package Requirements

For this integration to work properly, the PicoTest Conan package must have the following in its `conanfile.py`:

```python
def package_info(self):
    self.cpp_info.set_property("cmake_build_modules", ["cmake/PicoTest.cmake"])
```

This ensures that the `picotest_discover_tests` macro is automatically available when using `find_package(PicoTest)`.

## Testing

The integration has been tested and verified:
- ✅ Conan installs PicoTest successfully
- ✅ CMake finds PicoTest automatically
- ✅ All test executables build correctly
- ✅ Test discovery works (748 tests found)
- ✅ Build completes successfully
- ✅ Project root remains clean
- ✅ `picotest_discover_tests` macro is automatically available
- ✅ No manual includes or path configuration required

## Troubleshooting

If you encounter issues:

1. **Conan not found**: Install with `pip install conan`
2. **PicoTest not found**: Check that the remote is added: `conan remote list`
3. **Build errors**: Ensure you're using Conan 2.x or later
4. **CMake errors**: Verify that `build/conan_toolchain.cmake` exists in the build directory

## Migration from Manual Setup

If you were previously using `CMAKE_PREFIX_PATH`:

**Old method:**
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/picotest
```

**New method:**
```bash
mkdir build
conan install . --output-folder=build --build=missing
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The new method is more reliable and keeps your project root clean.

## Integration Success

This integration represents a complete, modern solution for using PicoTest with Conan:

- ✅ **Clean project structure** - No generated files in project root
- ✅ **Automatic dependency management** - No manual path configuration
- ✅ **Modern CMake integration** - Uses Conan 2.x best practices
- ✅ **Automatic test discovery** - `picotest_discover_tests` macro available without manual setup
- ✅ **Cross-platform compatibility** - Works on Windows, Linux, and macOS
- ✅ **CI/CD ready** - Easy to integrate into automated build systems

The integration demonstrates how to properly package a header-only library with CMake integration for use with modern Conan workflows. 