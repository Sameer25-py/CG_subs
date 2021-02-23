### Setup

We will be using CMake to build the assignments. If you don't have CMake (version >= 2.8) on your personal computer, you can install it using apt-get / apt on Linux or Macports/Homebrew on OS X. This command installs cmake and opengl dependencies for all future assignments (replace with brew for MacOS):
```!bash
sudo apt-get install cmake libx11-dev xorg-dev libglu1-mesa-dev freeglut3-dev libglew1.5 libglew1.5-dev libglu1-mesa libglu1-mesa-dev libgl1-mesa-glx libgl1-mesa-dev
```

To build the code, start in the folder that GitHub made or that was created when you unzipped the download. Run

    mkdir build
to create a build directory, followed by

    cd build
to enter the build directory. Then

    cmake ..
to have CMake generate the appropriate Makefiles for your system, then

    make
to make the executable, which will be deposited in the build directory.
    
    
Sample output for successful build:
```
[  9%] Built target glew
[ 42%] Built target glfw
[ 80%] Built target CGL
[100%] Built target draw
```