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
#include <unordered_map>
#include <algorithm>

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
    
    // Track which bricks each projectile has recently hit (to prevent rapid re-hits)
    // Maps projectile pointer to a set of brick pointers it has hit
    std::unordered_map<Projectile*, std::vector<Brick*>> projectileHitBricks_;
    
    // Game state
    int currentLevel_;
    int score_;
    int displayedScore_;  // Displayed score (for animated count-up effect)
    int bricksDestroyed_;  // Number of bricks destroyed
    
    // High score system
    int highScore_;
    static constexpr const char* HIGH_SCORE_FILE = "highscore.txt";
    
    // Score display
    sf::Text scoreText_;
    
    // Score calculation constants
    static constexpr int BASE_SCORE_PER_BRICK = 10;
    static constexpr float LEVEL_MULTIPLIER_BASE = 1.0f;
    static constexpr float LEVEL_MULTIPLIER_STEP = 0.2f;
    static constexpr float HEALTH_MULTIPLIER_BASE = 1.0f;
    static constexpr float HEALTH_MULTIPLIER_STEP = 0.5f;
    
    // Animated score constants
    static constexpr float SCORE_ANIMATION_SPEED = 500.0f;  // Points per second for count-up
    
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
    static constexpr float COLLISION_OFFSET = 8.0f;  // Offset to prevent sticking (increased to prevent re-collisions)
    
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
    
    /**
     * @brief Calculates score for destroying a brick.
     * @param level Current level
     * @param brickMaxHealth Maximum health of the brick
     * @return Score points awarded
     */
    int calculateScore(int level, int brickMaxHealth) const;
    
    /**
     * @brief Adds score to total score.
     * @param points Points to add
     */
    void addScore(int points);
    
    /**
     * @brief Updates animated score display.
     * @param deltaTime Time elapsed since last frame
     */
    void updateScoreDisplay(float deltaTime);
    
    /**
     * @brief Initializes score display text.
     */
    void initializeScoreDisplay();
    
    /**
     * @brief Loads high score from file.
     */
    void loadHighScore();
    
    /**
     * @brief Saves high score to file.
     */
    void saveHighScore();
    
    /**
     * @brief Gets the current score.
     * @return Current score
     */
    int getScore() const { return score_; }
    
    /**
     * @brief Gets the current level.
     * @return Current level
     */
    int getCurrentLevel() const { return currentLevel_; }
    
    /**
     * @brief Gets the number of bricks destroyed.
     * @return Number of bricks destroyed
     */
    int getBricksDestroyed() const { return bricksDestroyed_; }
};

#endif // PLAYINGSTATE_H

