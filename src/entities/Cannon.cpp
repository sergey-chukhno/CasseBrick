#include "entities/Cannon.h"
#include "core/FontManager.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include <exception>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Cannon::Cannon(const sf::Vector2f& position, unsigned int projectileCount)
    : position_(position)
    , angle_(DEFAULT_ANGLE)
    , angleDirection_(0.0f)
    , useMouseControl_(true)  // Mouse control enabled by default
    , barrel_(sf::Vector2f(BARREL_LENGTH, BARREL_WIDTH))
    , base_(BASE_RADIUS)
    , counterText_(FontManager::getDefaultFont(), "", 20)
    , projectileCount_(projectileCount)
{
    try
    {
        // Initialize base (stationary circle)
        base_.setFillColor(CANNON_COLOR);
        base_.setOutlineColor(CANNON_OUTLINE_COLOR);
        base_.setOutlineThickness(2.0f);
        base_.setOrigin(sf::Vector2f(BASE_RADIUS, BASE_RADIUS)); // Center origin
        base_.setPosition(position_);

        // Initialize barrel (rotatable rectangle)
        barrel_.setFillColor(CANNON_COLOR);
        barrel_.setOutlineColor(CANNON_OUTLINE_COLOR);
        barrel_.setOutlineThickness(2.0f);
        // Set origin to bottom center for rotation around base
        barrel_.setOrigin(sf::Vector2f(BARREL_LENGTH / 2.0f, BARREL_WIDTH));
        barrel_.setPosition(position_);
        barrel_.setRotation(sf::degrees(angle_));

        // Initialize counter text
        counterText_.setFillColor(COUNTER_TEXT_COLOR);
        counterText_.setStyle(sf::Text::Bold);
        updateCounterText();
        counterText_.setPosition(position_ + sf::Vector2f(0, COUNTER_OFFSET_Y));
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error in Cannon constructor: " << e.what() << std::endl;
        throw;
    }
}

void Cannon::update(float deltaTime, const sf::RenderWindow& window)
{
    // Update angle from mouse if mouse control is enabled
    if (useMouseControl_)
    {
        updateAngleFromMouse(window);
    }
    else
    {
        // Update angle based on keyboard input
        float angleChange = angleDirection_ * ANGLE_SPEED * deltaTime;
        angle_ += angleChange;
    }

    // Clamp angle to bounds (-45° to +45°)
    clampAngle();

    // Update visual rotation (convert degrees to sf::Angle)
    barrel_.setRotation(sf::degrees(angle_));
    barrel_.setPosition(position_);

    // Update counter text position (stationary above base)
    counterText_.setPosition(position_ + sf::Vector2f(0, COUNTER_OFFSET_Y));
}

void Cannon::render(sf::RenderWindow& window) const
{
    // Render base (stationary)
    window.draw(base_);

    // Render barrel (rotated) - this is the angle indicator
    window.draw(barrel_);

    // Render counter text
    window.draw(counterText_);
}

void Cannon::handleInput(const sf::Event& event, const sf::RenderWindow& /* window */)
{
    // Handle keyboard input
    if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::A ||
            keyPressed->code == sf::Keyboard::Key::Left)
        {
            angleDirection_ = -1.0f; // Rotate left (counter-clockwise)
            useMouseControl_ = false; // Disable mouse control when keyboard is used
        }
        else if (keyPressed->code == sf::Keyboard::Key::D ||
                 keyPressed->code == sf::Keyboard::Key::Right)
        {
            angleDirection_ = 1.0f; // Rotate right (clockwise)
            useMouseControl_ = false; // Disable mouse control when keyboard is used
        }
    }
    else if (auto* keyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        if (keyReleased->code == sf::Keyboard::Key::A ||
            keyReleased->code == sf::Keyboard::Key::Left ||
            keyReleased->code == sf::Keyboard::Key::D ||
            keyReleased->code == sf::Keyboard::Key::Right)
        {
            angleDirection_ = 0.0f; // Stop rotation
            useMouseControl_ = true; // Re-enable mouse control when keyboard is released
        }
    }

    // Handle mouse movement (enable mouse control, actual update happens in update())
    if (event.is<sf::Event::MouseMoved>())
    {
        useMouseControl_ = true; // Enable mouse control when mouse moves
        angleDirection_ = 0.0f;  // Disable keyboard rotation
    }
}

sf::FloatRect Cannon::getBounds() const
{
    // Return bounds of the base (cannon is fixed, so base bounds are sufficient)
    return base_.getGlobalBounds();
}

sf::Vector2f Cannon::getPosition() const
{
    return position_;
}

float Cannon::getAngle() const
{
    return angle_;
}

sf::Vector2f Cannon::getShootDirection() const
{
    return angleToDirection(angle_);
}

unsigned int Cannon::getProjectileCount() const
{
    return projectileCount_;
}

bool Cannon::canShoot() const
{
    return projectileCount_ > 0;
}

void Cannon::decrementProjectileCount()
{
    if (projectileCount_ > 0)
    {
        projectileCount_--;
        updateCounterText();
    }
}

void Cannon::updateCounterText()
{
    std::stringstream ss;
    ss << "x=" << projectileCount_;
    counterText_.setString(ss.str());

    // Center text horizontally
    sf::FloatRect textBounds = counterText_.getLocalBounds();
    counterText_.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
}

void Cannon::clampAngle()
{
    angle_ = std::max(MIN_ANGLE, std::min(MAX_ANGLE, angle_));
}

sf::Vector2f Cannon::angleToDirection(float angle) const
{
    // Convert angle to direction vector
    // 0° = straight up (0, -1)
    // 45° = up-right (0.707, -0.707)
    // -45° = up-left (-0.707, -0.707)
    float radians = angle * static_cast<float>(M_PI) / 180.0f;
    return sf::Vector2f(std::sin(radians), -std::cos(radians));
}

void Cannon::updateAngleFromMouse(const sf::RenderWindow& window)
{
    if (!useMouseControl_)
    {
        return;
    }

    // Check if window is still open and valid
    if (!window.isOpen())
    {
        return;
    }

    try
    {
        // Get mouse position in window pixel coordinates
        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        
        // Convert mouse X to angle
        // Map mouse X position to angle: left edge = -45°, center = 0°, right edge = +45°
        float windowWidth = static_cast<float>(window.getSize().x);
        float targetAngle = mouseXToAngle(static_cast<float>(mousePixelPos.x), windowWidth);

        // Direct control for responsive aiming (instant response)
        angle_ = targetAngle;

        clampAngle();
    }
    catch (...)
    {
        // If there's an error accessing the window, disable mouse control
        useMouseControl_ = false;
    }
}

float Cannon::mouseXToAngle(float mouseX, float windowWidth) const
{
    // Convert mouse X position to angle
    // Mouse at left edge (0) = -45°
    // Mouse at center (width/2) = 0°
    // Mouse at right edge (width) = +45°
    float normalizedX = (mouseX / windowWidth) * 2.0f - 1.0f; // -1 to 1
    return normalizedX * MAX_ANGLE * MOUSE_SENSITIVITY;
}

