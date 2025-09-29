### Radar Grid and Sweep
#### Core: Generate Ring, Radial, and Sweep Geometry
####  Opengl: Render Geometry using GLEW

CMakeLists:<br>
`set(radar_DIR "C:/Users/Bee/Documents/CCPP/dev/libs/radar-1.0/lib/cmake/radar")`

`find_package(radar 1.0 REQUIRED)
`

`if (MINGW)
  include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${RADAR_INCLUDE_DIRS})
endif()`

`target_link_libraries(OpenGLTest
  ${OPENGL_LIBRARIES}
  GLUT::GLUT
  GLEW::GLEW
  radar::radar_core
  radar::radar_opengl
  glfw
)`

main.cpp:<br>
header:<br>
`#include <RadarGeometry.h>
#include <RadarRenderer.h>`

to enable fading:<br>
`glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);`

geometry and renderer:<br>
`RadarGeometry geo(60, 0, 5);
RadarRenderer ringRenderer, radialRenderer, sweepRenderer;
auto ringverts = geo.generateRings(5);
ringRenderer.upload(ringverts);
auto radialverts = geo.generateRadials(12);
radialRenderer.upload(radialverts);
double lastTime = glfwGetTime();`

inside the loop:<br>
`double now = glfwGetTime();
double dt = now - lastTime;
lastTime = now;
auto sweepVerts = geo.generateSweep(dt);
sweepRenderer.upload(sweepVerts);
ringRenderer.render(GL_LINE_STRIP);
radialRenderer.render(GL_LINES);
sweepRenderer.render(GL_TRIANGLE_FAN);`

use main shader before drawing as the radar using its own shader.