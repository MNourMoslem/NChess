import os
import sys
import numpy
from setuptools import setup, find_packages, Extension

python_src = "nchess/core/src"
c_src = "c-nchess"

def find_c_files(directory):
    return [os.path.join(directory, f) for f in os.listdir(directory) if f.endswith('.c')]

# Detect compiler and platform
if os.name == "nt":  # Windows (MSVC)
    extra_compile_args = [
        "/O2", "/Oi", "/Ot", "/GL",  # Max Optimization
        "/Wall", "/WX", "/sdl",  # All Warnings
        "/permissive-", "/wd4996", "/wd4820", "/wd4710", "/wd4711", "/wd5045", "/wd6001", "/wd4115", "/wd4204", "/wd4100",  # Ignore noisy warnings
        "/Zi", "/FC", "/wd4255",  # Disable warning C4255
        "/F40000",  # Increase the stack size to 40 MB (you can adjust as necessary)
    ]
else:  # Linux/macOS (GCC/Clang)
    extra_compile_args = [
        "-Wsign-compare", "-DNDEBUG", "-g", "-fwrapv", "-O2", "-Wall",
        "-g", "-fstack-protector-strong", "-Wformat", "-Werror=format-security",
        "-fPIC"
    ]

nchess_module = Extension(
    'nchess.core.nchess_core',
    sources=find_c_files(python_src) + find_c_files(c_src + "/nchess"),
    include_dirs=[python_src, c_src, numpy.get_include()],
    extra_compile_args=extra_compile_args,
)

setup(
    name='nchess',
    version='1.2',
    ext_modules=[nchess_module],
    packages=find_packages(),
    include_package_data=True,
    install_requires=['numpy>=1.18.0', "wheel", "setuptools>=42"],
    author='MNMoslem',
    author_email='normoslem256@gmail.com',
    description='chess library written in C',
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    url='https://github.com/MNourMoslem/NChess',
    classifiers=[
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
    ],
    python_requires='>=3.7',
    license=open('LICENSE').read(),
)
