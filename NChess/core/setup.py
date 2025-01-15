import os
from setuptools import setup, Extension
import numpy
from setuptools.command.build_ext import build_ext

# Get current directory and build path
this_dir = os.path.dirname(os.path.abspath(__file__))
build_path = os.path.join(this_dir, 'build')

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

# Custom build_ext command to override the output directory
class CustomBuildExtCommand(build_ext):
    def finalize_options(self):
        super().finalize_options()
        # Set the build directory for output files
        self.build_lib = build_path

# Setup configuration
setup(
    name='nchess',
    version='1.0',
    description='Python wrapper for NChess C library',
    ext_modules=[nchess_module],
    install_requires=['numpy'],  # Ensure NumPy is installed
    cmdclass={
        'build_ext': CustomBuildExtCommand,  # Use the custom command
    },
    options={
        'build': {
            'build_base': build_path,  # Base build directory
        }
    }
)
