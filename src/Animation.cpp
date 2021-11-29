///\/\\////\///////\////\\\\\\/\\/\/\\////\///////\////\\\\\\/\\
//
//  Assignment       COMP4300 - Assignment 3
//  Professor:       David Churchill
//  Year / Term:     2021FALL
//  File Name:       Animation.cpp
// 
//  Student Name:    Mahdi Rabie
//  Student User:    mr1826
//  Student Email:   mr1826@mun.ca
//  Student ID:      201025756
//  Group Member(s): [enter student name(s)]
//
///\/\\////\///////\////\\\\\\/\\/\/\\////\///////\////\\\\\\/\\
                              
#include "Animation.h"
#include <cmath>

Animation::Animation()
{
}

Animation::Animation(const std::string & name, const sf::Texture & t)
    : Animation(name, t, 1, 0)
{

}

Animation::Animation(const std::string & name, const sf::Texture & t, size_t frameCount, size_t speed)
    : m_name                (name)
    , m_sprite                (t)
    , m_frameCount     (frameCount)     //  Total frames of animation for the entity
    , m_currentFrame  (0)
    , m_speed               (speed)                 //  Speed of the animation indicates time duration of each frame in each frame
{
    m_size = Vec2((float)t.getSize().x / frameCount, (float)t.getSize().y);     //  Each individual frame size in the animation
    m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
    m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
}

size_t counter = 0;
size_t Animation::RunCounter() {
    
    if (counter == 10)
    {
        counter = 0;
    }
    else {
        counter++;
    }
    return counter;
}

// updates the animation to show the next frame, depending on its speed
// animation loops when it reaches the end
void Animation::update()
{
    auto frame = 0;

	if ( m_name == "Run" )
	{
        //  Create another var for frame count because not sure where the frame keeps resetting
        
        m_currentFrame =  RunCounter();
        //  Getting strange error where currentFrame resets to zero on increment 3
//         if (m_currentFrame == 2)
//         {
//             m_currentFrame++;
//         }
        
		//  Calculate the correct frame of animation to play based on currentFrame and speed
        float fT = float(m_currentFrame) / float(m_speed);    //    % complete
        frame = floor ( fT * m_frameCount );
		//frame = ( m_currentFrame / m_speed ) % m_frameCount;
		//  Set the texture rectangle properly
		m_sprite.setTextureRect ( sf::IntRect ( frame * m_size.x, 0, m_size.x, m_size.y ) );
	}
    else 
    {

		
		//  Prevent division by 0 error
		if ( m_speed > 0 )
		{	
            //  Increment the life cycle the current animation has lived for
            m_currentFrame++;
			//  Calculate the correct frame of animation to play based on currentFrame and speed
			frame = ( m_currentFrame / m_speed ) % m_frameCount;
			//  Set the texture rectangle properly
			m_sprite.setTextureRect ( sf::IntRect ( frame * m_size.x, 0, m_size.x, m_size.y ) );
		}
    }
}
const Vec2 & Animation::getSize() const
{
    return m_size;
}

const std::string & Animation::getName() const
{
    return m_name;
}

sf::Sprite & Animation::getSprite()
{
    return m_sprite;
}

bool Animation::hasEnded() const
{
    //  Detect when animation has ended (last frame was played) and return true
    bool done =  (m_currentFrame == m_frameCount) ?   true : false;
    return done;
}

// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 3
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
