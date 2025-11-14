# Cyberpunk Cannon Shooter

A C++/SFML port of a Next.js Brick Breaker game, transformed into a Cyberpunk Space Cannon Shooter.

## 🎮 Game Description

Control a cannon at the bottom of the screen and shoot energy projectiles upward at descending enemy blocks. Projectiles bounce off walls, blocks, and the cannon itself. Clear waves of blocks to progress through increasingly difficult levels.

## 🛠️ Building the Project

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- SFML 2.5 or higher

### Building

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake ..

# Build the project
cmake --build .

# Run the game
./bin/CyberpunkCannonShooter
```

### macOS (using Homebrew)

```bash
# Install SFML
brew install sfml

# Build and run
mkdir build && cd build
cmake ..
make
./bin/CyberpunkCannonShooter
```

### Linux (Ubuntu/Debian)

```bash
# Install SFML
sudo apt-get install libsfml-dev

# Build and run
mkdir build && cd build
cmake ..
make
./bin/CyberpunkCannonShooter
```

### Windows

1. Install SFML from [sfml-dev.org](https://www.sfml-dev.org/download.php)
2. Set SFML_ROOT environment variable or update CMakeLists.txt
3. Use CMake GUI or command line to generate project files
4. Build with Visual Studio or your preferred IDE

## 📁 Project Structure

```
/
├── src/              # Source files
│   ├── core/        # Core game engine
│   ├── entities/    # Game entities (Cannon, Projectile, Block)
│   ├── managers/    # Manager classes (Audio, Asset, Level)
│   └── ui/          # UI components
├── include/         # Header files
├── assets/          # Game assets (textures, sounds, fonts)
├── config/          # Configuration files
├── tests/           # Unit and integration tests
└── docs/            # Documentation (not in git)

```

## 🎯 Features

- **Cannon Control**: Move horizontally and shoot energy projectiles
- **Bouncing Projectiles**: Projectiles bounce off walls, blocks, and cannon
- **Level Progression**: Increasing difficulty with more blocks and higher durability
- **Varying Blocks**: Different shapes, colors, and durability based on level
- **Cyberpunk Aesthetic**: Neon colors and glow effects

## 🧪 Testing

Tests will be implemented in the `tests/` directory using Catch2 or Google Test.

```bash
# Run tests (when implemented)
cd build
ctest
```

## 📝 Development

See `docs/ROADMAP.md` for the development roadmap and `docs/ANALYSIS.md` for game analysis.

## 📄 License

This project is for educational purposes.

## 🙏 Acknowledgments

- Original Next.js Brick Breaker game
- SFML community and documentation

