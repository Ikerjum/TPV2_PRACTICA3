// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

// Components list - must have at least one element
//
struct Transform;
struct Image;
struct Points;
struct ImageWithFrames;
struct MiracleFruit;
struct Health;

#define _CMPS_LIST_ \
	Transform, \
	Image, \
	Points, \
	ImageWithFrames, \
	MiracleFruit, \
	Health

// Groups list - must have at least one element
//
#define _GRPS_LIST_ \
	GHOSTS, \
	FRUITS

// Handlers list - must have at least one element
//
#define _HDLRS_LIST_ \
    PACMAN

// Systems list - must have at least one element
//
class FoodSystem;
class RenderSystem;
class PacManSystem;
class GameCtrlSystem;
class CollisionsSystem;
class GhostSystem;
class ImmunitySystem;

#define _SYS_LIST_ \
    FoodSystem, \
	RenderSystem, \
	PacManSystem, \
	GameCtrlSystem, \
	CollisionsSystem, \
	GhostSystem, \
	ImmunitySystem

