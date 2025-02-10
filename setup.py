import os
import numpy
from setuptools import setup, find_packages, Extension
import setuptools as st

python_src = "nchess/core/src"
c_src = "c-nchess"

def find_c_files(directory):
    c_files = [os.path.join(directory, f) for f in os.listdir(directory) if f.endswith('.c')]
    return c_files

nchess_module = Extension(
    'nchess.core.nchess_core',
    sources = find_c_files(python_src) + find_c_files(c_src + "/nchess"),
    include_dirs=[
        python_src,
        c_src,
        numpy.get_include(),
    ],
)

setup(
    name='nchess',
    version='1.1.11',
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
