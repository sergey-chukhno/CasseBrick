#!/bin/bash

# Test script for state transitions
# This script will build and run the game with output logging

echo "Building project..."
cd build
ninja

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "Build successful! Starting game..."
echo "Test instructions:"
echo "1. Click 'START' button - should transition to PlayingState"
echo "2. Press ESC to return to menu"
echo "3. Click 'SETTINGS' button - should transition to SettingsState"
echo "4. Click 'BACK' button - should return to menu"
echo "5. Close the game window"
echo ""
echo "Running game (check for segmentation faults)..."
echo ""

./bin/CyberpunkCannonShooter 2>&1

echo ""
echo "Game exited. Check output above for any errors."

