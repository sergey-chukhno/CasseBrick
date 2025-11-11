#include "core/states/PlayingState.h"
#include "core/Game.h"
#include "core/states/PausedState.h"
#include "core/states/GameOverState.h"
#include "core/states/MenuState.h"
#include "core/FontManager.h"
#include "entities/Cannon.h"
#include "entities/Projectile.h"
#include "entities/Brick.h"
#include "managers/BlockManager.h"
#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <fstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

PlayingState::PlayingState(Game* game)
    : game_(game)
    , font_(FontManager::getDefaultFont())
    , placeholderText_(
        font_,
        "PLAYING STATE\n\nPress SPACE or CLICK to shoot\nPress P to Pause\nPress ESC to return to Menu",
        32
    )
    , projectilePool_(100) // Pool size: 100 projectiles
    , currentLevel_(1)  // Will be synced with BlockManager in onEnter()
    , score_(0)
    , displayedScore_(0)
    , bricksDestroyed_(0)
    , highScore_(0)
    , scoreText_(font_, "Score: 0", 24)
{
    // Basic text initialization - positioning will be done in onEnter()
    placeholderText_.setFillColor(sf::Color(0, 217, 255)); // Cyan
    placeholderText_.setStyle(sf::Text::Bold);
    
    // Initialize score text
    scoreText_.setFillColor(sf::Color(0, 217, 255)); // Cyan
    scoreText_.setStyle(sf::Text::Bold);
    
    // Load high score
    loadHighScore();
}

void PlayingState::update(float deltaTime)
{
    // Update cannon
    if (cannon_)
    {
        cannon_->update(deltaTime, game_->getWindow());
        
        // Update all projectiles
        sf::Vector2u windowSize(game_->getWindowWidth(), game_->getWindowHeight());
        sf::FloatRect cannonBounds = cannon_->getBounds();
        projectilePool_.updateAll(deltaTime, windowSize, cannonBounds);
        
        // Update BlockManager
        if (blockManager_)
        {
            blockManager_->update(deltaTime, cannonBounds);
            
            // Check collisions between projectiles and bricks (this may destroy bricks)
            checkProjectileBrickCollisions();
            
            // Update block destroyed states AFTER collision detection
            // This ensures blocks are marked as destroyed when all their bricks are destroyed
            blockManager_->updateBlockDestroyedStates(deltaTime);
            
            // Check level completion FIRST (before game over)
            // This ensures we can advance to next level even if blocks are near the bottom
            if (blockManager_->isLevelComplete())
            {
                // Advance to next level
                // advanceLevel() will increment the level internally
                blockManager_->advanceLevel();
                // Update currentLevel_ to match BlockManager's level
                currentLevel_ = blockManager_->getCurrentLevel();
                std::cout << "Level " << currentLevel_ << " started" << std::endl;
                // Continue to next frame - don't check game over if level is complete
                return;
            }
            
            // Check game over conditions (only if level is not complete)
            if (blockManager_->hasBlocksReachedBottom() || 
                blockManager_->hasBlocksTouchedCannon(cannonBounds))
            {
                // Update high score if needed
                if (score_ > highScore_) {
                    highScore_ = score_;
                    saveHighScore();
                }
                
                // Trigger game over with statistics
                game_->queueStateChange(std::make_unique<GameOverState>(game_, score_, currentLevel_, bricksDestroyed_));
                return;
            }
        }
        
        // Update explosion particles
        updateExplosionParticles(deltaTime);
        
        // Update animated score display
        updateScoreDisplay(deltaTime);
    }
}

void PlayingState::render(sf::RenderWindow& window)
{
    // Render blocks (through BlockManager, behind projectiles and cannon)
    if (blockManager_)
    {
        blockManager_->render(window);
    }
    
    // Render projectiles (behind cannon)
    projectilePool_.renderAll(window);
    
    // Render explosion particles (behind cannon, on top of projectiles)
    renderExplosionParticles(window);
    
    // Render cannon
    if (cannon_)
    {
        cannon_->render(window);
    }
    
    // Render score display (top-left corner)
    window.draw(scoreText_);
    
    // Draw placeholder text (temporary, will be removed later)
    window.draw(placeholderText_);
}

void PlayingState::handleEvent(const sf::Event& event)
{
    // Pass events to cannon for angle control
    if (cannon_)
    {
        cannon_->handleInput(event, game_->getWindow());
    }
    
    // Handle shooting (Space bar or mouse click)
    if (cannon_ && cannon_->canShoot())
    {
        bool shootRequested = false;
        
        if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::Space)
            {
                shootRequested = true;
            }
        }
        
        if (auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseButton->button == sf::Mouse::Button::Left)
            {
                shootRequested = true;
            }
        }
        
        if (shootRequested)
        {
            // Shoot projectile from cannon
            sf::Vector2f spawnPosition;
            sf::Vector2f velocity;
            if (cannon_->shoot(spawnPosition, velocity))
            {
                // Acquire projectile from pool
                Projectile* projectile = projectilePool_.acquire(spawnPosition, velocity);
                if (!projectile)
                {
                    std::cerr << "Warning: Projectile pool is full!" << std::endl;
                }
            }
        }
    }
    
    // Handle game state transitions
    if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        // P key: Pause
        if (keyPressed->code == sf::Keyboard::Key::P)
        {
            std::cout << "Pause key pressed" << std::endl;
            game_->pushState(std::make_unique<PausedState>(game_));
        }
        // ESC key: Return to menu
        else if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            std::cout << "Returning to menu" << std::endl;
            game_->queueStateChange(std::make_unique<MenuState>(game_));
        }
    }
}

void PlayingState::onEnter()
{
    std::cout << "Entered PlayingState" << std::endl;
    
    try
    {
        // Initialize cannon at bottom center
        float cannonX = static_cast<float>(game_->getWindowWidth()) / 2.0f;
        float cannonY = static_cast<float>(game_->getWindowHeight()) - 50.0f; // 50px from bottom
        std::cout << "Creating cannon at (" << cannonX << ", " << cannonY << ")" << std::endl;
        
        cannon_ = std::make_unique<Cannon>(
            sf::Vector2f(cannonX, cannonY),
            10 // Initial projectile count
        );
        
        std::cout << "Cannon created successfully" << std::endl;
        
        // Initialize BlockManager
        blockManager_ = std::make_unique<BlockManager>(
            game_->getWindowWidth(),
            game_->getWindowHeight()
        );
        
        // Initialize game state
        score_ = 0;
        displayedScore_ = 0;
        bricksDestroyed_ = 0;
        
        // Initialize score display
        initializeScoreDisplay();
        
        // Start level 1
        blockManager_->startLevel(1);
        // Sync currentLevel_ with BlockManager's level
        currentLevel_ = blockManager_->getCurrentLevel();
        std::cout << "Level " << currentLevel_ << " started" << std::endl;
        
        // Center placeholder text now that we're safely in the state stack
        // SFML 3.0: Rect uses .size (Vector2f) instead of .width/.height
        sf::FloatRect textBounds = placeholderText_.getLocalBounds();
        placeholderText_.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
        placeholderText_.setPosition(sf::Vector2f(
            static_cast<float>(game_->getWindowWidth()) / 2.0f,
            100.0f // Position above cannon
        ));
        
        std::cout << "PlayingState initialization complete" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error in PlayingState::onEnter(): " << e.what() << std::endl;
        throw;
    }
    catch (...)
    {
        std::cerr << "Unknown error in PlayingState::onEnter()" << std::endl;
        throw;
    }
}

void PlayingState::onExit()
{
    std::cout << "Exited PlayingState" << std::endl;
}

void PlayingState::checkProjectileBrickCollisions()
{
    if (!blockManager_) {
        return;
    }
    
    // Get all active projectiles and blocks
    std::vector<Projectile*> activeProjectiles = projectilePool_.getActiveProjectiles();
    std::vector<Block*> activeBlocks = blockManager_->getActiveBlocks();
    
    // Iterate through all active projectiles
    for (Projectile* projectile : activeProjectiles) {
        if (!projectile || !projectile->isActive()) {
            continue;
        }
        
        sf::FloatRect projectileBounds = projectile->getBounds();
        bool hasCollided = false;  // Track if projectile has collided (for bounce direction)
        sf::FloatRect firstCollisionBounds;  // Store first collision for bounce
        
        // For each block
        for (Block* block : activeBlocks) {
            if (!block || block->isDestroyed()) {
                continue;
            }
            
            // Get all bricks in the block
            std::vector<Brick*> bricks = block->getBricks();
            
            // For each brick (allow multiple hits)
            for (Brick* brick : bricks) {
                if (!brick || brick->isDestroyed()) {
                    continue;
                }
                
                // Get brick bounds
                sf::FloatRect brickBounds = brick->getBounds();
                
                // Check collision (AABB)
                if (checkAABBCollision(projectileBounds, brickBounds)) {
                    // Get brick color and position before damage (for explosion effect)
                    sf::Vector2f brickPos = brick->getPosition();
                    sf::Color brickColor = brick->getBaseColor();
                    
                    // Apply damage to brick (1 damage per hit)
                    bool wasDestroyed = brick->takeDamage(1);
                    
                    // Store first collision for bounce (only bounce once per frame)
                    if (!hasCollided) {
                        hasCollided = true;
                        firstCollisionBounds = brickBounds;
                    }
                    
                    // Create explosion effect if brick is destroyed
                    if (wasDestroyed) {
                        createExplosion(brickPos, brickColor);
                        
                        // Calculate and add score
                        int brickMaxHealth = brick->getMaxHealth();
                        int points = calculateScore(currentLevel_, brickMaxHealth);
                        addScore(points);
                        
                        // Increment bricks destroyed counter
                        bricksDestroyed_++;
                    }
                    
                    // Continue checking other bricks (allow multiple hits)
                    // Don't break here - allow projectile to hit multiple bricks
                }
            }
        }
        
        // Bounce projectile based on first collision (only bounce once per frame)
        if (hasCollided) {
            bounceProjectileOffBrick(projectile, firstCollisionBounds);
        }
    }
}

bool PlayingState::checkAABBCollision(const sf::FloatRect& rect1, const sf::FloatRect& rect2) const
{
    // AABB collision detection (SFML 3.0: Rect uses position and size)
    return (rect1.position.x < rect2.position.x + rect2.size.x &&
            rect1.position.x + rect1.size.x > rect2.position.x &&
            rect1.position.y < rect2.position.y + rect2.size.y &&
            rect1.position.y + rect1.size.y > rect2.position.y);
}

void PlayingState::bounceProjectileOffBrick(Projectile* projectile, const sf::FloatRect& brickBounds)
{
    if (!projectile) {
        return;
    }
    
    sf::Vector2f projectilePos = projectile->getPosition();
    sf::Vector2f projectileVel = projectile->getVelocity();
    
    // Calculate brick center and edges
    float brickLeft = brickBounds.position.x;
    float brickRight = brickBounds.position.x + brickBounds.size.x;
    float brickTop = brickBounds.position.y;
    float brickBottom = brickBounds.position.y + brickBounds.size.y;
    float brickCenterX = brickLeft + brickBounds.size.x / 2.0f;
    float brickCenterY = brickTop + brickBounds.size.y / 2.0f;
    
    // Determine which side of the brick was hit based on projectile position and velocity
    // Calculate distances to each edge
    float distToLeft = std::abs(projectilePos.x - brickLeft);
    float distToRight = std::abs(projectilePos.x - brickRight);
    float distToTop = std::abs(projectilePos.y - brickTop);
    float distToBottom = std::abs(projectilePos.y - brickBottom);
    
    // Find the closest edge
    float minDist = std::min({distToLeft, distToRight, distToTop, distToBottom});
    
    // Also consider velocity direction for more accurate bounce
    bool movingRight = projectileVel.x > 0;
    bool movingLeft = projectileVel.x < 0;
    bool movingDown = projectileVel.y > 0;
    bool movingUp = projectileVel.y < 0;
    
    // Determine bounce based on closest edge and velocity direction
    // Use velocity direction as primary indicator, distance as secondary
    if (minDist == distToTop && (movingUp || projectilePos.y < brickCenterY)) {
        // Hit top edge, bounce down (reflect Y velocity downward)
        projectile->setVelocity(sf::Vector2f(projectileVel.x, std::abs(projectileVel.y)));
        projectile->setPosition(sf::Vector2f(projectilePos.x, brickTop - COLLISION_OFFSET));
    } else if (minDist == distToBottom && (movingDown || projectilePos.y > brickCenterY)) {
        // Hit bottom edge, bounce up (reflect Y velocity upward)
        projectile->setVelocity(sf::Vector2f(projectileVel.x, -std::abs(projectileVel.y)));
        projectile->setPosition(sf::Vector2f(projectilePos.x, brickBottom + COLLISION_OFFSET));
    } else if (minDist == distToLeft && (movingLeft || projectilePos.x < brickCenterX)) {
        // Hit left edge, bounce right (reflect X velocity rightward)
        projectile->setVelocity(sf::Vector2f(std::abs(projectileVel.x), projectileVel.y));
        projectile->setPosition(sf::Vector2f(brickLeft - COLLISION_OFFSET, projectilePos.y));
    } else if (minDist == distToRight && (movingRight || projectilePos.x > brickCenterX)) {
        // Hit right edge, bounce left (reflect X velocity leftward)
        projectile->setVelocity(sf::Vector2f(-std::abs(projectileVel.x), projectileVel.y));
        projectile->setPosition(sf::Vector2f(brickRight + COLLISION_OFFSET, projectilePos.y));
    } else {
        // Fallback: determine based on velocity direction only
        if (std::abs(projectileVel.x) > std::abs(projectileVel.y)) {
            // Horizontal collision (reflect X)
            if (movingRight) {
                projectile->setVelocity(sf::Vector2f(-std::abs(projectileVel.x), projectileVel.y));
                projectile->setPosition(sf::Vector2f(brickLeft - COLLISION_OFFSET, projectilePos.y));
            } else if (movingLeft) {
                projectile->setVelocity(sf::Vector2f(std::abs(projectileVel.x), projectileVel.y));
                projectile->setPosition(sf::Vector2f(brickRight + COLLISION_OFFSET, projectilePos.y));
            }
        } else {
            // Vertical collision (reflect Y)
            if (movingDown) {
                projectile->setVelocity(sf::Vector2f(projectileVel.x, -std::abs(projectileVel.y)));
                projectile->setPosition(sf::Vector2f(projectilePos.x, brickTop - COLLISION_OFFSET));
            } else if (movingUp) {
                projectile->setVelocity(sf::Vector2f(projectileVel.x, std::abs(projectileVel.y)));
                projectile->setPosition(sf::Vector2f(projectilePos.x, brickBottom + COLLISION_OFFSET));
            }
        }
    }
}

void PlayingState::createExplosion(const sf::Vector2f& position, const sf::Color& color)
{
    // Create 10-16 particles with color variation (medium complexity)
    int particleCount = 10 + (std::rand() % 7);  // 10-16 particles
    
    for (int i = 0; i < particleCount; ++i) {
        ExplosionParticle particle;
        
        // Random direction and speed
        float angle = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f * static_cast<float>(M_PI);
        float speed = 100.0f + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 100.0f;  // 100-200 px/s
        
        particle.position = position;
        particle.velocity = sf::Vector2f(
            std::cos(angle) * speed,
            std::sin(angle) * speed
        );
        
        // Color variation: slight randomization of RGB values
        int colorVariation = 30;  // +/- 30 RGB units
        int r = std::max(0, std::min(255, static_cast<int>(color.r) + (std::rand() % (colorVariation * 2)) - colorVariation));
        int g = std::max(0, std::min(255, static_cast<int>(color.g) + (std::rand() % (colorVariation * 2)) - colorVariation));
        int b = std::max(0, std::min(255, static_cast<int>(color.b) + (std::rand() % (colorVariation * 2)) - colorVariation));
        particle.color = sf::Color(r, g, b);
        
        // Lifetime: 0.2-0.4 seconds
        particle.maxLifetime = 0.2f + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 0.2f;
        particle.lifetime = particle.maxLifetime;
        
        // Size: 2-4px radius
        particle.size = 2.0f + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f;
        
        explosionParticles_.push_back(particle);
    }
}

void PlayingState::updateExplosionParticles(float deltaTime)
{
    // Update all particles
    for (auto& particle : explosionParticles_) {
        // Update position
        particle.position += particle.velocity * deltaTime;
        
        // Update lifetime
        particle.lifetime -= deltaTime;
    }
    
    // Remove expired particles
    explosionParticles_.erase(
        std::remove_if(
            explosionParticles_.begin(),
            explosionParticles_.end(),
            [](const ExplosionParticle& particle) {
                return particle.lifetime <= 0.0f;
            }
        ),
        explosionParticles_.end()
    );
}

void PlayingState::renderExplosionParticles(sf::RenderWindow& window) const
{
    for (const auto& particle : explosionParticles_) {
        // Calculate alpha based on lifetime
        float alphaFactor = particle.lifetime / particle.maxLifetime;
        unsigned char alpha = static_cast<unsigned char>(255 * alphaFactor);
        
        // Create particle shape
        sf::CircleShape particleShape(particle.size);
        particleShape.setOrigin(sf::Vector2f(particle.size, particle.size));
        particleShape.setPosition(particle.position);
        particleShape.setFillColor(sf::Color(particle.color.r, particle.color.g, particle.color.b, alpha));
        particleShape.setOutlineThickness(0.0f);
        
        window.draw(particleShape);
    }
}

int PlayingState::calculateScore(int level, int brickMaxHealth) const
{
    // Calculate level multiplier (linear scaling)
    float levelMultiplier = LEVEL_MULTIPLIER_BASE + (level - 1) * LEVEL_MULTIPLIER_STEP;
    
    // Calculate health multiplier (linear scaling)
    float healthMultiplier = HEALTH_MULTIPLIER_BASE + (brickMaxHealth - 1) * HEALTH_MULTIPLIER_STEP;
    
    // Calculate final score
    float finalScore = static_cast<float>(BASE_SCORE_PER_BRICK) * levelMultiplier * healthMultiplier;
    
    return static_cast<int>(finalScore);
}

void PlayingState::addScore(int points)
{
    score_ += points;
}

void PlayingState::updateScoreDisplay(float deltaTime)
{
    // Animate score counting up
    if (displayedScore_ < score_) {
        int scoreDifference = score_ - displayedScore_;
        int increment = static_cast<int>(SCORE_ANIMATION_SPEED * deltaTime);
        
        if (increment >= scoreDifference) {
            displayedScore_ = score_;
        } else {
            displayedScore_ += increment;
        }
        
        // Update score text
        std::stringstream ss;
        ss << "Score: " << displayedScore_;
        scoreText_.setString(ss.str());
    } else if (displayedScore_ > score_) {
        // Safety check: if displayed score is somehow higher than actual score, sync it
        displayedScore_ = score_;
        std::stringstream ss;
        ss << "Score: " << displayedScore_;
        scoreText_.setString(ss.str());
    }
}

void PlayingState::initializeScoreDisplay()
{
    // Set score text position (top-left corner)
    scoreText_.setPosition(sf::Vector2f(20.0f, 20.0f));
    
    // Initialize displayed score
    displayedScore_ = score_;
    
    // Set initial score text
    std::stringstream ss;
    ss << "Score: " << displayedScore_;
    scoreText_.setString(ss.str());
}

void PlayingState::loadHighScore()
{
    std::ifstream file(HIGH_SCORE_FILE);
    if (file.is_open()) {
        file >> highScore_;
        file.close();
    } else {
        // File doesn't exist, set high score to 0
        highScore_ = 0;
    }
}

void PlayingState::saveHighScore()
{
    std::ofstream file(HIGH_SCORE_FILE);
    if (file.is_open()) {
        file << highScore_;
        file.close();
    }
}

