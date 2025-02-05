import os
import numpy
from setuptools import setup, find_packages, Extension

this_dir = '.'
core_dir = os.path.join(this_dir, "NChess/core")
build_path = os.path.join(core_dir, "build")

def find_c_files(directory):
    c_files = [os.path.join(directory, f) for f in os.listdir(directory) if f.endswith('.c')]
    return c_files

nchess_module = Extension(
    'NChess.core.nchess',
    sources=[
        *find_c_files(os.path.join(core_dir, 'src')),
        *find_c_files(os.path.join(core_dir, 'src/nchess')),
    ],
    include_dirs=[
        "NChess/core/src",
        "NChess/core/src/nchess",
        numpy.get_include(),
    ],
)

setup(
    name='NChess',
    version='1.0.24',
    ext_modules=[
            nchess_module
        ],
    packages=find_packages(),
    include_package_data=True,
    install_requires=[
        'numpy>=1.18.0', "wheel", "setuptools>=42"
    ],
    author='MNMoslem',
    author_email='normoslem256@gmail.com',
    description='chess library written in c',
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
