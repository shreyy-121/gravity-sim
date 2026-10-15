# Solar System Gravity Simulation

A 2D simulation of the Sun and the eight planets, written in C++ with GLFW, GLEW and legacy (fixed-function) OpenGL.

Every body pulls on every other body with Newtonian gravity. The orbits you see are not animated along a fixed path; they come purely from that pull.

<!-- Add a screenshot: ![screenshot](docs/screenshot.png) -->

## Features

- The Sun plus Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus and Neptune
- Real N-body gravity: every body attracts every other body, not just the Sun attracting the planets
- Colours, relative masses and size order based on the real solar system
- Stable orbits over hours of simulated time
- One source file, no assets

## Requirements

- A C++11 (or newer) compiler
- [GLFW 3](https://www.glfw.org/) and [GLEW](http://glew.sourceforge.net/)
- A GPU/driver that supports legacy OpenGL (the code uses `glBegin`/`glEnd` and `glOrtho`)

## Build and run

The commands below assume the source file is called `main.cpp`. Adjust names and paths to your setup.

**Linux (Debian/Ubuntu)**

```bash
sudo apt install build-essential libglfw3-dev libglew-dev
g++ -std=c++11 main.cpp -o solar_system -lglfw -lGLEW -lGL
./solar_system
```

**Windows (MSYS2, MinGW-w64 shell)**

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-glfw mingw-w64-x86_64-glew
g++ -std=c++11 main.cpp -o solar_system.exe -lglfw3 -lglew32 -lopengl32 -lgdi32
./solar_system.exe
```

**Visual Studio:** add the GLFW and GLEW include and library folders to the project, and link `opengl32.lib`, `glfw3.lib` and `glew32.lib`.

Close the window to quit. There are no other controls yet.

## How it works

### Gravity

For each planet `p` and every other body `other`:

```text
dx = other.x - p.x
dy = other.y - p.y
r  = sqrt(dx*dx + dy*dy)
a  = G * other.m / (r*r + 25)

ax += a * dx / r
ay += a * dy / r
```

- `G = 0.6`.
- The `+ 25` is a softening term. It stops the pull from blowing up when two bodies get very close.
- A planet's own mass cancels out (`F = G*m1*m2/r^2`, then `a = F/m1`), so only the other body's mass decides how hard it pulls.

### Update step (once per frame)

1. **Pull pass:** for every body, add up the pull from all the others using the current positions, then nudge its velocity: `v += a * dt`.
2. **Move pass:** only after every velocity is updated, move every body: `x += v * dt`, then draw it.

This is semi-implicit (symplectic) Euler integration. The two separate passes matter. If each planet moves as soon as its own pull is summed, the next planet sees the moved one, so the pulls between a pair are no longer equal and opposite. Orbits then slowly decay; in testing, Earth spiralled into the Sun after about 9 minutes even with only the Sun and Earth in the system. With two passes the orbits stay stable.

### Starting conditions

- Each planet starts on a circle around the Sun with the speed of a circular orbit under the formula above, pointing sideways to the line towards the Sun:

  ```text
  v  = sqrt(G * M * r / (r*r + 25))     (M = 4000, the Sun's mass)
  vx = -v * y / r
  vy =  v * x / r
  ```

- The Sun's starting velocity is set to cancel the planets' total momentum, so the whole system does not drift off the screen.

## Not to scale

The real solar system does not fit in an 800x600 window, so the model makes some trade-offs:

| What | How it is done |
|---|---|
| Distances | Compressed to fit the window. The order and the gap between Mars and Jupiter are kept. In reality Neptune is about 77x farther out than Mercury. |
| Sizes | Exaggerated so planets are visible. The size order is right (Jupiter largest, Mercury smallest). |
| Masses | The real ratios between the planets (Jupiter is about 318x Earth), scaled so the Sun is 4000, then reduced a further 10x. |
| Speeds | Circular-orbit speeds for this simulation's gravity formula, not real orbital speeds. |
| Time | One lap takes about 2 s for Mercury, 5 s for Earth and 20 s for Neptune at `dt = 0.5` and 60 fps. |

The extra 10x mass reduction is there because, at the full real ratios, Jupiter and Saturn sit too close together in the squeezed layout and scatter each other within a few minutes.

### Planet table

| Body | Orbit radius (px) | Drawn radius (px) | Mass (Sun = 4000) |
|---|---|---|---|
| Sun | - | 30 | 4000 |
| Mercury | 55 | 3 | 0.000066 |
| Venus | 80 | 5 | 0.00098 |
| Earth | 105 | 5 | 0.0012 |
| Mars | 128 | 4 | 0.00013 |
| Jupiter | 175 | 11 | 0.382 |
| Saturn | 215 | 9 | 0.114 |
| Uranus | 250 | 7 | 0.0175 |
| Neptune | 280 | 7 | 0.0206 |

## Code layout

Everything lives in one file:

- `Engine`: creates the window and OpenGL context, and sets up the screen each frame (clear, and put `(0, 0)` in the middle)
- `drawCircle`: draws a filled circle as a triangle fan, since OpenGL only draws points, lines and triangles
- `planet`: position, velocity, radius, mass and colour of one body
- `planets`: the table with the Sun and the eight planets
- `main`: cancels the Sun's momentum, then runs the loop (pull pass, move and draw pass, swap buffers)

## Tweaking

- **`dt`:** the time step. Smaller means slower and smoother motion; larger means faster and coarser orbits (Mercury gets visibly wobbly around `dt = 1`).
- **`G` and the softening term (`25`):** change the strength of gravity and how gently close passes are handled. If you change `G` or the Sun's mass, recompute the starting speeds with the formula above.
- **Window size:** `WIDTH` and `HEIGHT` in `Engine`.
- **Adding a planet:** pick an orbit radius `r` and an angle `a`, then use `x = r*cos(a)`, `y = r*sin(a)`, `v = sqrt(G*M*r/(r*r + 25))`, `vx = -v*sin(a)`, `vy = v*cos(a)`. Keep orbits well separated from their neighbours and masses small compared with the Sun, because heavy neighbours scatter each other.

## Stability

With the values in this repository and `dt` set to 0.5 or 1.0, a headless copy of the update loop ran for 3 simulated hours at 60 fps with no collisions and no planets leaving the window. Orbits stayed within roughly 1% to 6% of their starting radius.

## Known limitations

- The simulation advances by a fixed `dt` every frame, so its speed depends on the frame rate. Enable vsync with `glfwSwapInterval(1)` for a consistent speed.
- The viewport is set once at startup, so resizing or maximizing the window does not rescale the picture.
- It is 2D only, and bodies have no collisions; planets pass straight through each other.
- Pluto and the moons are not included.
- Legacy OpenGL is deprecated and may be unavailable in some environments (for example, core-profile-only contexts).

## Ideas for next steps

- Orbit trails
- Pause and speed controls on the keyboard
- Zoom and pan
- Moons and an asteroid belt
- Modern OpenGL (shaders and vertex buffers) instead of `glBegin`/`glEnd`
- A more accurate integrator, such as RK4
