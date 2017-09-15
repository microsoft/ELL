'''
Setup for Embedded Learning Library
'''
import os.path
from setuptools import setup


here = os.path.abspath(os.path.dirname(__file__))

setup(
    name='ell',
    version='0.0.1',

    author='the ELL team',
    author_email='ellteam@outlook.com',

    description='Microsoft Embedded Learning Library',
    long_description='Microsoft Embedded Learning Library',
    url='https://github.com/Microsoft/ELL',

    license='MIT',

    platforms=['win32', 'win64', 'linux', 'osx'],
    classifiers=[
        'Development Status :: 4 - Beta',

        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',

        'License :: OSI Approved :: MIT License',

        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
    ],

    keywords='machine learning toolkit for microcontrollers and other small devices',

    packages=['ell', 'ell.util', 'ell.vision'],
    
    include_package_data=True,
    install_requires=[], # handled in meta.yaml
)