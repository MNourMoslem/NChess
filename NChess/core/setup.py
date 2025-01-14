import os
from setuptools import setup, Extension
import numpy

this_dir = os.getcwd()

# Function to collect all .c files in the src directory
def find_c_files(directory):
    return [os.path.join(directory, f) for f in os.listdir(directory) if f.endswith('.c')]

# Define the extension module
nchess_module = Extension(
    'nchess',  # Name of the module
    sources=[*find_c_files(f'{this_dir}/src'), *find_c_files(this_dir)],  # Programmatically find all C source files
    include_dirs=[
        'src',  # Add include directories if needed
        numpy.get_include(),  # Include NumPy headers
    ],
)

# Setup configuration
setup(
    name='nchess',
    version='1.0',
    description='Python wrapper for NChess C library',
    ext_modules=[nchess_module],
    install_requires=['numpy'],  # Ensure NumPy is installed
)
