#ifndef PLAYINGSTATE_H
#define PLAYINGSTATE_H

#include "core/GameState.h"
#include "entities/Cannon.h"
#include "entities/Projectile.h"
#include "entities/Brick.h"
#include "managers/BlockManager.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <deque>

// Forward declaration
class Game;

/**
 * @brief Playing state for active gameplay.
 * 
 * PlayingState is the main gameplay state where the player controls
 * the cannon and shoots projectiles at blocks. Currently a placeholder
 * that will be implemented in later steps.
 */
class PlayingState : public GameState
{
public:
    /**
     * @brief Constructs a PlayingState.
     * @param game Pointer to the Game instance for state transitions
     */
    explicit PlayingState(Game* game);

    /**
     * @brief Destructor.
     */
    ~PlayingState() override = default;

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
    void onEnter() override;
    void onExit() override;

private:
    Game* game_;
    const sf::Font& font_;  // Reference to font from FontManager
    sf::Text placeholderText_;
    
    // Cannon
    std::unique_ptr<Cannon> cannon_;
    
    // Projectile pool
    ProjectilePool projectilePool_;
    
    // Block manager (wave-based spawning and descent)
    std::unique_ptr<BlockManager> blockManager_;
    
    // Game state
    int currentLevel_;
    int score_;
    
    // Explosion particle system
    struct ExplosionParticle {
        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::Color color;
        float lifetime;
        float maxLifetime;
        float size;
    };
    std::deque<ExplosionParticle> explosionParticles_;
    
    // Collision detection constants
    static constexpr float COLLISION_OFFSET = 2.0f;  // Offset to prevent sticking
    
    /**
     * @brief Checks collisions between projectiles and bricks.
     */
    void checkProjectileBrickCollisions();
    
    /**
     * @brief Performs AABB collision detection between two rectangles.
     * @param rect1 First rectangle
     * @param rect2 Second rectangle
     * @return True if rectangles intersect, false otherwise
     */
    bool checkAABBCollision(const sf::FloatRect& rect1, const sf::FloatRect& rect2) const;
    
    /**
     * @brief Determines collision side and bounces projectile.
     * @param projectile Projectile to bounce
     * @param brickBounds Brick bounding rectangle
     */
    void bounceProjectileOffBrick(Projectile* projectile, const sf::FloatRect& brickBounds);
    
    /**
     * @brief Creates an explosion effect at the specified position.
     * @param position Explosion position
     * @param color Explosion color (from brick)
     */
    void createExplosion(const sf::Vector2f& position, const sf::Color& color);
    
    /**
     * @brief Updates explosion particles.
     * @param deltaTime Time elapsed since last frame
     */
    void updateExplosionParticles(float deltaTime);
    
    /**
     * @brief Renders explosion particles.
     * @param window Render window
     */
    void renderExplosionParticles(sf::RenderWindow& window) const;
};

#endif // PLAYINGSTATE_H

