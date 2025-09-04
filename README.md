# Pool Game Simulator
This program was written in C++ for x86_64 Linux systems. It was built using OpenGL libaries, and it runs a physical simulation of a pool table. This was written for a computer graphics course, and it is a heavily modified version of an assignment from that course. 
## Credit
- The code regarding physics simulation and the shaders were written by me
- Some portion of main.cpp, specifically initializing GL, were written by my professor Dr. Daljit Singh Dhillon ([his website](https://djsingh.people.clemson.edu/))
- The project also uses helper functions written by Anton Gerdelan from his [OpenGL 4 Tutorials](https://antongerdelan.net/opengl/)
- Finally, the project uses Sean Barret's image loader ([stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h))

## System Requirements
- Will only run on an x86-64 Linux-based system
- Requires a desktop environment, or at least some kind of graphical environment in which a window can be opened

## How to Install
- Clone the repository or download the ZIP and extract into new directory
- You'll need to download and install dependencies
  - Most of these are default in many Linux distros
	- libgl1
	- libx11-6
	- libxxf86vm1
	- libxrandr2
	- libxi6
	- libstdc++6
	- libgcc-s1
	- libc6
	- libglx0
	- libgl1-mesa-glx
	- libxcb1
	- libxext6
	- libxrender1
	- libxau6
	- libxdmcp6
	- libbsd0
	- libmd0

## How to run
- The repo contains a pre-compiled executable: main/game.out
- To run the program, navigate to the main folder, and type ```./game.out```
## How to compile
- If you wish to recompile the program, you can use the Makefile
- While in the 'main' directory, type "make"
