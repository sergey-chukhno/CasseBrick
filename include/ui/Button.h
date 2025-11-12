#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

/**
 * @brief Reusable button component with cyberpunk styling and glow effects.
 * 
 * Button provides a customizable button with:
 * - Multi-layer glow effects (outer glow, inner glow, outline)
 * - Hover animation (scale and brightness)
 * - Click feedback (flash effect)
 * - Callback-based event handling
 * - Smooth animations using delta time
 */
class Button
{
public:
    /**
     * @brief Constructs a Button.
     * @param font Reference to the font for button text
     * @param text Button text label
     * @param position Button position (center)
     * @param size Button size (width, height)
     */
    Button(const sf::Font& font, const std::string& text,
           const sf::Vector2f& position, const sf::Vector2f& size);

    /**
     * @brief Updates button state and animations.
     * @param deltaTime Time elapsed since last frame (in seconds)
     */
    void update(float deltaTime);

    /**
     * @brief Renders the button.
     * @param window Render window to draw to
     */
    void render(sf::RenderWindow& window) const;

    /**
     * @brief Handles input events for the button.
     * @param event SFML event to process
     * @param window Render window for coordinate mapping
     */
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);

    /**
     * @brief Sets the click callback function.
     * @param callback Function to call when button is clicked
     */
    void setOnClick(std::function<void()> callback);

    /**
     * @brief Sets the button position.
     * @param position New position (center)
     */
    void setPosition(const sf::Vector2f& position);

    /**
     * @brief Sets the button size.
     * @param size New size (width, height)
     */
    void setSize(const sf::Vector2f& size);

    /**
     * @brief Sets the button text.
     * @param text New text label
     */
    void setText(const std::string& text);

    /**
     * @brief Sets the button colors.
     * @param fillColor Button fill color
     * @param outlineColor Button outline color
     * @param textColor Text color
     */
    void setColors(const sf::Color& fillColor, const sf::Color& outlineColor,
                   const sf::Color& textColor);

    /**
     * @brief Gets the global bounding rectangle of the button.
     * @return Global bounds of the button
     */
    sf::FloatRect getGlobalBounds() const;

    /**
     * @brief Checks if the button is currently hovered.
     * @return True if hovered, false otherwise
     */
    bool isHovered() const;

private:
    // Button geometry
    sf::Vector2f position_;
    sf::Vector2f size_;
    sf::Vector2f baseScale_;  // Base scale (1.0, 1.0)
    float hoverScale_;        // Current hover scale (1.0 to 1.05)

    // Visual elements
    sf::RectangleShape buttonRect_;      // Main button rectangle
    sf::RectangleShape glowRect_;        // Outer glow rectangle
    sf::Text text_;                      // Button text

    // Colors
    sf::Color fillColor_;      // Button fill color
    sf::Color outlineColor_;   // Button outline color
    sf::Color textColor_;      // Text color

    // Animation state
    float glowIntensity_;      // Current glow intensity (0.5 to 1.0)
    bool isHovered_;           // Hover state
    bool wasClicked_;          // Click state (for flash effect)
    float clickFlashTime_;     // Time remaining for click flash (seconds)

    // Callback
    std::function<void()> onClickCallback_;

    // Animation constants
    static constexpr float HOVER_SCALE = 1.12f;  // Increased from 1.05f for more noticeable hover
    static constexpr float HOVER_LERP_SPEED = 6.0f;  // Faster hover transition
    static constexpr float GLOW_INTENSITY_MIN = 0.6f;  // Increased minimum glow
    static constexpr float GLOW_INTENSITY_MAX = 1.2f;  // Increased maximum glow (can exceed 1.0 for brightness)
    static constexpr float CLICK_FLASH_DURATION = 0.1f;
    static constexpr float GLOW_RADIUS = 12.0f;  // Increased glow radius
    static constexpr float OUTLINE_THICKNESS = 3.0f;  // Thicker outline for more visibility

    /**
     * @brief Updates hover state and animations.
     * @param deltaTime Time elapsed since last frame
     */
    void updateHover(float deltaTime);

    /**
     * @brief Renders the glow effect (multiple layers).
     * @param window Render window to draw to
     */
    void renderGlow(sf::RenderWindow& window) const;

    /**
     * @brief Linear interpolation between two values.
     * @param a Start value
     * @param b End value
     * @param t Interpolation factor (0.0 to 1.0)
     * @return Interpolated value
     */
    static float lerp(float a, float b, float t);

    /**
     * @brief Clamps a value between min and max.
     * @param value Value to clamp
     * @param min Minimum value
     * @param max Maximum value
     * @return Clamped value
     */
    static float clamp(float value, float min, float max);
};

#endif // BUTTON_H

