#ifndef CANNON_H
#define CANNON_H

#include <SFML/Graphics.hpp>

/**
 * @brief Cannon class that can be rotated to adjust shooting angle.
 * 
 * The cannon is fixed at the bottom center of the screen and can be rotated
 * left/right to adjust the angle at which projectiles are shot. The cannon
 * displays a projectile counter and supports both keyboard and mouse input
 * for angle control.
 */
class Cannon
{
public:
    /**
     * @brief Constructs a Cannon at the specified position.
     * @param position Position of the cannon (typically bottom center)
     * @param projectileCount Initial number of projectiles (default: 10)
     */
    Cannon(const sf::Vector2f& position, unsigned int projectileCount = 10);

    /**
     * @brief Destructor.
     */
    ~Cannon() = default;

    /**
     * @brief Updates the cannon state.
     * @param deltaTime Time elapsed since last frame (in seconds)
     * @param window Reference to the render window (for mouse position)
     */
    void update(float deltaTime, const sf::RenderWindow& window);

    /**
     * @brief Renders the cannon.
     * @param window Reference to the render window
     */
    void render(sf::RenderWindow& window) const;

    /**
     * @brief Handles input events.
     * @param event SFML event
     * @param window Reference to the render window
     */
    void handleInput(const sf::Event& event, const sf::RenderWindow& window);

    /**
     * @brief Gets the bounding rectangle of the cannon.
     * @return Bounding rectangle
     */
    sf::FloatRect getBounds() const;

    /**
     * @brief Gets the position of the cannon.
     * @return Position vector
     */
    sf::Vector2f getPosition() const;

    /**
     * @brief Gets the current angle of the cannon.
     * @return Angle in degrees (0° = straight up, positive = right, negative = left)
     */
    float getAngle() const;

    /**
     * @brief Gets the normalized direction vector for shooting.
     * @return Normalized direction vector
     */
    sf::Vector2f getShootDirection() const;

    /**
     * @brief Gets the current projectile count.
     * @return Number of projectiles remaining
     */
    unsigned int getProjectileCount() const;

    /**
     * @brief Checks if the cannon can shoot.
     * @return True if projectiles are available, false otherwise
     */
    bool canShoot() const;

    /**
     * @brief Decrements the projectile count.
     * Called when a projectile is shot.
     */
    void decrementProjectileCount();

private:
    // Angle control constants
    static constexpr float DEFAULT_ANGLE = 0.0f;        // 0° (straight up)
    static constexpr float MIN_ANGLE = -45.0f;          // -45° (left)
    static constexpr float MAX_ANGLE = 45.0f;           // +45° (right)
    static constexpr float ANGLE_SPEED = 90.0f;         // 90 degrees per second
    static constexpr float MOUSE_SENSITIVITY = 1.0f;    // Mouse sensitivity factor

    // Position (fixed at bottom center)
    sf::Vector2f position_;

    // Angle control
    float angle_;              // Current angle in degrees
    float angleDirection_;     // -1.0 (rotate left), 0.0 (stop), 1.0 (rotate right)

    // Mouse control
    bool useMouseControl_;     // Enable mouse control

    // Visual
    sf::RectangleShape barrel_;    // Cannon barrel (rotatable)
    sf::CircleShape base_;         // Cannon base (stationary)
    sf::Text counterText_;         // Projectile counter text

    // Projectile management
    unsigned int projectileCount_;

    // Visual constants
    static constexpr float BASE_RADIUS = 20.0f;         // Base circle radius
    static constexpr float BARREL_LENGTH = 60.0f;       // Barrel length
    static constexpr float BARREL_WIDTH = 15.0f;        // Barrel width
    static constexpr float COUNTER_OFFSET_Y = -50.0f;   // Counter Y offset above cannon

    // Colors
    static constexpr sf::Color CANNON_COLOR = sf::Color(0, 217, 255);      // Cyan #00d9ff
    static constexpr sf::Color CANNON_OUTLINE_COLOR = sf::Color(0, 217, 255); // Cyan outline
    static constexpr sf::Color COUNTER_TEXT_COLOR = sf::Color(255, 255, 255); // White

    /**
     * @brief Updates the counter text display.
     */
    void updateCounterText();

    /**
     * @brief Clamps the angle within bounds.
     */
    void clampAngle();

    /**
     * @brief Converts an angle to a direction vector.
     * @param angle Angle in degrees
     * @return Normalized direction vector
     */
    sf::Vector2f angleToDirection(float angle) const;

    /**
     * @brief Updates the angle from mouse position.
     * @param window Reference to the render window
     */
    void updateAngleFromMouse(const sf::RenderWindow& window);

    /**
     * @brief Converts mouse X position to angle.
     * @param mouseX Mouse X position
     * @param windowWidth Window width
     * @return Angle in degrees
     */
    float mouseXToAngle(float mouseX, float windowWidth) const;
};

#endif // CANNON_H

