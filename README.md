# NOTE: Development of pNAB moved to https://github.com/GT-NucleicAcids/pnab

# proto-Nucleic Acid Builder (pNAB)
## Try the Package: [![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/alenaizan/pnab/master?urlpath=%2Fapps%2Fbinder%2Fdriver.ipynb)
## Read the documentation: [![docs v1.1.4](https://img.shields.io/badge/docs-v1.1.4-blue)](https://proto-nucleic-acid-builder-v1-1-4.netlify.app/html/index.html)

[![Build Status](https://travis-ci.com/alenaizan/pnab.svg?branch=master)](https://travis-ci.com/alenaizan/pnab)
[![Build status](https://ci.appveyor.com/api/projects/status/9uew76hr6ftlwso4/branch/master?svg=true)](https://ci.appveyor.com/project/alenaizan/pnab/branch/master)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/alenaizan/pnab.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/alenaizan/pnab/context:cpp)
[![Language grade: Python](https://img.shields.io/lgtm/grade/python/g/alenaizan/pnab.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/alenaizan/pnab/context:python)

## Overview
The proto-Nucleic Acid Builder is a program for modeling the 3D strucutres of DNA, RNA, and nucleic acid analogs. Nucleic acids with alternative backbones or nucleobases can be constructed by the program by supplying the 3D structure of isolated backbones or nucleobases. The program can perform a helical parameter search and backbone conformation search and find reasonable nucleic acid structures. Geometric and energetic criteria are used to evaluate candidate structures. The program is written in C++ and Python, and has a graphical user interface. The program is available for the Linux, MacOS, and Windows platforms. 
![image](Doxygen/images/output.png)

## Installing the Pre-compiled Package Using the conda package manager
To install the conda package, first install miniconda/anaconda. Then create a new environment for pNAB,
```
conda create -n pnab -c alenaizan -c conda-forge pnab
conda activate pnab
```
## Compiling the Package
The files `install.sh` and `install.bat` provide example scripts for compiling the package for the Linux and Windows platforms. 

## Documentations:
Development version: [![docs latest](https://img.shields.io/badge/docs-latest-blue)](https://alenaizan.github.io/pnab/html/index.html)

Latest release: [![docs v1.1.4](https://img.shields.io/badge/docs-v1.1.4-blue)](https://proto-nucleic-acid-builder-v1-1-4.netlify.app/html/index.html)
