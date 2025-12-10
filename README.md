# Computer Graphics Project - Village Fair Scene 🎡

A complete animated village fair scene featuring a Ferris wheel, carousel, swing ride, fireworks, and day/night cycle using OpenGL and GLUT.

![Village Fair](screenshot.png)

## ✨ Features

### 🎢 Rides & Attractions
- 🎡 **Ferris Wheel** - Rotating with 8 cabins and colorful lights at night
- 🎠 **Carousel** - 6 animated horses with up/down motion and platform lights
- 🎢 **Swing Ride** - Pendulum physics with realistic motion
- 🍔 **Food Cart** - Animated vendor with waving arm
- 🎪 **Food & Game Stalls** - 3 additional stalls (Balloon Game, Popcorn, Ring Toss)

### 🌅 Environment & Atmosphere
- 🌞 **Day/Night Cycle** - Automatic smooth transition between day and night
- ☀️ **Sun** - Moving across the sky during daytime
- 🌙 **Moon** - Visible at night with crater details
- ⭐ **Twinkling Stars** - 100 stars that appear and twinkle at night
- ☁️ **Animated Clouds** - 6 clouds drifting across the sky
- 🌳 **Trees** - 8 trees on both edges providing depth and framing
- 🛤️ **Pathways** - Detailed dirt paths connecting all attractions
- 🌱 **Textured Ground** - Grass patches and decorative elements
- 💡 **Dynamic Lighting** - All rides light up automatically at night
- 💡 **String Lights** - 8 strings of twinkling fairy lights throughout the fair

### 🎪 Interactive Elements
- 🎆 **Fireworks** - Particle system with colorful explosions (Press F)
- 🎪 **Entrance Gate** - Decorated archway entrance
- 🪧 **Village Fair Sign** - Illuminated sign with blinking multicolor lights
- 👥 **Walking Crowd** - 8 animated people strolling around the fair
- 🎈 **Floating Balloons** - 8 colorful balloons drifting across the scene
- 🚩 **Waving Flags** - 4 flags with realistic wave animation

## 🎮 Controls

| Key | Action |
|-----|--------|
| **F** | Launch fireworks |
| **N** | Toggle day/night transition |
| **D** | Force daytime |
| **T** | Force nighttime |
| **SPACE** | Resume automatic day/night cycle |
| **ESC** | Exit program |

---

## 🖥️ Installation & Setup

### For Linux (Ubuntu/Debian)

#### 1. Install Dependencies
```bash
sudo apt-get update
sudo apt-get install freeglut3-dev mesa-common-dev libglu1-mesa-dev g++ make
```

#### 2. Build the Project
```bash
make
```

#### 3. Run the Project
```bash
./ferris_wheel
```

### For Other Linux Distributions

**Fedora/RHEL:**
```bash
sudo dnf install freeglut-devel mesa-libGLU-devel gcc-c++ make
make
./ferris_wheel
```

**Arch Linux:**
```bash
sudo pacman -S freeglut mesa glu base-devel
make
./ferris_wheel
```

---

### For Windows

#### Option 1: Using Visual Studio (Recommended)

1. **Install Visual Studio** (Community Edition is free)
   - Download from: https://visualstudio.microsoft.com/
   - During installation, select "Desktop development with C++"

2. **Install FreeGLUT**
   - Download pre-built binaries from: https://www.transmissionzero.co.uk/software/freeglut-devel/
   - Extract the zip file

3. **Set up the project:**
   - Create a new "Empty C++ Project" in Visual Studio
   - Copy `main.cpp` to your project folder
   - Right-click project → Properties

4. **Configure Include Directories:**
   - C/C++ → General → Additional Include Directories
   - Add: `path\to\freeglut\include`

5. **Configure Library Directories:**
   - Linker → General → Additional Library Directories
   - Add: `path\to\freeglut\lib\x64` (or x86 for 32-bit)

6. **Link Libraries:**
   - Linker → Input → Additional Dependencies
   - Add: `freeglut.lib;opengl32.lib;glu32.lib;`

7. **Copy DLLs:**
   - Copy `freeglut.dll` from `freeglut\bin\x64` to your project's output folder

8. **Build and Run:**
   - Press F5 or Ctrl+F5

#### Option 2: Using MinGW

1. **Install MinGW-w64:**
   - Download from: https://www.mingw-w64.org/
   - Or use MSYS2: https://www.msys2.org/

2. **Install FreeGLUT via MSYS2:**
   ```bash
   pacman -S mingw-w64-x86_64-freeglut mingw-w64-x86_64-gcc
   ```

3. **Compile:**
   ```bash
   g++ -o ferris_wheel.exe main.cpp -lfreeglut -lopengl32 -lglu32 -Wall -std=c++11
   ```

4. **Run:**
   ```bash
   ./ferris_wheel.exe
   ```

#### Option 3: Using Code::Blocks

1. **Install Code::Blocks** with MinGW: http://www.codeblocks.org/
2. **Install FreeGLUT** (see Option 1 step 2)
3. Create new project → Glut project
4. Copy `main.cpp` content to your main file
5. Build and Run (F9)

---

## 📁 Project Structure

```
CG-Project/
├── main.cpp          # Main source code
├── Makefile          # Linux/Unix build configuration
├── README.md         # This file
├── .gitignore        # Git ignore file
└── ferris_wheel      # Compiled executable (Linux)
```

## 🎨 Code Features

- **Particle System** - Advanced fireworks with 50 particles per explosion
- **State Management** - Clean day/night cycle with smooth transitions
- **Animation System** - Multiple independent animations running at 60 FPS
- **Modular Design** - Separated drawing functions for each component
- **Color Management** - Dynamic brightness adjustment based on time of day

## 🛠️ Customization

You can modify these values in `main.cpp`:

```cpp
// Animation speeds
ferrisWheelAngle += 0.5f;     // Ferris wheel rotation speed
carouselAngle += 0.8f;        // Carousel rotation speed
transitionSpeed = 0.002f;     // Day/night cycle speed

// Firework settings
const int MAX_FIREWORKS = 10;  // Maximum simultaneous fireworks
float nextFireworkTime = 100.0f; // Auto-launch frequency

// Colors
glClearColor(r, g, b, 1.0f);  // Sky color (in getSkyColor function)
```

## 📸 Screenshots

**Daytime Scene:**
- Bright blue sky with sun
- Colorful rides and attractions
- Animated people walking

**Nighttime Scene:**
- Dark sky with moon and twinkling stars
- All rides illuminated with colorful lights
- Blinking sign lights
- Fireworks displays

## 🎓 Educational Value

This project demonstrates:
- **Computer Graphics Fundamentals** - 2D transformations, rotations, translations
- **Animation Techniques** - Timer-based animation, smooth transitions
- **Particle Systems** - Physics simulation for fireworks
- **State Management** - Day/night cycle, multiple animation states
- **User Interaction** - Keyboard input handling
- **Modular Programming** - Clean separation of rendering functions

## 📝 License

This is an educational project for computer graphics coursework.

## 👨‍💻 Author

**Mohimenul Islam**  
Computer Graphics Course Project

---

## 🚀 Quick Start

**Linux:**
```bash
make && ./ferris_wheel
```

**Windows (MinGW):**
```bash
g++ -o ferris_wheel.exe main.cpp -lfreeglut -lopengl32 -lglu32 -Wall -std=c++11
ferris_wheel.exe
```

**Enjoy the fair!** 🎡🎆✨
- ✅ Support structure
- ✅ Ground/grass area
- ✅ Village fair stalls/buildings
- ✅ Sky background

## Customization Ideas

You can enhance this project by adding:

1. **More Details**:
   - Trees and clouds
   - More stalls and decorations
   - Flags and banners
   - People/crowds

2. **Animation**:
   - Moving clouds
   - Swinging flags
   - Blinking lights on ferris wheel

3. **Interactive Elements**:
   - Mouse controls to speed up/slow down
   - Day/night cycle
   - Camera zoom in/out

4. **Additional Rides**:
   - Carousel
   - Swing rides
   - Food stalls

## Development Tips

- Modify colors in the `glColor3f()` calls
- Adjust animation speed in the `timer()` function
- Change window size by modifying WIDTH and HEIGHT constants
- Add more objects by creating new draw functions

## Troubleshooting

**If compilation fails:**
- Make sure OpenGL libraries are installed
- Check that you have g++ compiler: `g++ --version`

**If window doesn't appear:**
- Check if X11 forwarding is enabled (if using SSH)
- Try running with: `DISPLAY=:0 ./ferris_wheel`

## Clean Build

```bash
make clean
```

This removes all compiled object files and executables.
