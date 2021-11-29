///\/\\////\///////\////\\\\\\/\\/\/\\////\///////\////\\\\\\/\\
//
//  Assignment       COMP4300 - Assignment 3
//  Professor:       David Churchill
//  Year / Term:     2021FALL
//  File Name:       Scene_Play.cpp
// 
//  Student Name:    Mahdi Rabie, Constance Gray
//  Student User:    mr1826, cdg165
//  Student Email:   mr1826@mun.ca, cdg165@mun.ca
//  Student ID:      201025756, 201050531
//  Group Member(s): [enter student name(s)]
//
///\/\\////\///////\////\\\\\\/\\/\/\\////\///////\////\\\\\\/\\

#include "Scene_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

Scene_Play::Scene_Play(GameEngine * gameEngine, const std::string & levelPath)
    : Scene(gameEngine)
    , m_levelPath(levelPath)
{
    init(m_levelPath);
}

//  Register keyboard controls, Set a font and call loadLevel
void Scene_Play::init(const std::string & levelPath)
{
	registerAction ( sf::Keyboard::P,           "PAUSE" );                                 //    Pause the game
	registerAction ( sf::Keyboard::Escape, "QUIT" );                                   //    Exit to menu if in game and exit window if in menu
	registerAction ( sf::Keyboard::T,           "TOGGLE_TEXTURE" );           //    Toggle drawing (T)extures
	registerAction ( sf::Keyboard::C,           "TOGGLE_COLLISION" );        //    Toggle drawing (C)ollision Boxes
	registerAction ( sf::Keyboard::G,           "TOGGLE_GRID" );                 //    Toggle drawing (G)rid
	registerAction ( sf::Keyboard::W,          "UP" );                                      //    Go Up/Jumping
	registerAction ( sf::Keyboard::A,           "LEFT" );                                   //    Go Left
	registerAction ( sf::Keyboard::D,           "RIGHT" );                               //     Go Right
	registerAction ( sf::Keyboard::Space,   "SHOOT" );                              //     Spawn a bullet when space bar is pressed

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Arial"));

    loadLevel(levelPath);
}

//  This function takes in a grid (x,y) position and an Entity
//  Returns a Vec2 indicating where the CENTER position of the Entity is
Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    //  Get references to the entities name and size attributes
    const std::string& tempName = entity->getComponent<CAnimation>().animation.getName();
    auto& aSize = m_game->assets().getAnimation(tempName).getSize();
    //  Calculate new coordinates for the center of the entity
    float entityCenterX = gridX * m_gridSize.x + (aSize.x / 2);
    float entityCenterY = ((height() / m_gridSize.y) - gridY)* m_gridSize.y - (aSize.y / 2);

    return Vec2(entityCenterX, entityCenterY);
}
                             
//  Read in the level file and assign the assets to their starting positions
void Scene_Play::loadLevel(const std::string & filename)
{
    // Reset the entity manager every time we load a level
    m_entityManager = EntityManager();

    //  Prepare to read in the level file
    std::ifstream fin(filename);
    std::string line;
    bool playerInput = false;

    //  Read in the level file and assign the attributes
    while (std::getline(fin, line)) 
    {
        std::istringstream iss(line);
        std::string firstWord;
        std::string type;
        int pos_x, pos_y;
        iss >> firstWord;

        if (firstWord == "Player") 
        {
            playerInput = true;
            iss >> m_playerConfig.X >> m_playerConfig.Y >> 
                m_playerConfig.CX >> m_playerConfig.CY >> 
                m_playerConfig.SPEED >> m_playerConfig.JUMP >> m_playerConfig.MAXSPEED >> 
                m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;
        }

        else if (firstWord == "Tile")
        {
            iss >> type >> pos_x >> pos_y;
            auto e = m_entityManager.addEntity("tile");
            
            e->addComponent<CAnimation>(m_game->assets().getAnimation(type), true);
            e->addComponent<CTransform>(gridToMidPixel(pos_x, pos_y, e));
            e->addComponent<CBoundingBox>(m_game->assets().getAnimation(type).getSize());
        }

        else if (firstWord == "Dec")
        {
            iss >> type >> pos_x >> pos_y;
            auto e = m_entityManager.addEntity("dec");
            e->addComponent<CAnimation>(m_game->assets().getAnimation(type), true);
            e->addComponent<CTransform>(gridToMidPixel(pos_x, pos_y, e));
        }

        else
        {
            std::cout << "Error in file reading";
        }
    }
    spawnPlayer();
}

//  Spawn the players character at the start of the game
//  This is also used as the players re-spawn point when they die
void Scene_Play::spawnPlayer()
{
    //  Spawn the player's on screen character with the attributes read in from the level file
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    m_player->addComponent<CTransform>(Vec2 ( gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player)), Vec2( m_playerConfig.SPEED, m_playerConfig.MAXSPEED), Vec2(1.0, 1.0), 0.0 );
    m_player->addComponent<CBoundingBox>(Vec2 (m_playerConfig.CX, m_playerConfig.CY));
}

//  Spawn bullets
void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
    //  Reference the entities Transform attributes
    auto& player = entity->getComponent<CTransform> ();
    
    //  Spawn a bullet at the entities location, going in the direction the entity is facing
	auto bullet = m_entityManager.addEntity ( "bullet" );

    bullet->addComponent<CAnimation> ( m_game->assets ().getAnimation ( m_playerConfig.WEAPON ), true );

	//  CTransform(const Vec2 & p, const Vec2 & sp, const Vec2 & sc, float a)	: pos ( p ), prevPos ( p ), velocity ( sp ), scale ( sc ), angle ( a ) {}
    bullet->addComponent<CTransform> ( Vec2 ( player.pos ), Vec2 (( m_playerConfig.SPEED * 2), (m_playerConfig.MAXSPEED * 2) ), Vec2 ( player.scale.x, 1.0 ), ( player.angle + 90.0) );
    bullet->addComponent<CBoundingBox> ( m_game->assets ().getAnimation ( m_playerConfig.WEAPON ).getSize () );
}

//  Update all entities on the screen and entity systems
void Scene_Play::update()
{
    //  Pause systems when the game is paused
	if ( m_paused )
	{
		//  No systems should update when the game is paused
	}
    else
    {
        //  Update all entities on the screen 
        m_entityManager.update ();
        //  Update all systems
        sMovement ();
        sLifespan ();
        sCollision ();
        sAnimation ();
        sRender ();
    }
}

//  Update the positions of all entities on the screen 
//  Applying gravity as needed
void Scene_Play::sMovement()
{
	auto& pTransform = m_player->getComponent<CTransform> ();                //  Get a reference to the players Transform attributes
	auto& pInput = m_player->getComponent<CInput> ();                                 //  Get a reference to the players Input attributes
	auto& playerV1 = m_player->getComponent<CTransform> ().velocity;     //  Get a reference to the players velocity
	auto    playerV2 = Vec2 ( 0.0f, 0.0f );                                                                  //  A new variable to hold the players updated velocity
	auto& state = m_player->getComponent<CState> ().state;                          //  Get a reference to the players State attributes
	auto& canJump = m_player->getComponent<CInput> ().canJump;           //  Get a reference to the players canJump attribute
   
    //  Store previous position before updating
    pTransform.prevPos = pTransform.pos;

    //  Update player movement based on input
    if (pInput.right)
    {
        //  Update the direction the player is facing
        m_player->getComponent<CTransform> ().scale = Vec2 ( 1.0f, 1.0f );
        if ( state == "Stand" ) { state = "Run"; }
        auto goR = playerV1.x + m_playerConfig.SPEED;
        if (goR <= m_playerConfig.MAXSPEED)
        {
            playerV2.x = goR;
        }
        else { playerV2.x += m_playerConfig.SPEED; }
    }
    if (pInput.left)
    {
        //  Update the direction the player is facing
        m_player->getComponent<CTransform> ().scale = Vec2 ( -1.0f, 1.0f );
        if ( state == "Stand" ) { state = "Run"; }
        auto goL = playerV1.x - m_playerConfig.SPEED;
        if (goL >= -m_playerConfig.MAXSPEED)
        {
            playerV2.x = goL;
        }
        else { playerV2.x -= m_playerConfig.SPEED; }
     }
    if (pInput.up)
    {
        //  Check if the player is already jumping
        if (canJump)
        {
            playerV1.y = m_playerConfig.JUMP;
            canJump = false;
            state = "Air";
        }
    }
    //  Apply gravity to the player when neccessary 
    if ( pInput.down ||state == "Air")
    {
        playerV2.y = playerV1.y + m_playerConfig.GRAVITY;
    }

    pTransform.velocity = playerV2;
    pTransform.pos += playerV2;

    //  Move the bullets on the map
	for ( auto e : m_entityManager.getEntities ( "bullet" ) )
	{
		auto& entityX = e->getComponent<CTransform> ();
		//  Check the direction of travel
        if ( entityX.scale.x > 0 ) {
            //  Bullet is traveling right
            entityX.pos.x += entityX.velocity.x;
        }
        else {
			//  Bullet is traveling left
			entityX.pos.x -= entityX.velocity.x;
        }
	}
}

void Scene_Play::sLifespan()
{
    // TODO:  Check all entities with a lifespan and destroy them if they go over
	
	for ( auto entity : m_entityManager.getEntities("tile") )
	{
        //  Check for an animation component
        try
        {
            auto& hasAna = entity->getComponent<CAnimation> ();
        }
        catch (...)
        {
            continue;
        }
        //  Check if the entity is at the end of it's lifespan
        bool anaEnd = entity->getComponent<CAnimation> ().animation.hasEnded ();
		 //  If entity is a question box ignore
        if ( anaEnd && ( entity->getComponent<CAnimation> ().animation.getName() != "Question" ))  
		{           
			entity->destroy();
		}
	}
}

void Scene_Play::sCollision ()
{
    //Get the players size & transform components
	auto& pSize = m_player->getComponent<CBoundingBox> ().size;
	auto& pTransform = m_player->getComponent<CTransform> ();
    
    //m_player->getComponent<CInput>().down = true;
    bool noCollisionUnder = true;

    for (auto tile : m_entityManager.getEntities("tile"))
    {
        //  Quick references to the tile
        //  auto& tAnimation = tile->getComponent<CAnimation>().animation;
        auto& tName = tile->getComponent<CAnimation>().animation.getName();
        auto& tSize = tile->getComponent<CBoundingBox>().size;
        auto& tTransform = tile->getComponent<CTransform>();

        //  Implement bullet / tile collisions
        for (auto bullet : m_entityManager.getEntities("bullet"))
        {
            //  Check for a collision with the Physics.getOverlap ()  
            //  Note: A positive number means a collision has occurred
            auto collisionCheck = (Physics::GetOverlap(bullet, tile));
            if ((collisionCheck.x) > 0 && (collisionCheck.y > 0))
            {
                //  A collision has occurred destroy the bullet
                bullet->destroy();
                //  Check if the tile is a brick which can be destroyed by a bullet
                if (tName == "Brick")
                {
                    tile->destroy();
                    tile->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
                    std::cout << "Explosion";
                }
            }
        }

        //  Perform player / tile collision check by calling getOverlap()
        auto collisionCheck = Physics::GetOverlap(m_player, tile);

        //  Note: A positive number means a collision has occurred
        if (collisionCheck.x > 0 && collisionCheck.y > 0)
        {
            //  An overlap has occured getPreviousOverlap()
            auto prevCollision = (Physics::GetPreviousOverlap(m_player, tile));

            //  Check if collision came from the top or bottom
            if (prevCollision.x > 0)
            {
                //  Check if collision came from the top
                if (pTransform.prevPos.y <= pTransform.pos.y)
                {
                    //  Top Collision: Push player back so they are standing on the item
                    pTransform.pos.y = pTransform.prevPos.y;
                    //  Set Velocity to 0
                    pTransform.velocity.y = 0.0f;
                    
                    if ( m_player->getComponent<CState> ().state == "Air" )
                    {
                        m_player->getComponent<CState> ().state = "Stand";
                    }
					if ( !m_player->getComponent<CInput> ().up )
					{
						m_player->getComponent<CInput> ().canJump = true;
					}
                    m_player->getComponent<CInput>().down = false;
                    noCollisionUnder = false;
                }
                else
                {
                    //  Bottom Collision:  move player down
                    pTransform.pos.y = pTransform.prevPos.y;
					//  Set Velocity to 0
					pTransform.velocity.y = 0.0f;

                    //  check that it isn't a side collision
                    auto tileBottom = ((tTransform.pos.y) + (tSize.y * 0.5));
                    auto playerH = (pTransform.pos.y - (pSize.y * 0.5) + 10);
                    if (playerH > tileBottom)
                    {
                        //  Check if object is a brick
                        if (tName == "Brick")
                        {
                            tile->destroy();
							//  Activate the explosion animation
							auto explode = m_entityManager.addEntity ( "tile" );
							explode->addComponent<CAnimation> ( m_game->assets ().getAnimation ( "Explosion" ), true );
							//  Position the explosion where the brick was
							explode->addComponent<CTransform> ( Vec2 ( tTransform.pos ) );
                            break;
                        }
                        //  If it is a question activate the coin animation
                        if (tName == "Question")
                        {
                            //  Transition the question box to activated texture and generate coin animation
                            tile->addComponent<CAnimation>( m_game->assets ().getAnimation ( "Question2" ), true );
                            tile->addComponent<CTransform>(Vec2(tTransform.pos));
                            //  Activate the coin animation
                            auto coin = m_entityManager.addEntity("tile");
                            coin->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"), true);
                            //  Position the coin above the question box
                            auto addH = tTransform.pos.y - tSize.y;
                            coin->addComponent<CTransform>(Vec2(tTransform.pos.x, addH));
                        }
                    }
                }
            }

            //  Check prevpos.y for a side collision
            if (prevCollision.y > 0)
            {
                auto side = pTransform.pos.x - pTransform.prevPos.x;
                //   Note there are no special circumstances if it came from left or right
                if ( side > 0)
                {
                    //  Impact came from the left
                    pTransform.pos.x = pTransform.prevPos.x;
                }
                else
                {
                    //  Impact came from the right
                    pTransform.pos.x = pTransform.prevPos.x;
                }
                
            }
            //  If there was no prev overlap (i.e. collision came diagonally) push to the side 
            else 
            {
				pTransform.pos.x = pTransform.prevPos.x;
            }
        }
    }

    if (noCollisionUnder && pTransform.velocity.y == 0.0f) {
        m_player->getComponent<CInput>().down = true;

    }
    //  Check if the players has fallen down a hole
	if ( ( pTransform.pos.y - ( pSize.y / 2 ) ) > height () )
	{
		//  Player has died, re-spawn
		m_player->destroy ();
		spawnPlayer ();
	}
    // TODO: Implement player / tile collisions and resolutions
    //       Update the CState component of the player to store whether
    //       it is currently on the ground or in the air. This will be
    //       used by the Animation system

    //  Prevent the player from walking off the left side of the map
    if ((pTransform.pos.x - (pSize.x / 2)) < 0)
    {
        pTransform.pos.x = pTransform.prevPos.x;
    }
}

//  Set the appropriate attributes to initialize the registered player input
void Scene_Play::sDoAction(const Action& action)
{
    auto & state = m_player->getComponent<CState>().state;
    auto & playerInput = m_player->getComponent<CInput>();

    //  At the start of the action, get the requested action
    if ( action.type () == "START" )
    {
        //  Game settings
                if ( action.name () == "TOGGLE_TEXTURE" )       { m_drawTextures = !m_drawTextures; }
        else if ( action.name () == "TOGGLE_COLLISION" )    { m_drawCollision = !m_drawCollision; }
        else if ( action.name () == "TOGGLE_GRID" )              { m_drawGrid = !m_drawGrid; }
        else if ( action.name () == "PAUSE" )                            { m_paused = ( m_paused ) ? false : true; }
        else if ( action.name () == "QUIT" )                              { onEnd (); }

        //  Player movement inputs
        //  These actions will not be performed while the game is paused
        if ( !m_paused )
        {
            if ( action.name () == "RIGHT" )
            {
                playerInput.right = true;
            }
            else if ( action.name () == "LEFT" )
            {
                playerInput.left = true;
            }
            else if ( action.name () == "UP" )
            {
                playerInput.up = true;
            }
            else if ( action.name () == "SHOOT" )
            {
                // Spawn a bullet at the players location
                if ( playerInput.canShoot )
                {
                    spawnBullet ( m_player );
                    playerInput.shoot = true;

                    //  Prevent the player from firing another bullet until the space bar is released
                    playerInput.canShoot = false;
                }
            }
        }
    }
    if ( !m_paused ){
        //  The Action has ended
        if ( action.type ()   == "END" )
        {
            if ( action.name () == "RIGHT" )
            {
                playerInput.right = false;
                if ( state == "Run" ) {state = "Stand";}
            }

            else if ( action.name () == "LEFT" )
            {
                playerInput.left = false;
                if ( state == "Run" ) { state = "Stand"; }
            }

            else if ( action.name () == "UP" )
			{
				playerInput.up = false;
			}

            else if ( action.name () == "SHOOT" )
            {
                playerInput.shoot = false;
                playerInput.canShoot = true;
            }
        }
    }
}
                              
//  Implement animation of entities
void Scene_Play::sAnimation()
{
    //  Set the animation of the player based on its CState component
   auto& state = m_player->getComponent<CState>().state;
   m_player->addComponent<CAnimation>(m_game->assets().getAnimation(state), true);

    //  Update each entity on screen with an animation component
    for (auto e : m_entityManager.getEntities()) 
    {
        e->getComponent<CAnimation>().animation.update();

        // if the animation is not repeated, and it has ended, destroy the entity
        if ((!e->getComponent<CAnimation>().repeat) && (e->getComponent<CAnimation>().animation.hasEnded()))
        {
            e->destroy();
        }
    }
}

void Scene_Play::onEnd()
{
    // TODO: When the scene ends, change back to the MENU scene
    m_game->changeScene("MENU", nullptr, true);
}
                              
void Scene_Play::drawLine(const Vec2 & p1, const Vec2 & p2)
{
    sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
    m_game->window().draw(line, 2, sf::Lines);
}

void Scene_Play::sRender()
{
    // color the background darker so you know that the game is paused
    if (!m_paused) { m_game->window().clear(sf::Color(100, 100, 255)); }
    else { m_game->window().clear(sf::Color(50, 50, 150)); }
                              
    // set the viewport of the window to be centered on the player if it's far enough right
    auto & pPos = m_player->getComponent<CTransform>().pos;
    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
    sf::View view = m_game->window().getView();
    view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
    m_game->window().setView(view);
        
    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        for (auto e : m_entityManager.getEntities())
        {
            auto & transform = e->getComponent<CTransform>();

            if (e->hasComponent<CAnimation>())
            {
                auto & animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                m_game->window().draw(animation.getSprite());
            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangleshape
    if (m_drawCollision)
    {
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto & box = e->getComponent<CBoundingBox>();
                auto & transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x-1, box.size.y-1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color(255, 255, 255, 255));
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }

    // draw the grid so that students can easily debug
    if (m_drawGrid)
    {
        float leftX = m_game->window().getView().getCenter().x - width() / 2;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += m_gridSize.x)
        {
            drawLine(Vec2(x, 0), Vec2(x, height()));
        }
                              
        for (float y = 0; y < height(); y += m_gridSize.y)
        {
            drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x)
            {
                std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
                m_game->window().draw(m_gridText);
            }
        }
    }
}


// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 3
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
