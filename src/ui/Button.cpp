#include "ui/Button.h"
#include <algorithm>
#include <cmath>

Button::Button(const sf::Font& font, const std::string& text,
               const sf::Vector2f& position, const sf::Vector2f& size)
    : position_(position)
    , size_(size)
    , baseScale_(1.0f, 1.0f)
    , hoverScale_(1.0f)
    , text_(font, text, 24)
    , fillColor_(0, 217, 255, 100)      // Cyan with transparency
    , outlineColor_(0, 217, 255)        // Cyan
    , textColor_(255, 255, 255)         // White
    , glowIntensity_(GLOW_INTENSITY_MIN)
    , isHovered_(false)
    , wasClicked_(false)
    , clickFlashTime_(0.0f)
{
    // Initialize button rectangle
    buttonRect_.setSize(size_);
    buttonRect_.setFillColor(fillColor_);
    buttonRect_.setOutlineColor(outlineColor_);
    buttonRect_.setOutlineThickness(OUTLINE_THICKNESS);
    buttonRect_.setOrigin(sf::Vector2f(size_.x / 2.0f, size_.y / 2.0f));
    buttonRect_.setPosition(position_);

    // Initialize glow rectangle (larger than button)
    glowRect_.setSize(sf::Vector2f(size_.x + GLOW_RADIUS * 2, size_.y + GLOW_RADIUS * 2));
    glowRect_.setOrigin(sf::Vector2f(
        (size_.x + GLOW_RADIUS * 2) / 2.0f,
        (size_.y + GLOW_RADIUS * 2) / 2.0f
    ));
    glowRect_.setPosition(position_);

    // Initialize text
    text_.setFillColor(textColor_);
    text_.setStyle(sf::Text::Bold);
    
    // Center text on button
    sf::FloatRect textBounds = text_.getLocalBounds();
    text_.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
    text_.setPosition(position_);
}

void Button::update(float deltaTime)
{
    // Update hover scale and glow intensity
    updateHover(deltaTime);

    // Update click flash
    if (clickFlashTime_ > 0.0f)
    {
        clickFlashTime_ -= deltaTime;
        if (clickFlashTime_ < 0.0f)
        {
            clickFlashTime_ = 0.0f;
            wasClicked_ = false;
        }
    }

    // Update button rectangle scale
    float currentScale = hoverScale_;
    buttonRect_.setScale(sf::Vector2f(currentScale, currentScale));
    glowRect_.setScale(sf::Vector2f(currentScale, currentScale));
}

void Button::render(sf::RenderWindow& window) const
{
    // Render glow layers (outer to inner)
    renderGlow(window);

    // Render button rectangle
    window.draw(buttonRect_);

    // Render text
    window.draw(text_);

    // Render click flash (white overlay) - fades out over time
    if (clickFlashTime_ > 0.0f)
    {
        // Flash fades from full to zero
        float flashAlpha = (clickFlashTime_ / CLICK_FLASH_DURATION) * 200.0f;  // Fade from 200 to 0
        flashAlpha = std::max(0.0f, std::min(255.0f, flashAlpha));  // Clamp to valid range
        sf::RectangleShape flashRect(size_);
        flashRect.setFillColor(sf::Color(255, 255, 255, static_cast<unsigned char>(flashAlpha)));
        flashRect.setOrigin(sf::Vector2f(size_.x / 2.0f, size_.y / 2.0f));
        flashRect.setPosition(position_);
        flashRect.setScale(sf::Vector2f(hoverScale_, hoverScale_));
        window.draw(flashRect);
    }
}

void Button::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    // Get mouse position in window coordinates
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixelPos);

    // Check if mouse is over button
    isHovered_ = getGlobalBounds().contains(mousePos);

    // Handle mouse button press
    if (auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseButton->button == sf::Mouse::Button::Left && isHovered_)
        {
            wasClicked_ = true;
            clickFlashTime_ = CLICK_FLASH_DURATION;

            // Trigger callback
            if (onClickCallback_)
            {
                onClickCallback_();
            }
        }
    }
}

void Button::setOnClick(std::function<void()> callback)
{
    onClickCallback_ = callback;
}

void Button::setPosition(const sf::Vector2f& position)
{
    position_ = position;
    buttonRect_.setPosition(position_);
    glowRect_.setPosition(position_);
    text_.setPosition(position_);
}

void Button::setSize(const sf::Vector2f& size)
{
    size_ = size;
    buttonRect_.setSize(size_);
    buttonRect_.setOrigin(sf::Vector2f(size_.x / 2.0f, size_.y / 2.0f));
    
    glowRect_.setSize(sf::Vector2f(size_.x + GLOW_RADIUS * 2, size_.y + GLOW_RADIUS * 2));
    glowRect_.setOrigin(sf::Vector2f(
        (size_.x + GLOW_RADIUS * 2) / 2.0f,
        (size_.y + GLOW_RADIUS * 2) / 2.0f
    ));
}

void Button::setText(const std::string& text)
{
    text_.setString(text);
    
    // Re-center text
    sf::FloatRect textBounds = text_.getLocalBounds();
    text_.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
    text_.setPosition(position_);
}

void Button::setColors(const sf::Color& fillColor, const sf::Color& outlineColor,
                       const sf::Color& textColor)
{
    fillColor_ = fillColor;
    outlineColor_ = outlineColor;
    textColor_ = textColor;

    buttonRect_.setFillColor(fillColor_);
    buttonRect_.setOutlineColor(outlineColor_);
    text_.setFillColor(textColor_);
}

sf::FloatRect Button::getGlobalBounds() const
{
    return buttonRect_.getGlobalBounds();
}

bool Button::isHovered() const
{
    return isHovered_;
}

void Button::updateHover(float deltaTime)
{
    // Target values based on hover state
    float targetScale = isHovered_ ? HOVER_SCALE : 1.0f;
    float targetGlow = isHovered_ ? GLOW_INTENSITY_MAX : GLOW_INTENSITY_MIN;

    // Lerp towards target values
    float lerpFactor = HOVER_LERP_SPEED * deltaTime;
    hoverScale_ = lerp(hoverScale_, targetScale, lerpFactor);
    glowIntensity_ = lerp(glowIntensity_, targetGlow, lerpFactor);

    // Clamp values
    hoverScale_ = clamp(hoverScale_, 1.0f, HOVER_SCALE);
    glowIntensity_ = clamp(glowIntensity_, GLOW_INTENSITY_MIN, GLOW_INTENSITY_MAX);
}

void Button::renderGlow(sf::RenderWindow& window) const
{
    // Render 3 glow layers with decreasing opacity and increasing size
    for (int i = 0; i < 3; ++i)
    {
        float layerAlpha = glowIntensity_ * (50.0f - i * 15.0f);  // Decreasing alpha
        float layerScale = 1.0f + (i + 1) * 0.05f;  // Increasing size

        sf::RectangleShape glowLayer(sf::Vector2f(
            size_.x + GLOW_RADIUS * (i + 1) * 2,
            size_.y + GLOW_RADIUS * (i + 1) * 2
        ));
        glowLayer.setFillColor(sf::Color(
            outlineColor_.r,
            outlineColor_.g,
            outlineColor_.b,
            static_cast<unsigned char>(layerAlpha)
        ));
        glowLayer.setOrigin(sf::Vector2f(
            glowLayer.getSize().x / 2.0f,
            glowLayer.getSize().y / 2.0f
        ));
        glowLayer.setPosition(position_);
        glowLayer.setScale(sf::Vector2f(hoverScale_ * layerScale, hoverScale_ * layerScale));

        window.draw(glowLayer);
    }
}

float Button::lerp(float a, float b, float t)
{
    return a + (b - a) * std::min(1.0f, std::max(0.0f, t));
}

float Button::clamp(float value, float min, float max)
{
    return std::max(min, std::min(max, value));
}

