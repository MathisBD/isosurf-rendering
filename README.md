# isosurf-rendering

# Building 

Required libraries : openGL, glfw, glew, glm.

To build the project (out of source) : go to ./build (create it if necessary), then :
cmake -DCMAKE_BUILD_TYPE=mode .. && make && ./test
CMake supports different build modes :
    - Debug : no optimizations
    - Release : optimizations but no debug symbols
    - RelWithDebInfo : optimizations + debug symbols (useful for profiling)

# Running

Use the arrow keys to move left/right and forward/backwards.
To move up : right SHIFT.
To move down : right CTRL.
To look around use hold down the right button of the mouse.

The color of a triangle indicates how small it is.
# Sources:

Gregorski, Benjamin, Mark Duchaineau, Peter Lindstrom, Valerio Pascucci, and Kenneth I. Joy. "Interactive view-dependent rendering of large isosurfaces." In Proceedings of the conference on Visualization'02, pp. 475-484. IEEE Computer Society, 2002.

Scholz, Manuel, Jan Bender, and Carsten Dachsbacher. "Level of Detail for Real-Time Volumetric Terrain Rendering." In VMV, pp. 211-218. 2013.

K. Weiss and L. De Floriani. "Simplex and Diamond Hierarchies: Models and Applications".


# Diamond labeling scheme
I use the indexing scheme described in "Simplex and Diamond Hierarchies: Models and Applications", section "4.4. RSB hierarchies and fully-subdivided cubes", introduced by Weiss and De Floriani.
For a diamond of depth d, we define:
    it's level [d / 3]
    it's phase d % 3

If we constrain all diamonds and tetrahedrons to have integer coordinate vertices (i.e. we work on a grid of size 1+2**maxDepth), we can get all useful info for a diamond using only its center vertex. 

For a given vertex (x, y, z), there is at most one diamond at this vertex.
We write x, y and z as binary coordinates :
    x = ox1...oxn tx1 tx2 0...0
    y = oy1...oyn ty1 ty2 0...0
    x = oz1...ozn tz1 tz2 0...0
The scale s is the largest number of trailing zeros common to x, y and z. It gives the level of the diamond in the hierarchy : level = maxDepth - scale (+ constant maybe).
The origin (bottom-left vertex) of the supercube containing (x,y,z) is (ox,oy,oz).
The type (tx,ty,tz) gives the offset of the diamond within its supercube. The type t can't have all three coordinates even. The number of even/odd coordinates of t gives the phase of the diamond, as follows :
    Number of coords : odd (t2=1)  even (t2=0)       
    Phase 0 :          3           0
    Phase 1 :          2           1
    Phase 2 :          1           2

I should probably extract the phase, level, and direction at the same time.

(In all the following : when we modify o, t or s, we carry out the modifications
to the rest of x/y/z. Example : if tx=0b11 and we add 1 to tx, tx will become 0b00
and we add 1 to ox.) 

# Diamond Children
To get the children of a diamond :
    Phase 0 : add or substract 1 from exactly one odd coord of t (6 children).
    Phase 1 : add or substract 1 from exactly one odd coord of t (4 children).
    Phase 2 : add or substract 1 to all three coords of (t2 s1) (8 children).

# Diamond Parents
To get the parents of a diamond :
    Phase 0 : add or substract 1 to all three coords of t, taking care that the new version of (on t1) needs to have exactly one odd coordinate (3 parents).
    Phase 1 : add or substract 1 from exactly one even coord of t (2 parents).
    Phase 2 : add or substract 1 from exactly one even coord of t (4 parents).



