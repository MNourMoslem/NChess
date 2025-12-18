#!/usr/bin/env python3
"""
Build script for NChess C library
Usage: python build.py [command1] [command2] ...

Commands:
  clean       - Remove build directory
  build       - Build library (release mode)
  build-debug - Build library (debug mode)
  test        - Compile and run tests (requires library)
  test-debug  - Compile and run tests in debug mode (requires library)

Compiler Options:
  --compiler=<gcc|clang|msvc>  - Specify compiler (auto-detected if not provided)

Examples:
  python build.py build
  python build.py clean build
  python build.py build test
  python build.py clean build-debug test-debug
  python build.py --compiler=msvc build
  python build.py --compiler=gcc clean build test
"""

import os
import sys
import subprocess
import glob
import shutil
from pathlib import Path


# Compiler configurations
class CompilerConfig:
    def __init__(self, name, exe, base_flags, debug_flags, release_flags, ar_cmd=None, ar_flags=None):
        self.name = name
        self.exe = exe
        self.base_flags = base_flags
        self.debug_flags = debug_flags
        self.release_flags = release_flags
        self.ar_cmd = ar_cmd or "ar"
        self.ar_flags = ar_flags or ["rcs"]

COMPILERS = {
    "gcc": CompilerConfig(
        name="gcc",
        exe="gcc",
        base_flags=["-Wall", "-Wextra", "-std=c11"],
        debug_flags=["-g", "-O0"],
        release_flags=[
            "-Wsign-compare", "-DNDEBUG", "-g", "-fwrapv", "-O2", "-Wall", "-g",
            "-fstack-protector-strong", "-Wformat", "-Werror=format-security",
            "-g", "-fwrapv", "-O2", "-fPIC"
        ],
        ar_cmd="ar",
        ar_flags=["rcs"]
    ),
    "clang": CompilerConfig(
        name="clang",
        exe="clang",
        base_flags=["-Wall", "-Wextra", "-std=c11"],
        debug_flags=["-g", "-O0"],
        release_flags=[
            "-Wsign-compare", "-DNDEBUG", "-g", "-fwrapv", "-O2", "-Wall", "-g",
            "-fstack-protector-strong", "-Wformat", "-Werror=format-security",
            "-g", "-fwrapv", "-O2", "-fPIC"
        ],
        ar_cmd="ar",
        ar_flags=["rcs"]
    ),
    "msvc": CompilerConfig(
        name="msvc",
        exe="cl",
        base_flags=["/nologo", "/TC"],
        debug_flags=["/Od", "/Zi", "/RTC1", "/MDd"],
        release_flags=[
            "/O2", "/Oi", "/Ot", "/GL", "/MD", "/DNDEBUG",
            "/W4", "/wd4996", "/wd4820", "/wd4710", "/wd4711", 
            "/wd5045", "/wd4115", "/wd4204", "/wd4100", "/wd4255"
        ],
        ar_cmd="lib",
        ar_flags=["/nologo"]
    )
}

# Global compiler config (will be set during initialization)
CC_CONFIG = None

SRC_DIR = "nchess"
TEST_DIR = "test"
BUILD_DIR = "build"
OBJ_DIR = os.path.join(BUILD_DIR, "obj")
TEST_OBJ_DIR = os.path.join(BUILD_DIR, "test_obj")
BIN_DIR = os.path.join(BUILD_DIR, "bin")


def get_library_name():
    """Get library name based on compiler"""
    if CC_CONFIG.name == "msvc":
        return "nchess.lib"
    return "libnchess.a"


def get_executable_name(name):
    """Get executable name based on platform"""
    if os.name == "nt":
        return f"{name}.exe"
    return name


TARGET = None  # Will be set after compiler detection
TEST_TARGET = None  # Will be set after compiler detection


def detect_compiler():
    """Auto-detect available compiler"""
    # Try to detect MSVC first on Windows
    if os.name == "nt":
        if shutil.which("cl"):
            return "msvc"
        
        # Try to find Visual Studio using vswhere
        vswhere_path = r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
        if os.path.exists(vswhere_path):
            try:
                result = subprocess.run(
                    [vswhere_path, "-latest", "-property", "installationPath"],
                    capture_output=True, text=True, timeout=5
                )
                if result.returncode == 0 and result.stdout.strip():
                    vs_path = result.stdout.strip()
                    # Try to find cl.exe in Visual Studio installation
                    possible_paths = [
                        os.path.join(vs_path, "VC", "Tools", "MSVC", "*", "bin", "Hostx64", "x64", "cl.exe"),
                        os.path.join(vs_path, "VC", "Tools", "MSVC", "*", "bin", "Hostx86", "x86", "cl.exe"),
                    ]
                    for pattern in possible_paths:
                        matches = glob.glob(pattern)
                        if matches:
                            return "msvc"
            except:
                pass
    
    # Try GCC
    if shutil.which("gcc"):
        return "gcc"
    
    # Try Clang
    if shutil.which("clang"):
        return "clang"
    
    return None


def setup_msvc_environment():
    """Setup MSVC environment variables by calling vcvarsall.bat"""
    # Try to find vcvarsall.bat
    vswhere_path = r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
    if not os.path.exists(vswhere_path):
        return False
    
    try:
        # Get Visual Studio installation path
        result = subprocess.run(
            [vswhere_path, "-latest", "-property", "installationPath"],
            capture_output=True, text=True, timeout=5
        )
        if result.returncode != 0 or not result.stdout.strip():
            return False
        
        vs_path = result.stdout.strip()
        vcvarsall = os.path.join(vs_path, "VC", "Auxiliary", "Build", "vcvarsall.bat")
        
        if not os.path.exists(vcvarsall):
            return False
        
        # Determine architecture
        import platform
        arch = "x64" if platform.machine().endswith('64') else "x86"
        
        # Run vcvarsall.bat and capture environment variables
        cmd = f'"{vcvarsall}" {arch} && set'
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        
        if result.returncode != 0:
            return False
        
        # Parse and set environment variables
        for line in result.stdout.split('\n'):
            if '=' in line:
                key, _, value = line.partition('=')
                key = key.strip()
                value = value.strip()
                if key and value:
                    os.environ[key] = value
        
        return True
    except:
        return False


def setup_compiler(compiler_name=None):
    """Setup compiler configuration"""
    global CC_CONFIG, TARGET, TEST_TARGET
    
    if compiler_name is None:
        compiler_name = detect_compiler()
        if compiler_name is None:
            print("Error: No compiler found!")
            print("Please install GCC, Clang, or MSVC and ensure it's in your PATH.")
            sys.exit(1)
        print(f"Auto-detected compiler: {compiler_name}")
    else:
        if compiler_name not in COMPILERS:
            print(f"Error: Unknown compiler '{compiler_name}'")
            print(f"Available compilers: {', '.join(COMPILERS.keys())}")
            sys.exit(1)
        
        # Verify compiler is available
        test_exe = COMPILERS[compiler_name].exe
        if not shutil.which(test_exe):
            print(f"Error: Compiler '{compiler_name}' ({test_exe}) not found in PATH")
            sys.exit(1)
        print(f"Using compiler: {compiler_name}")
    
    # Setup MSVC environment if needed
    if compiler_name == "msvc":
        if not os.environ.get("INCLUDE") or not os.environ.get("LIB"):
            print("Setting up MSVC environment...")
            if not setup_msvc_environment():
                print("Warning: Failed to automatically setup MSVC environment.")
                print("Please run this script from a 'Developer Command Prompt for VS'")
                print("or 'x64 Native Tools Command Prompt for VS'")
                sys.exit(1)
    
    CC_CONFIG = COMPILERS[compiler_name]
    TARGET = os.path.join(BIN_DIR, get_library_name())
    TEST_TARGET = os.path.join(BIN_DIR, get_executable_name("test_nchess"))


def ensure_dir(directory):
    """Create directory if it doesn't exist"""
    Path(directory).mkdir(parents=True, exist_ok=True)


def clean():
    """Remove build directory"""
    if os.path.exists(BUILD_DIR):
        shutil.rmtree(BUILD_DIR)
        print(f"Cleaned {BUILD_DIR}")
    else:
        print("Nothing to clean")


def compile_source(src_file, obj_file, cflags):
    """Compile a single source file to object file"""
    ensure_dir(os.path.dirname(obj_file))
    
    if CC_CONFIG.name == "msvc":
        # MSVC uses /Fo for output file
        cmd = [CC_CONFIG.exe] + cflags + [f"/Fo{obj_file}", "/c", src_file]
    else:
        # GCC/Clang use -o
        cmd = [CC_CONFIG.exe] + cflags + ["-c", "-o", obj_file, src_file]
    
    print(f"Compiling {src_file}...")
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error compiling {src_file}:")
        if result.stdout:
            print(result.stdout)
        if result.stderr:
            print(result.stderr)
        return False
    return True


def create_archive(obj_files, target):
    """Create static library from object files"""
    ensure_dir(os.path.dirname(target))
    
    if CC_CONFIG.name == "msvc":
        # MSVC uses lib.exe with /OUT:
        cmd = [CC_CONFIG.ar_cmd] + CC_CONFIG.ar_flags + [f"/OUT:{target}"] + obj_files
    else:
        # GCC/Clang use ar
        cmd = [CC_CONFIG.ar_cmd] + CC_CONFIG.ar_flags + [target] + obj_files
    
    print(f"Creating archive {target}...")
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error creating archive:")
        if result.stdout:
            print(result.stdout)
        if result.stderr:
            print(result.stderr)
        return False
    return True


def link_executable(obj_files, libs, target, cflags):
    """Link object files and libraries into executable"""
    ensure_dir(os.path.dirname(target))
    
    if CC_CONFIG.name == "msvc":
        # MSVC uses link.exe for linking
        cmd = ["link", "/nologo", f"/OUT:{target}"] + obj_files + libs
    else:
        # GCC/Clang use -o
        cmd = [CC_CONFIG.exe] + cflags + ["-o", target] + obj_files + libs
    
    print(f"Linking executable {target}...")
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error linking executable:")
        if result.stdout:
            print(result.stdout)
        if result.stderr:
            print(result.stderr)
        return False
    return True


def build_library(debug_mode=False):
    """Build the library"""
    # Set compilation flags
    cflags = CC_CONFIG.base_flags + (CC_CONFIG.debug_flags if debug_mode else CC_CONFIG.release_flags)
    
    # Find all source files
    src_files = glob.glob(os.path.join(SRC_DIR, "*.c"))
    if not src_files:
        print(f"No source files found in {SRC_DIR}")
        return False
    
    print(f"\n--- Building library in {'DEBUG' if debug_mode else 'RELEASE'} mode ---")
    print(f"Compiler: {CC_CONFIG.name}")
    print(f"Found {len(src_files)} source files")
    
    # Compile all source files
    obj_files = []
    for src_file in src_files:
        src_name = os.path.basename(src_file)
        obj_ext = ".obj" if CC_CONFIG.name == "msvc" else ".o"
        obj_name = src_name.replace(".c", obj_ext)
        obj_file = os.path.join(OBJ_DIR, obj_name)
        
        if not compile_source(src_file, obj_file, cflags):
            return False
        
        obj_files.append(obj_file)
    
    # Create static library
    if not create_archive(obj_files, TARGET):
        return False
    
    print(f"Library created: {TARGET}")
    return True


def build_and_run_tests(debug_mode=False):
    """Build and run test executable"""
    # Check if library exists
    if not os.path.exists(TARGET):
        print(f"Error: Library not found at {TARGET}")
        print("Please build the library first with 'python build.py build'")
        return False
    
    # Set compilation flags
    cflags = CC_CONFIG.base_flags + (CC_CONFIG.debug_flags if debug_mode else CC_CONFIG.release_flags)
    
    print(f"\n--- Building tests in {'DEBUG' if debug_mode else 'RELEASE'} mode ---")
    print(f"Compiler: {CC_CONFIG.name}")
    
    # Find all test source files
    test_files = glob.glob(os.path.join(TEST_DIR, "*.c"))
    if not test_files:
        print(f"Error: No .c files found in {TEST_DIR}")
        return False
    
    print(f"Found {len(test_files)} test files")
    
    # Add include path for nchess headers
    if CC_CONFIG.name == "msvc":
        test_cflags = cflags + [f"/I{SRC_DIR}"]
    else:
        test_cflags = cflags + [f"-I{SRC_DIR}"]
    
    # Compile all test files
    obj_ext = ".obj" if CC_CONFIG.name == "msvc" else ".o"
    test_obj_files = []
    
    for test_file in test_files:
        test_name = os.path.basename(test_file)
        obj_name = test_name.replace(".c", obj_ext)
        obj_file = os.path.join(TEST_OBJ_DIR, obj_name)
        
        if not compile_source(test_file, obj_file, test_cflags):
            return False
        
        test_obj_files.append(obj_file)
    
    # Link test executable with library
    if not link_executable(test_obj_files, [TARGET], TEST_TARGET, test_cflags):
        return False
    
    print(f"Test executable created: {TEST_TARGET}")
    
    # Run tests
    print("\n--- Running tests ---")
    result = subprocess.run([TEST_TARGET], capture_output=False, text=True)
    
    if result.returncode != 0:
        print(f"\nTests failed with exit code: {result.returncode}")
        return False
    
    print("\nTests completed successfully!")
    return True


def main():
    """Main entry point"""
    if len(sys.argv) < 2:
        print("No command specified.")
        print(__doc__)
        return 1
    
    # Parse compiler option
    compiler_name = None
    commands = []
    
    for arg in sys.argv[1:]:
        if arg.startswith("--compiler="):
            compiler_name = arg.split("=", 1)[1].lower()
        else:
            commands.append(arg)
    
    # Setup compiler before running commands
    setup_compiler(compiler_name)
    
    if not commands:
        print("No command specified.")
        print(__doc__)
        return 1
    
    # Execute commands
    for command in commands:
        cmd = command.lower()
        
        if cmd == "clean":
            clean()
        
        elif cmd == "build":
            if not build_library(debug_mode=False):
                print(f"\nFailed to execute: {command}")
                return 1
        
        elif cmd == "build-debug":
            if not build_library(debug_mode=True):
                print(f"\nFailed to execute: {command}")
                return 1
        
        elif cmd == "test":
            if not build_and_run_tests(debug_mode=False):
                print(f"\nFailed to execute: {command}")
                return 1
        
        elif cmd == "test-debug":
            if not build_and_run_tests(debug_mode=True):
                print(f"\nFailed to execute: {command}")
                return 1
        
        else:
            print(f"Unknown command: {command}")
            print("\nAvailable commands: clean, build, build-debug, test, test-debug")
            return 1
    
    print("\n✓ All commands completed successfully!")
    return 0


if __name__ == "__main__":
    sys.exit(main())
