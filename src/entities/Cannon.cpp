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
    , corePulseAlpha_(CORE_PULSE_MAX_ALPHA)
    , corePulseDirection_(-1.0f)  // Start decreasing
    , glowIntensity_(GLOW_INTENSITY_MAX)
    , baseBody_(sf::Vector2f(BASE_WIDTH, BASE_HEIGHT))
    , baseLeftPanel_(sf::Vector2f(BASE_PANEL_WIDTH, BASE_PANEL_HEIGHT))
    , baseRightPanel_(sf::Vector2f(BASE_PANEL_WIDTH, BASE_PANEL_HEIGHT))
    , baseCore_(BASE_CORE_RADIUS)
    , baseFrontPanel_(sf::Vector2f(BASE_FRONT_WIDTH, BASE_FRONT_HEIGHT))
    , barrelMain_(sf::Vector2f(BARREL_LENGTH, BARREL_WIDTH))
    , barrelMuzzle_(sf::Vector2f(BARREL_MUZZLE_LENGTH, BARREL_MUZZLE_WIDTH))
    , barrelRing1_(BARREL_RING_RADIUS)
    , barrelRing2_(BARREL_RING_RADIUS)
    , barrelFin1_(sf::Vector2f(BARREL_FIN_WIDTH, BARREL_FIN_LENGTH))
    , barrelFin2_(sf::Vector2f(BARREL_FIN_WIDTH, BARREL_FIN_LENGTH))
    , counterText_(FontManager::getDefaultFont(), "", 20)
    , projectileCount_(projectileCount)
{
    try
    {
        // Initialize base components
        initializeBase();
        
        // Initialize barrel components
        initializeBarrel();
        
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

    // Update animations (pulse, glow)
    updateAnimations(deltaTime);

    // Update barrel rotation and position
    updateBarrelTransform();

    // Update counter text position (below base)
    counterText_.setPosition(position_ + sf::Vector2f(0, COUNTER_OFFSET_Y));
}

void Cannon::render(sf::RenderWindow& window) const
{
    // Render glow effects for base components (behind main shapes)
    renderGlow(window, baseBody_, CANNON_PRIMARY, glowIntensity_);
    renderGlow(window, baseLeftPanel_, CANNON_SECONDARY, glowIntensity_ * 0.8f);
    renderGlow(window, baseRightPanel_, CANNON_SECONDARY, glowIntensity_ * 0.8f);
    renderGlow(window, baseFrontPanel_, CANNON_ACCENT, glowIntensity_ * 0.9f);
    
    // Render glow for barrel components (behind main shapes)
    renderGlow(window, barrelMain_, CANNON_PRIMARY, glowIntensity_);
    renderGlow(window, barrelMuzzle_, CANNON_SECONDARY, glowIntensity_ * 0.9f);
    
    // Render base components (stationary, in order from back to front)
    window.draw(baseBody_);
    window.draw(baseLeftPanel_);
    window.draw(baseRightPanel_);
    window.draw(baseFrontPanel_);
    
    // Render energy core glow (behind core)
    // Create a temporary core shape for glow rendering with current pulse alpha
    sf::CircleShape coreForGlow = baseCore_;
    coreForGlow.setFillColor(sf::Color(CANNON_CORE.r, CANNON_CORE.g, CANNON_CORE.b, 
                                        static_cast<unsigned char>(corePulseAlpha_ * 255.0f)));
    renderGlow(window, coreForGlow, CANNON_CORE, corePulseAlpha_);
    
    // Render energy core with pulsating alpha
    sf::CircleShape core = baseCore_;
    sf::Color coreColor = CANNON_CORE;
    coreColor.a = static_cast<unsigned char>(corePulseAlpha_ * 255.0f);
    core.setFillColor(coreColor);
    window.draw(core);
    
    // Render barrel components (rotatable, in order from back to front)
    window.draw(barrelMain_);
    window.draw(barrelRing1_);
    window.draw(barrelRing2_);
    window.draw(barrelFin1_);
    window.draw(barrelFin2_);
    window.draw(barrelMuzzle_);

    // Render counter text (below base)
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
    // Return bounds of the base body (cannon is fixed, so base bounds are sufficient)
    return baseBody_.getGlobalBounds();
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

void Cannon::initializeBase()
{
    // Main base body (rounded rectangle shape)
    baseBody_.setFillColor(CANNON_PRIMARY);
    baseBody_.setOutlineColor(CANNON_OUTLINE);
    baseBody_.setOutlineThickness(2.0f);
    baseBody_.setOrigin(sf::Vector2f(BASE_WIDTH / 2.0f, BASE_HEIGHT)); // Origin at bottom center
    baseBody_.setPosition(position_);
    
    // Left side panel
    baseLeftPanel_.setFillColor(CANNON_SECONDARY);
    baseLeftPanel_.setOutlineColor(CANNON_OUTLINE);
    baseLeftPanel_.setOutlineThickness(1.5f);
    baseLeftPanel_.setOrigin(sf::Vector2f(BASE_PANEL_WIDTH / 2.0f, BASE_PANEL_HEIGHT));
    baseLeftPanel_.setPosition(position_ + sf::Vector2f(-BASE_WIDTH / 2.0f - BASE_PANEL_WIDTH / 2.0f, 0));
    
    // Right side panel
    baseRightPanel_.setFillColor(CANNON_SECONDARY);
    baseRightPanel_.setOutlineColor(CANNON_OUTLINE);
    baseRightPanel_.setOutlineThickness(1.5f);
    baseRightPanel_.setOrigin(sf::Vector2f(BASE_PANEL_WIDTH / 2.0f, BASE_PANEL_HEIGHT));
    baseRightPanel_.setPosition(position_ + sf::Vector2f(BASE_WIDTH / 2.0f + BASE_PANEL_WIDTH / 2.0f, 0));
    
    // Front panel (at top of base)
    baseFrontPanel_.setFillColor(CANNON_ACCENT);
    baseFrontPanel_.setOutlineColor(CANNON_OUTLINE);
    baseFrontPanel_.setOutlineThickness(1.5f);
    baseFrontPanel_.setOrigin(sf::Vector2f(BASE_FRONT_WIDTH / 2.0f, BASE_FRONT_HEIGHT));
    baseFrontPanel_.setPosition(position_ + sf::Vector2f(0, -BASE_HEIGHT + BASE_FRONT_HEIGHT / 2.0f));
    
    // Central energy core
    baseCore_.setFillColor(CANNON_CORE);
    baseCore_.setOutlineColor(CANNON_OUTLINE);
    baseCore_.setOutlineThickness(1.5f);
    baseCore_.setOrigin(sf::Vector2f(BASE_CORE_RADIUS, BASE_CORE_RADIUS));
    baseCore_.setPosition(position_ + sf::Vector2f(0, -BASE_HEIGHT / 2.0f));
}

void Cannon::initializeBarrel()
{
    // Main barrel segment
    barrelMain_.setFillColor(CANNON_PRIMARY);
    barrelMain_.setOutlineColor(CANNON_OUTLINE);
    barrelMain_.setOutlineThickness(2.0f);
    // Origin at bottom center for rotation around attachment point
    barrelMain_.setOrigin(sf::Vector2f(BARREL_LENGTH / 2.0f, BARREL_WIDTH));
    
    // Muzzle opening (wider at the end)
    barrelMuzzle_.setFillColor(CANNON_SECONDARY);
    barrelMuzzle_.setOutlineColor(CANNON_OUTLINE);
    barrelMuzzle_.setOutlineThickness(2.0f);
    // Origin at left center (attaches to end of barrel)
    barrelMuzzle_.setOrigin(sf::Vector2f(0, BARREL_MUZZLE_WIDTH / 2.0f));
    
    // Energy ring 1 (near base of barrel)
    barrelRing1_.setFillColor(sf::Color::Transparent);
    barrelRing1_.setOutlineColor(CANNON_ACCENT);
    barrelRing1_.setOutlineThickness(2.0f);
    barrelRing1_.setOrigin(sf::Vector2f(BARREL_RING_RADIUS, BARREL_RING_RADIUS));
    
    // Energy ring 2 (middle of barrel)
    barrelRing2_.setFillColor(sf::Color::Transparent);
    barrelRing2_.setOutlineColor(CANNON_ACCENT);
    barrelRing2_.setOutlineThickness(2.0f);
    barrelRing2_.setOrigin(sf::Vector2f(BARREL_RING_RADIUS, BARREL_RING_RADIUS));
    
    // Top fin
    barrelFin1_.setFillColor(CANNON_SECONDARY);
    barrelFin1_.setOutlineColor(CANNON_OUTLINE);
    barrelFin1_.setOutlineThickness(1.5f);
    barrelFin1_.setOrigin(sf::Vector2f(BARREL_FIN_WIDTH / 2.0f, 0)); // Origin at bottom center
    
    // Bottom fin
    barrelFin2_.setFillColor(CANNON_SECONDARY);
    barrelFin2_.setOutlineColor(CANNON_OUTLINE);
    barrelFin2_.setOutlineThickness(1.5f);
    barrelFin2_.setOrigin(sf::Vector2f(BARREL_FIN_WIDTH / 2.0f, BARREL_FIN_LENGTH)); // Origin at top center
    
    // Initial barrel transform
    updateBarrelTransform();
}

void Cannon::updateBarrelTransform()
{
    // Calculate rotation origin (top center of base, where barrel attaches)
    // Base origin is at bottom center, so top center is BASE_HEIGHT up
    sf::Vector2f rotationOrigin = position_ + sf::Vector2f(0, -BASE_HEIGHT);
    
    // Convert angle to radians for calculations
    float angleRad = angle_ * static_cast<float>(M_PI) / 180.0f;
    float cosAngle = std::cos(angleRad);
    float sinAngle = std::sin(angleRad);
    
    // Main barrel segment (rotates around its bottom center origin)
    barrelMain_.setPosition(rotationOrigin);
    barrelMain_.setRotation(sf::degrees(angle_));
    
    // Muzzle (at the end of the barrel, extends forward)
    float barrelEndX = sinAngle * BARREL_LENGTH;
    float barrelEndY = -cosAngle * BARREL_LENGTH;
    barrelMuzzle_.setPosition(rotationOrigin + sf::Vector2f(barrelEndX, barrelEndY));
    barrelMuzzle_.setRotation(sf::degrees(angle_));
    
    // Energy ring 1 (1/3 along barrel)
    float ring1X = sinAngle * (BARREL_LENGTH / 3.0f);
    float ring1Y = -cosAngle * (BARREL_LENGTH / 3.0f);
    barrelRing1_.setPosition(rotationOrigin + sf::Vector2f(ring1X, ring1Y));
    
    // Energy ring 2 (2/3 along barrel)
    float ring2X = sinAngle * (BARREL_LENGTH * 2.0f / 3.0f);
    float ring2Y = -cosAngle * (BARREL_LENGTH * 2.0f / 3.0f);
    barrelRing2_.setPosition(rotationOrigin + sf::Vector2f(ring2X, ring2Y));
    
    // Top fin (attached to top side of barrel, at midpoint)
    float finMidX = sinAngle * (BARREL_LENGTH / 2.0f);
    float finMidY = -cosAngle * (BARREL_LENGTH / 2.0f);
    // Offset perpendicular to barrel (to the left/right side)
    float finOffsetX = -cosAngle * (BARREL_WIDTH / 2.0f + BARREL_FIN_LENGTH / 2.0f);
    float finOffsetY = -sinAngle * (BARREL_WIDTH / 2.0f + BARREL_FIN_LENGTH / 2.0f);
    barrelFin1_.setPosition(rotationOrigin + sf::Vector2f(finMidX + finOffsetX, finMidY + finOffsetY));
    barrelFin1_.setRotation(sf::degrees(angle_));
    
    // Bottom fin (attached to bottom side of barrel, at midpoint)
    // Offset to the opposite side
    float fin2OffsetX = cosAngle * (BARREL_WIDTH / 2.0f + BARREL_FIN_LENGTH / 2.0f);
    float fin2OffsetY = sinAngle * (BARREL_WIDTH / 2.0f + BARREL_FIN_LENGTH / 2.0f);
    barrelFin2_.setPosition(rotationOrigin + sf::Vector2f(finMidX + fin2OffsetX, finMidY + fin2OffsetY));
    barrelFin2_.setRotation(sf::degrees(angle_));
}

void Cannon::updateAnimations(float deltaTime)
{
    // Update core pulse animation
    // Pulse speed controls how fast alpha changes (alpha units per second)
    float pulseSpeed = (CORE_PULSE_MAX_ALPHA - CORE_PULSE_MIN_ALPHA) * CORE_PULSE_SPEED;
    corePulseAlpha_ += corePulseDirection_ * pulseSpeed * deltaTime;
    
    // Clamp and reverse direction
    if (corePulseAlpha_ >= CORE_PULSE_MAX_ALPHA)
    {
        corePulseAlpha_ = CORE_PULSE_MAX_ALPHA;
        corePulseDirection_ = -1.0f; // Start decreasing
    }
    else if (corePulseAlpha_ <= CORE_PULSE_MIN_ALPHA)
    {
        corePulseAlpha_ = CORE_PULSE_MIN_ALPHA;
        corePulseDirection_ = 1.0f; // Start increasing
    }
    
    // Update glow intensity (subtle pulsing synchronized with core)
    // Normalize corePulseAlpha_ to 0-1 range for smooth glow variation
    float normalizedPulse = (corePulseAlpha_ - CORE_PULSE_MIN_ALPHA) / (CORE_PULSE_MAX_ALPHA - CORE_PULSE_MIN_ALPHA);
    glowIntensity_ = GLOW_INTENSITY_MIN + (GLOW_INTENSITY_MAX - GLOW_INTENSITY_MIN) * normalizedPulse;
}

void Cannon::renderGlow(sf::RenderWindow& window, const sf::RectangleShape& shape, const sf::Color& baseColor, float intensity) const
{
    // Render multiple glow layers with decreasing opacity and increasing size
    for (int i = 0; i < GLOW_LAYERS; ++i)
    {
        float layerAlpha = intensity * (40.0f - i * 12.0f); // Decreasing alpha
        float layerScale = 1.0f + (i + 1) * 0.08f; // Increasing size
        
        sf::RectangleShape glowLayer(shape.getSize());
        glowLayer.setFillColor(sf::Color(
            baseColor.r,
            baseColor.g,
            baseColor.b,
            static_cast<unsigned char>(layerAlpha)
        ));
        glowLayer.setOutlineColor(sf::Color::Transparent);
        glowLayer.setOutlineThickness(0.0f);
        glowLayer.setOrigin(shape.getOrigin());
        glowLayer.setPosition(shape.getPosition());
        glowLayer.setRotation(shape.getRotation());
        
        // Calculate scale: base scale * layer scale
        sf::Vector2f baseScale = shape.getScale();
        glowLayer.setScale(sf::Vector2f(baseScale.x * layerScale, baseScale.y * layerScale));
        
        window.draw(glowLayer);
    }
}

void Cannon::renderGlow(sf::RenderWindow& window, const sf::CircleShape& shape, const sf::Color& baseColor, float intensity) const
{
    // Render multiple glow layers with decreasing opacity and increasing size
    for (int i = 0; i < GLOW_LAYERS; ++i)
    {
        float layerAlpha = intensity * (40.0f - i * 12.0f); // Decreasing alpha
        float layerScale = 1.0f + (i + 1) * 0.08f; // Increasing size
        
        sf::CircleShape glowLayer(shape.getRadius());
        glowLayer.setFillColor(sf::Color(
            baseColor.r,
            baseColor.g,
            baseColor.b,
            static_cast<unsigned char>(layerAlpha)
        ));
        glowLayer.setOutlineColor(sf::Color::Transparent);
        glowLayer.setOutlineThickness(0.0f);
        glowLayer.setOrigin(shape.getOrigin());
        glowLayer.setPosition(shape.getPosition());
        glowLayer.setRotation(shape.getRotation());
        
        // Calculate scale: base scale * layer scale
        sf::Vector2f baseScale = shape.getScale();
        glowLayer.setScale(sf::Vector2f(baseScale.x * layerScale, baseScale.y * layerScale));
        
        window.draw(glowLayer);
    }
}

