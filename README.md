# FABRIK
A simple 2D inverse kinematics simulation using the FABRIK (Forward And Backward Reaching Inverse Kinematics) algorithm, implemented in modern OpenGL. Simulates a salamander-like creature with articulated joints and leg movement that reacts to mouse input.
+ FABRIK algorithm for leg IK solving.
+ Torque-based body when legs step.
+ Adjustable joint count, leg positions, widths, and stepping behavior.

## Showcase
<p align="center">
  <img src="https://github.com/mingthang/FABRIK/blob/master/res/fabrikresult.gif" width="75%" />
</p>
<p align="center">
  <img src="https://github.com/mingthang/FABRIK/blob/master/res/fabrik.gif" width="75%" />
</p>

## Third Party Libraries Used
### GLFW
- **Version**: 3.4
- **Website**: [https://www.glfw.org/](https://www.glfw.org/)
- **Description**: GLFW library for creating windows, contexts, and handling input.

### GLAD
- **Version**: 0.1.36
- **Website**: [https://glad.dav1d.de/](https://glad.dav1d.de/)
- **Description**: OpenGL loader for managing OpenGL function pointers.

### GLM
- **Version**: 1.0.1
- **Repository**: [https://github.com/g-truc/glm](https://github.com/g-truc/glm)
- **Description**: OpenGL Mathematics header-only library.
