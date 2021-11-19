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

void Scene_Play::init(const std::string & levelPath)
{
    registerAction(sf::Keyboard::P,     "PAUSE");
    registerAction(sf::Keyboard::Escape,"QUIT");
    registerAction(sf::Keyboard::T,     "TOGGLE_TEXTURE");          // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C,     "TOGGLE_COLLISION");       // Toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::G,     "TOGGLE_GRID");                 // Toggle drawing (G)rid
                       
    registerAction(sf::Keyboard::W, "UP");                                       // Go Up
    registerAction(sf::Keyboard::A, "LEFT");                                     // Go Left
    registerAction(sf::Keyboard::D, "RIGHT");                                 // Go Right
    registerAction(sf::Keyboard::S, "DOWN");                                // Go Down
    registerAction(sf::Keyboard::Space, "SHOOT");              // Spawn a bullet when space bar is pressed


    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Arial"));

    loadLevel(levelPath);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    // TODO: This function takes in a grid (x,y) position and an Entity
    //       Return a Vec2 indicating where the CENTER position of the Entity should be
    //       You must use the Entity's Animation size to position it correctly
    //       The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y
    //       The bottom-left corner of the Animation should align with the bottom left of the grid cell
    
    const std::string& tempName = entity->getComponent<CAnimation>().animation.getName();
    auto& aSize = m_game->assets().getAnimation(tempName).getSize();
    float entityCenterX = gridX * m_gridSize.x + (aSize.x / 2);
    float entityCenterY = ((height() / m_gridSize.y) - gridY)* m_gridSize.y - (aSize.y / 2);

    return Vec2(entityCenterX, entityCenterY);
}
                              
void Scene_Play::loadLevel(const std::string & filename)
{
    // reset the entity manager every time we load a level
    m_entityManager = EntityManager();

    std::ifstream fin(filename);
    std::string line;
    bool playerInput = false;
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
    // TODO: read in the level file and add the appropriate entities
    //       use the PlayerConfig struct m_playerConfig to store player properties
    //       this struct is defined at the top of Scene_Play.h

    // NOTE: all of the code below is sample code which shows you how to
    //       set up and use entities with the new syntax, it should be removed
    if (playerInput)
    { 
    spawnPlayer();
    }
    // some sample entities
    //auto brick = m_entityManager.addEntity("tile");
    // IMPORTANT: always add the CAnimation component first so that gridToMidPixel can compute correctly
    // brick->addComponent<CAnimation>(m_game->assets().getAnimation("Brick"), true);
    // brick->addComponent<CTransform>(Vec2(96, 480));
    // NOTE: You final code should position the entity with the grid x,y position read from the file:
    // brick->addComponent<CTransform>(gridToMidPixel(gridX, gridY, brick);

   // if (brick->getComponent<CAnimation>().animation.getName() == "Brick")
    //{
      //  std::cout << "This could be a good way of identifying if a tile is a brick!\n";
    //}

    // auto block = m_entityManager.addEntity("tile");
    // block->addComponent<CAnimation>(m_game->assets().getAnimation("Block"), true);
    // block->addComponent<CTransform>(Vec2(224, 480));
    // add a bounding box, this will now show up if we press the 'C' key
    // block->addComponent<CBoundingBox>(m_game->assets().getAnimation("Block").getSize());
                              
    //auto question = m_entityManager.addEntity("tile");
    //question->addComponent<CAnimation>(m_game->assets().getAnimation("Question"), true);
    //question->addComponent<CTransform>(Vec2(352, 480));

    // NOTE: THIS IS INCREDIBLY IMPORTANT PLEASE READ THIS EXAMPLE
    //       Components are now returned as references rather than pointers
    //       If you do not specify a reference variable type, it will COPY the component
    //       Here is an example:
    //
    //       This will COPY the transform into the variable 'transform1' - it is INCORRECT
    //       Any changes you make to transform1 will not be changed inside the entity
    //       auto transform1 = entity->get<CTransform>()
    //
    //       This will REFERENCE the transform with the variable 'transform2' - it is CORRECT
    //       Now any changes you make to transform2 will be changed inside the entity
    //       auto & transform2 = entity->get<CTransform>()
}

void Scene_Play::spawnPlayer()
{
    // here is a sample player entity which you can use to construct other entities
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    m_player->addComponent<CTransform>(Vec2 ( gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player)), Vec2( m_playerConfig.SPEED, m_playerConfig.MAXSPEED), Vec2(1.0, 1.0), 0.0 );
    m_player->addComponent<CBoundingBox>(Vec2 (m_playerConfig.CX, m_playerConfig.CY));

    // TODO: be sure to add the remaining components to the player
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
    auto& angleB = entity->getComponent<CTransform> ();

    //  Spawn a bullet at the players location, going in the direction the player is facing
	auto e = m_entityManager.addEntity ( "bullet" );
	e->addComponent<CAnimation> ( m_game->assets ().getAnimation ( m_playerConfig.WEAPON ), true );
	//      CTransform(const Vec2 & p, const Vec2 & sp, const Vec2 & sc, float a)	: pos ( p ), prevPos ( p ), velocity ( sp ), scale ( sc ), angle ( a ) {}
    e->addComponent<CTransform> ( Vec2 ( angleB.pos ), Vec2 ( m_playerConfig.SPEED * 3, m_playerConfig.MAXSPEED * 3 ), Vec2 ( 1.0, 1.0 ), (angleB.angle + 90.0) );
	e->addComponent<CBoundingBox> ( m_game->assets ().getAnimation ( m_playerConfig.WEAPON ).getSize () );
}

void Scene_Play::update()
{
    m_entityManager.update();

    // TODO: implement pause functionality

    sMovement();
    sLifespan();
    sCollision();
    sAnimation();
    sRender();
}

void Scene_Play::sMovement()
{
        // TODO: Implement player movement / jumping based on its CInput component
        // TODO: Implement gravity's effect on the player
        // TODO: Implement the maximum player speed in both X and Y directions
        // NOTE: Setting an entity's scale.x to -1/1 will make it face to the left/right
        auto& pTransform = m_player->getComponent<CTransform>();
        auto& pInput = m_player->getComponent<CInput>();

        //  Store previous position before updating
        pTransform.prevPos = pTransform.pos;

        Vec2 playerV(0, 0);
    
        if (pInput.right)       {playerV.x += m_playerConfig.SPEED;}
        if (pInput.left)          {playerV.x -= m_playerConfig.SPEED;}
        if (pInput.up)           {playerV.y -= m_playerConfig.SPEED;}
        if (pInput.down)     {playerV.y += m_playerConfig.SPEED;}
        
        //  Move the bullets on the map

		for ( auto e : m_entityManager.getEntities ( "bullet" ) )
		{
			auto& entityX = e->getComponent<CTransform> ().pos;
			auto& entityV = e->getComponent<CTransform> ().velocity;
			entityX.x = entityX.x + entityV.y;
		}
     
        pTransform.velocity = playerV;
        pTransform.pos += pTransform.velocity;
}

void Scene_Play::sLifespan()
{
    // TODO: Check lifespan of entities that have them, and destroy them if they go over
}

void Scene_Play::sCollision()
{
    // REMEMBER: SFML's (0,0) position is on the TOP-LEFT corner
    //           This means jumping will have a negative y-component
    //           and gravity will have a positive y-component
    //           Also, something BELOW something else will have a y value GREATER than it
    //           Also, something ABOVE something else will have a y value LESS than it
    for ( auto tile : m_entityManager.getEntities ( "Tile" ) )
    {
        //  Implement bullet / tile collisions
        for (auto bullet : m_entityManager.getEntities ( "bullet" ) ) 
        {
            //  Check for a collision with the Physics.getOverlap ()  
            //  Note: A positive number means a collision has occurred
            auto collisionCheck = (Physics::GetOverlap( bullet, tile ));
            if ( ( collisionCheck.x ) > 0 && ( collisionCheck.y > 0 ) )
            {
                //  A collision has occurred destroy the bullet
                bullet->destroy();
                //  Check if the tile is a brick which can be destroyed by a bullet
                if ((tile->getComponent<CAnimation> ().animation.getName()) == "brick" )
                {
                    tile->destroy();
                }
            }
        }
    }

    //       Destroy the tile if it has a Brick animation
    // TODO: Implement player / tile collisions and resolutions
    //       Update the CState component of the player to store whether
    //       it is currently on the ground or in the air. This will be
    //       used by the Animation system
    // TODO: Check to see if the player has fallen down a hole (y > height())
    // TODO: Don't let the player walk off the left side of the map
}

void Scene_Play::sDoAction(const Action& action)
{
    if ( action.type () == "START" )
    {
                    if ( action.name () == "TOGGLE_TEXTURE" )       { m_drawTextures = !m_drawTextures; }
            else if ( action.name () == "TOGGLE_COLLISION" )    { m_drawCollision = !m_drawCollision; }
            else if ( action.name () == "TOGGLE_GRID" )              { m_drawGrid = !m_drawGrid; }
            else if ( action.name () == "PAUSE" )                            { setPaused ( !m_paused ); }
            else if ( action.name () == "QUIT" )                               { onEnd (); }
            else if ( action.name () == "RIGHT" )                             { m_player->getComponent<CInput> ().right = true; }
            else if ( action.name () == "LEFT" )                                { m_player->getComponent<CInput> ().left = true; }
            else if ( action.name () == "UP" )                                   { m_player->getComponent<CInput> ().up = true; }
            else if ( action.name () == "DOWN" )                            { m_player->getComponent<CInput> ().down = true; }

            else if ( action.name () == "SHOOT" ) 
			{
                        
                        if ( m_player->getComponent < CInput > ().canShoot )    //  Verify the player has released the space bar before firing a second bullet
                        {
                                spawnBullet ( m_player );
                                m_player->getComponent<CInput> ().shoot = true;
                                m_player->getComponent<CInput> ().canShoot = false;                                                            //  Prevent the player from firing another bullet until the space bar is released
                        }
			}
    }
    else if (action.type() == "END")
    {
                    if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
            else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
            else if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; }
            else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = false; }
			else if ( action.name () == "SHOOT" )
			{
                m_player->getComponent<CInput> ().shoot = false;
                m_player->getComponent<CInput> ().canShoot = true;                                                            //  Prevent the player from firing another bullet until the space bar is released
			}
    }
}
                              
void Scene_Play::sAnimation()
{
    // TODO: Complete the Animation class code first

    // TODO: set the animation of the player based on its CState component
    // TODO: for each entity with an animation, call entity->getComponent<CAnimation>().animation.update()
    //       if the animation is not repeated, and it has ended, destroy the entity
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
