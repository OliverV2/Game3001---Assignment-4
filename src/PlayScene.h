#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__



#include "Scene.h"
#include "Plane.h"
#include "Player.h"
#include "Button.h"
#include "DebugKeys.h"
#include "Obstacle.h"
#include "PathNode.h"
#include "GameObject.h"
#include "Button.h"



class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Scene LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;
private:
	// PRIVATE FUNCTIONS
	void m_buildGrid();

	void m_displayGrid();

	void m_displayGridLOS();

	void m_setGridLOS();

	// MEMBER VARIABLES
	glm::vec2 m_mousePosition;

	Plane* m_pPlaneSprite;
	Plane* m_pPlaneSprite2;
	Player* m_pPlayer;
	bool m_playerFacingRight;
	bool m_bPlayerHasLOS;
	bool m_bPlayerHasLOS1;
	bool m_bEnemyHasLOS;
	bool m_bEnemyHasLOS1;
	bool m_bdetection;
	bool m_bdetection2;
	bool fleenow1;
	bool fleenow2;
	Obstacle* m_pObstacle;

	Tile* m_pTile;


	bool m_bDebugMode;
	bool m_bPatrolMode;

	bool m_bDebugKeys[NUM_OF_DEBUG_KEYS];
	bool is_hit;

	std::vector<PathNode*> m_pGrid;

	//Lab 1 Code
	std::vector<PathNode*> m_pPatrolPath;
	void m_buildClockwisePatrolPath();
	void m_displayPatrolPath();
	int m_targetPathNodeIndex;
	int m_targetPathNodeIndex1;
	int Plane1hlth, Plane2hlth;
	void m_bflee();
	void cover();
	void range();
	void close();
	PathNode* m_pTargetPathNode;
	PathNode* m_pTargetPathNode1;
	void m_movePlaneToTargetNode();
	
	
};

#endif /* defined (__PLAY_SCENE__) */