# Doppler Effect Simulation

by Dr. Richard D. Kaminsky &nbsp; 3/26/2025 &ndash; 4/8/2025

Project type: C++ / Desktop Qt 6.8.3 MinGW 64-bit

## Overview

This is an animation of an ambulance with its siren on going past
a person. The siren is synthesized as a pure tone (a sine wave).
Its frequency varies according to the *Doppler effect* and volume
varies according to the *inverse square law* (reciprocal of the
distance squared).

## How to Deploy

1. Build this project
2. Create an empty directory
3. From the build directory copy the executable file `doppler_ed.exe`
   and subdirectory `img` to the empty directory
4. In a command window, change to that directory
5. Run `C:\Qt\6.8.3\mingw_64\bin\windeployqt doppler_ed.exe`

This will add the necessary dynamic link libraries and other
files to the directory to allow running the application.
