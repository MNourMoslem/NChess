import os
import sys
from setuptools import setup, find_packages, Extension
from setuptools.command.build_ext import build_ext

# Paths
PYTHON_SRC = "NChess/core/src"
C_SRC = "c-nchess/nchess"

def find_c_files(directory):
    """Find all .c files in a directory."""
    c_files = []
    for f in os.listdir(directory):
        if f.endswith('.c'):
            c_files.append(os.path.join(directory, f))
    return c_files

# Detect compiler and platform
if os.name == "nt":  # Windows (MSVC)
    extra_compile_args = [
        "/O2", "/Oi", "/Ot", "/GL",  # Max Optimization
        "/W4",  # Warning level 4 (not /Wall which is too noisy)
        "/permissive-",  # Standards conformance
        "/wd4996", "/wd4820", "/wd4710", "/wd4711", "/wd5045", "/wd6001",
        "/wd4115", "/wd4204", "/wd4100", "/wd4255",  # Disable noisy warnings
        "/wd4668", "/wd4200", "/wd4127",  # Python/numpy header warnings
        "/wd4191",  # Unsafe function pointer conversions
        "/Zi", "/FC",  # Debug info
    ]
    extra_link_args = ["/LTCG"]  # Link-time code generation
else:  # Linux/macOS (GCC/Clang)
    extra_compile_args = [
        "-O3", "-Wall", "-Wextra",
        "-fPIC", "-std=c99"
    ]
    extra_link_args = []

# Define the extension module
nchess_core = Extension(
    'NChess.core.nchess_core',  # Module path
    sources=find_c_files(PYTHON_SRC) + find_c_files(C_SRC),
    include_dirs=[
        PYTHON_SRC,
        C_SRC,
    ],
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
)

# Custom build command to show where the extension is built
class CustomBuildExt(build_ext):
    def run(self):
        super().run()
        print("\n" + "="*60)
        print("BUILD SUCCESSFUL!")
        print("="*60)
        for ext in self.extensions:
            print(f"Extension: {ext.name}")
            print(f"Built at: {self.get_ext_fullpath(ext.name)}")
        print("="*60)
        print("\nTo test the extension:")
        print("  python -c \"from nchess.core import nchess_core; print('Success!')\"")
        print("\nOr run a test script:")
        print("  python test_import.py")
        print("="*60 + "\n")

setup(
    name='nchess',
    version='1.2.1',
    packages=find_packages(include=['nchess', 'nchess.*']),
    ext_modules=[nchess_core],
    cmdclass={'build_ext': CustomBuildExt},
    include_package_data=True,
    install_requires=[
        'numpy>=1.18.0',
    ],
    setup_requires=[
        'numpy>=1.18.0',
    ],
    author='MNMoslem',
    author_email='normoslem256@gmail.com',
    description='High-performance chess library written in C',
    long_description=open('README.md', encoding='utf-8').read(),
    long_description_content_type='text/markdown',
    url='https://github.com/MNourMoslem/NChess',
    classifiers=[
        'Programming Language :: Python :: 3',
        'Programming Language :: C',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
        'Topic :: Games/Entertainment :: Board Games',
    ],
    python_requires='>=3.7',
    license='MIT',
    zip_safe=False,
)
