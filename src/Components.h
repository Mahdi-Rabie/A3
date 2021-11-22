///\/\\////\///////\////\\\\\\/\\/\/\\////\///////\////\\\\\\/\\
//
//  Assignment       COMP4300 - Assignment 3
//  Professor:       David Churchill
//  Year / Term:     2021FALL
//  File Name:       Components.h
// 
//  Student Name:    Mahdi Rabie
//  Student User:    mr1826
//  Student Email:   mr1826@mun.ca
//  Student ID:      201025756
//  Group Member(s): [enter student name(s)]
//
///\/\\////\///////\////\\\\\\/\\/\/\\////\///////\////\\\\\\/\\
                              
#pragma once

#include "Animation.h"
#include "Assets.h"

class Component
{
public:
    bool has = false;
};

class CTransform : public Component
{
public:
    Vec2 pos        = { 0.0, 0.0 };
    Vec2 prevPos    = { 0.0, 0.0 };
    Vec2 scale      = { 1.0, 1.0 };
    Vec2 velocity   = { 0.0, 0.0 };
    float angle     = 0;

    CTransform() {}
    CTransform(const Vec2 & p)
        : pos(p) {}
    CTransform(const Vec2 & p, const Vec2 & sp, const Vec2 & sc, float a)
        : pos(p), prevPos(p), velocity(sp), scale(sc), angle(a) {}

};

class CLifeSpan : public Component
{
public:
    int lifespan = 0;
    int frameCreated = 0;
    CLifeSpan() {}
    CLifeSpan(int duration, int frame) 
        : lifespan(duration), frameCreated(frame) {}
};

class CInput : public Component
{
public:
    bool up         = false;
    bool down       = false;
    bool left       = false;
    bool right      = false;
    bool shoot      = false;
    bool canShoot   = true;
    bool canJump    = true;

    CInput() {}
};

class CBoundingBox : public Component
{
public:
    Vec2 size;
    Vec2 halfSize;
    CBoundingBox() {}
    CBoundingBox(const Vec2 & s)
        : size(s), halfSize(s.x / 2, s.y / 2) {}
};

class CAnimation : public Component
{
public:
    Animation animation;
    bool repeat = false;
    CAnimation() {}
    CAnimation(const Animation & animation, bool r)
        : animation(animation), repeat(r) {}
};

class CGravity : public Component
{
public:
    float gravity = 0;
    CGravity() {}
    CGravity(float g) : gravity(g) {}
};
                              
class CState : public Component
{
public:
    std::string state = "Air";
    CState() {}
    CState(const std::string & s) : state(s) {}
};


// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 3
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
