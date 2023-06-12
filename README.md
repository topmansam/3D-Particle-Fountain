# 3D-Particle-Fountain
This is a C++ project that simulates a 3D particle fountain. The program generates particles that rise up from a central point and display various behaviors and effects.

## Features
The particle fountain project includes the following features:

- Color/Size: Each particle is assigned a random color and size.
- Spark: Sparks are generated when particles collide with each other.
- Spray Mode: High spray mode increases the randomness of particle velocities.
- Collision: Particles collide with each other and with the ground.
- Textures: The fountain is textured using an image file.

## Prerequisites
Before running the program, make sure you have the following installed:

FreeGLUT: OpenGL Utility Toolkit for creating windows, handling input, and rendering.

## Compilation and Execution
To compile and run the particle fountain program, follow these steps:

1. Set up the necessary libraries (FreeGLUT) and include them in your project.
2. Compile the C++ code using a C++ compiler. For example, using g++:
3. Execute the program:

## Controls
The following controls are available while running the program:

F: Toggle firing mode (change the pattern in which particles are generated).
R: Toggle random speed for particles.
S: Toggle spark generation on particle collision.
P: Toggle spray mode for particle velocities.
C: Toggle shading mode between flat and smooth.
B: Toggle backface culling.
1-4: Change the render mode for particles.

## Additional Information
- The maximum number of particles that can be rendered at once is set to 100.
- The program simulates gravity and applies friction to particles when they collide with the ground or other particles.
- The particles are initialized with random positions, velocities, colors, and sizes.
- The fountain is rendered as a textured triangle.
- Sparks are generated when particles collide. Sparks have a random color and a limited lifetime.
- Feel free to explore and modify the code to further enhance the particle fountain simulation.
