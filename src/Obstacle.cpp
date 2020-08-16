#include "Obstacle.h"

#include "TextureManager.h"

Obstacle::Obstacle()
{
	TextureManager::Instance()->load("../Assets/textures/obstacletile.png", "obstacle");

	const auto size = TextureManager::Instance()->getTextureSize("obstacle");
	setWidth(size.x);
	setHeight(size.y);
	getTransform()->position = glm::vec2(300.0f, 400.0f);
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;

	setType(OBSTACLE);
}

Obstacle::~Obstacle()
= default;

void Obstacle::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the target
	TextureManager::Instance()->draw("obstacle", x, y, 0, 255, false);
}

void Obstacle::update()
{
}

void Obstacle::clean()
{
}


Tile::Tile()
{
	TextureManager::Instance()->load("../Assets/textures/grasstile.png", "tile");

	const auto size = TextureManager::Instance()->getTextureSize("tile");
	setWidth(size.x);
	setHeight(size.y);
	getTransform()->position = glm::vec2(300.0f, 400.0f);
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;

	setType(TILE);
}

Tile::~Tile()
= default;

void Tile::draw()
{
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the target
	TextureManager::Instance()->draw("tile", x, y, 0, 255, false);
}

void Tile::update()
{
}

void Tile::clean()
{
}

