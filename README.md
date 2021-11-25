# isosurf-rendering

Required libraries : openGL (mesa), glfw, glew, glm.

To build the project (out of source) : go to /build, then 
cmake ../ && make && ./test
To build in release mode (i.e. with optimizations turned on),
add the -DCMAKE_BUILD_TYPE=Release option to cmake.

# Sources:

Gregorski, Benjamin, Mark Duchaineau, Peter Lindstrom, Valerio Pascucci, and Kenneth I. Joy. "Interactive view-dependent rendering of large isosurfaces." In Proceedings of the conference on Visualization'02, pp. 475-484. IEEE Computer Society, 2002.

Scholz, Manuel, Jan Bender, and Carsten Dachsbacher. "Level of Detail for Real-Time Volumetric Terrain Rendering." In VMV, pp. 211-218. 2013.

K. Weiss and L. De Floriani. "Simplex and Diamond Hierarchies: Models and Applications".