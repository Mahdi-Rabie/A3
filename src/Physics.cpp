///\/\\////\///////\////\\\\\\/\\/\/\\////\///////\////\\\\\\/\\
//
//  Assignment       COMP4300 - Assignment 3
//  Professor:       David Churchill
//  Year / Term:     2021FALL
//  File Name:       Physics.cpp
// 
//  Student Name:    Mahdi Rabie
//  Student User:    mr1826
//  Student Email:   mr1826@mun.ca
//  Student ID:      201025756
//  Group Member(s): [enter student name(s)]
//
///\/\\////\///////\////\\\\\\/\\/\/\\////\///////\////\\\\\\/\\

#include "Physics.h"
#include "Components.h"
//		Return the overlap rectangle size of the bounding boxes of entity a and b
Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    auto	aCheckB = a->hasComponent<CBoundingBox>();
	auto	bCheckB = b->hasComponent<CBoundingBox> ();
	//		Check if the entity has a bounding box
	if (!aCheckB || !bCheckB)
	{
		//		The entity has no bounding box
		return Vec2 ( 0, 0 );	
	}
	//	The entity has a bounding box so continue
    //  Declare local vars
    auto ox = 0.0;
    auto oy = 0.0;
//     Vec2 overlap ;

    //  Get a reference to the two entities position & bounding boxes
    auto& aPos = a->getComponent<CTransform>().pos;
    auto& bPos = b->getComponent<CTransform> ().pos;
	auto& aBox = a->getComponent<CBoundingBox>().halfSize;
	auto& bBox = b->getComponent<CBoundingBox>().halfSize;


    //  Calculate overlap & return the size of the overlapped rectangle
    auto deltaX = abs ( aPos.x - bPos.x );
    auto deltaY = abs ( aPos.y - bPos.y );
    auto delta = (deltaX, deltaY);

    ox = aBox.x + bBox.x - deltaX;
    oy = aBox.y + bBox.y - deltaY;
    auto overlap = Vec2(ox, oy);

    return overlap;
}

  //	Return the previous overlap rectangle size of the bounding boxes of entity a and b
Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{ 
	auto aCheckB = a->hasComponent<CBoundingBox> ();
	auto bCheckB = b->hasComponent<CBoundingBox> ();
	//		Check if the entity has a bounding box
	if ( !aCheckB || !bCheckB )
	{
		//		The entity has no bounding box
		return Vec2 ( 0, 0 );
	}
	
	//	The entity has a bounding box so continue
	// Declare local vars
	auto ox = 0.0;
	auto oy = 0.0;
	
	//  Get a reference to the two entities position & bounding boxes
	auto& aPos = a->getComponent<CTransform> ().prevPos;
	auto& bPos = b->getComponent<CTransform> ().prevPos;
	auto& aBox = a->getComponent<CBoundingBox> ().halfSize;
	auto& bBox = b->getComponent<CBoundingBox> ().halfSize;

	//  Calculate overlap & return the size of the overlapped rectangle
	auto deltaX = abs ( aPos.x - bPos.x );
	auto deltaY = abs ( aPos.y - bPos.y );
	auto delta = ( deltaX, deltaY );

	ox = abs(aBox.x + bBox.x - deltaX);
	oy = abs(aBox.y + bBox.y - deltaY);
	auto overlap = Vec2 ( ox, oy );

	return overlap;
}


// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 3
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
