#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include "Util.h"
#include "Renderer.h"

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	
	drawDisplayList();

	if(m_bDebugMode)
	{
		auto LOSColour = (!m_bPlayerHasLOS) ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

		Util::DrawLine(m_pPlayer->getTransform()->position, m_pPlaneSprite->getTransform()->position, LOSColour);

		Util::DrawRect(m_pPlayer->getTransform()->position - glm::vec2(m_pPlayer->getWidth() * 0.5f, m_pPlayer->getHeight() *0.5f),
			m_pPlayer->getWidth(), m_pPlayer->getHeight());

		Util::DrawRect(m_pPlaneSprite->getTransform()->position - glm::vec2(m_pPlaneSprite->getWidth() * 0.5f, m_pPlaneSprite->getHeight() * 0.5f),
			m_pPlaneSprite->getWidth(), m_pPlaneSprite->getHeight());

		/*Util::DrawRect(m_pObstacle->getTransform()->position - glm::vec2(m_pObstacle->getWidth() * 0.5f, m_pObstacle->getHeight() * 0.5f),
			m_pObstacle->getWidth(), m_pObstacle->getHeight());*/
			
		m_displayGrid();

		//m_displayGridLOS();
		
	}
}

void PlayScene::update()
{
	updateDisplayList();

	m_bPlayerHasLOS = CollisionManager::LOSCheck(m_pPlayer, m_pPlaneSprite, m_pObstacle);

	CollisionManager::AABBCheck(m_pPlayer, m_pPlaneSprite);

	CollisionManager::AABBCheck(m_pPlayer, m_pObstacle);

	m_setGridLOS();


	m_movePlaneToTargetNode();
}

void PlayScene::clean()
{
	removeAllChildren();

}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

	// handle player movement with GameController
	if (SDL_NumJoysticks() > 0)
	{
		if (EventManager::Instance().getGameController(0) != nullptr)
		{
			const auto deadZone = 10000;
			if (EventManager::Instance().getGameController(0)->LEFT_STICK_X > deadZone)
			{
				m_pPlayer->setAnimationState(PLAYER_RUN_RIGHT);
				m_playerFacingRight = true;

				m_pPlayer->getRigidBody()->velocity = glm::vec2(5.0f, 0.0f);
				m_pPlayer->getTransform()->position += m_pPlayer->getRigidBody()->velocity;
				m_pPlayer->getRigidBody()->velocity *= m_pPlayer->getRigidBody()->velocity * 0.9f;
			}
			else if (EventManager::Instance().getGameController(0)->LEFT_STICK_X < -deadZone)
			{
				m_pPlayer->setAnimationState(PLAYER_RUN_LEFT);
				m_playerFacingRight = false;

				m_pPlayer->getRigidBody()->velocity = glm::vec2(-5.0f, 0.0f);
				m_pPlayer->getTransform()->position += m_pPlayer->getRigidBody()->velocity;
				m_pPlayer->getRigidBody()->velocity *= m_pPlayer->getRigidBody()->velocity * 0.9f;
			}
			else
			{
				if (m_playerFacingRight)
				{
					m_pPlayer->setAnimationState(PLAYER_IDLE_RIGHT);
				}
				else
				{
					m_pPlayer->setAnimationState(PLAYER_IDLE_LEFT);
				}
			}
		}
	}


	// handle player movement if no Game Controllers found
	if (SDL_NumJoysticks() < 1)
	{
		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_A))
		{
			m_pPlayer->setAnimationState(PLAYER_RUN_LEFT);
			m_playerFacingRight = false;

			SoundManager::Instance().playSound("steps", 0, 1);

			m_pPlayer->getRigidBody()->velocity = glm::vec2(-5.0f, 0.0f);
			m_pPlayer->getTransform()->position += m_pPlayer->getRigidBody()->velocity;
			m_pPlayer->getRigidBody()->velocity *= m_pPlayer->getRigidBody()->velocity * 0.9f;
		}
		else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
		{
			m_pPlayer->setAnimationState(PLAYER_RUN_RIGHT);
			m_playerFacingRight = true;

			SoundManager::Instance().playSound("steps", 0, 1);

			m_pPlayer->getRigidBody()->velocity = glm::vec2(5.0f, 0.0f);
			m_pPlayer->getTransform()->position += m_pPlayer->getRigidBody()->velocity;
			m_pPlayer->getRigidBody()->velocity *= m_pPlayer->getRigidBody()->velocity * 0.9f;
		}
		else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_W))
		{
			m_pPlayer->setAnimationState(PLAYER_RUN_LEFT);


			m_pPlayer->getRigidBody()->velocity = glm::vec2(0.0f, -5.0f);
			m_pPlayer->getTransform()->position += m_pPlayer->getRigidBody()->velocity;
			m_pPlayer->getRigidBody()->velocity *= m_pPlayer->getRigidBody()->velocity * 0.9f;
		}

		else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_S))
		{
			m_pPlayer->setAnimationState(PLAYER_RUN_LEFT);


			m_pPlayer->getRigidBody()->velocity = glm::vec2(0.0f, 5.0f);
			m_pPlayer->getTransform()->position += m_pPlayer->getRigidBody()->velocity;
			m_pPlayer->getRigidBody()->velocity *= m_pPlayer->getRigidBody()->velocity * 0.9f;
		}
		else
		{
			if (m_playerFacingRight)
			{
				m_pPlayer->setAnimationState(PLAYER_IDLE_RIGHT);
			}
			else
			{
				m_pPlayer->setAnimationState(PLAYER_IDLE_LEFT);
			}
		}
	}

	// H KEY Section
	
	if(!m_bDebugKeys[H_KEY])
	{
		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_H))
		{
			// toggles Debug Mode
			m_bDebugMode = !m_bDebugMode;

			m_bDebugKeys[H_KEY] = true;

			if (m_bDebugMode)
			{
				std::cout << "DEBUG Mode On" << std::endl;
			}
			else
			{
				std::cout << "DEBUG Mode Off" << std::endl;
			}
		}
	}

	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_H))
	{
		m_bDebugKeys[H_KEY] = false;
	}

	// K KEY Section
	
	if (!m_bDebugKeys[K_KEY])
	{
		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_K))
		{
			std::cout << "DEBUG: Enemies taking damage!" << std::endl;

			m_bDebugKeys[K_KEY] = true;
		}
	}

	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_K))
	{
		m_bDebugKeys[K_KEY] = false;
	}

	// P KEY Section

	if (!m_bDebugKeys[P_KEY])
	{
		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_P))
		{
			// toggles Patrol Mode
			m_bPatrolMode = !m_bPatrolMode;

			m_bDebugKeys[P_KEY] = true;

			if (m_bPatrolMode)
			{
				std::cout << "DEBUG: Patrol Mode On" << std::endl;			
			}
			else
			{
				std::cout << "DEBUG: Patrol Mode Off" << std::endl;
			}
		}
	}

	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_P))
	{
		m_bDebugKeys[P_KEY] = false;
	}
	

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance()->quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance()->changeSceneState(START_SCENE);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		TheGame::Instance()->changeSceneState(END_SCENE);
	}
}

void PlayScene::m_buildGrid()
{
	// Logic to add PathNodes to the scene
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			auto pathNode = new PathNode();
			pathNode->getTransform()->position = glm::vec2(pathNode->getWidth() * col + Config::TILE_SIZE * 0.5, pathNode->getHeight() * row + Config::TILE_SIZE * 0.5);
			m_pGrid.push_back(pathNode);
		}
	}

	std::cout << "Number of Nodes: " << m_pGrid.size() << std::endl;
}

void PlayScene::m_displayGrid()
{
	// Logic to add PathNodes to the scene
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			auto Colour = (!m_pGrid[row * Config::COL_NUM + col]->getLOS()) ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			
			Util::DrawRect(m_pGrid[row * Config::COL_NUM + col]->getTransform()->position - glm::vec2(m_pGrid[row * Config::COL_NUM + col]->getWidth() * 0.5f, m_pGrid[row * Config::COL_NUM + col]->getHeight() * 0.5f),
				Config::TILE_SIZE, Config::TILE_SIZE);

			Util::DrawRect(m_pGrid[row * Config::COL_NUM + col]->getTransform()->position,
				5, 5, Colour);
		}
	}
}

void PlayScene::m_displayGridLOS()
{
	for (auto node : m_pGrid)
	{
		if(!node->getLOS())
		{
			auto colour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

			Util::DrawLine(node->getTransform()->position, m_pPlayer->getTransform()->position, colour);
		}
		
	}
}

void PlayScene::m_setGridLOS()
{
	for (auto node : m_pGrid)
	{
		node->setLOS(CollisionManager::LOSCheck(node, m_pPlayer, m_pObstacle));
	}
}

void PlayScene::m_buildClockwisePatrolPath()
{
	//moving right
	for (auto i = 0; i< Config::COL_NUM; i++)
	{
		m_pPatrolPath.push_back(m_pGrid[i]);
	}

	//moving down
	for (auto i = 1; i < Config::ROW_NUM; i++)
	{
		m_pPatrolPath.push_back(m_pGrid[i * Config::COL_NUM + Config::COL_NUM - 1]);
	}

	//moving left
	for (auto i = 1; i < Config::COL_NUM; i++)
	{
		m_pPatrolPath.push_back(m_pGrid[Config::COL_NUM * Config::ROW_NUM - 1 - i]);
	}

	//move up
	for (auto i = Config::ROW_NUM - 2; i > 0; i--)
	{
		m_pPatrolPath.push_back(m_pGrid[i * Config::COL_NUM]);
	}
}

void PlayScene::m_displayPatrolPath()
{
	for (auto node : m_pPatrolPath)
	{
		std::cout << "(" << node->getTransform()->position.x << "," << node->getTransform()->position.y << ")" << std::endl;
	}
}

void PlayScene::m_movePlaneToTargetNode()
{
	if (m_bPatrolMode)
	{
		m_pTargetPathNode = m_pPatrolPath[m_targetPathNodeIndex];
		auto targetVector = Util::normalize(m_pTargetPathNode->getTransform()->position - m_pPlaneSprite->getTransform()->position);

		if (targetVector.x == 1)
		{
			m_pPlaneSprite->setAngle(90.0f);
		}

		else if (targetVector.x == -1)
		{
			m_pPlaneSprite->setAngle(-90.0f);
		}

		if (targetVector.y == 1)
		{
			m_pPlaneSprite->setAngle(180.0f);
		}

		else if (targetVector.y == -1)
		{
			m_pPlaneSprite->setAngle(0.0f);
		}

		m_pPlaneSprite->getRigidBody()->velocity = targetVector;
		m_pPlaneSprite->getTransform()->position += m_pPlaneSprite->getRigidBody()->velocity * m_pPlaneSprite->getRigidBody()->maxSpeed;
		if (m_pPlaneSprite->getTransform()->position == m_pTargetPathNode->getTransform()->position)
		{
			m_targetPathNodeIndex++;
			if (m_targetPathNodeIndex > m_pPatrolPath.size() -1)
			{
				m_targetPathNodeIndex = 0;
			}
		}
	}
}


void PlayScene::start()
{
	m_bPlayerHasLOS = false;

	SoundManager::Instance().allocateChannels(16);
	SoundManager::Instance().setMusicVolume(40);
	SoundManager::Instance().setSoundVolume(40);
	SoundManager::Instance().load("audio/Sun.wav", "sun", SOUND_MUSIC);
	SoundManager::Instance().playMusic("sun", -1, 0);
	SoundManager::Instance().load("audio/Footstep_Dirt_00.wav", "steps", SOUND_SFX);


	m_buildGrid();

	m_buildClockwisePatrolPath();
	/*m_displayPatrolPath();*/
	m_targetPathNodeIndex = 1;
	
	
	m_bDebugMode = false;
	m_bPatrolMode = false;

	//Tile Texture
	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(0.0f, 0.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(80.0f, 0.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(160.0f, 0.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(240.0f, 0.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(320.0f, 0.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(400.0f, 0.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(480.0f, 0.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(560.0f, 0.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(640.0f, 0.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(720.0f, 0.0f);
	addChild(m_pTile);

	//Tile Texture
	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(0.0f, 80.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(80.0f, 80.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(160.0f, 80.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(240.0f, 80.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(320.0f, 80.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(400.0f, 80.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(480.0f, 80.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(560.0f, 80.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(640.0f, 80.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(720.0f, 80.0f);
	addChild(m_pTile);


	//Tile Texture
	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(0.0f, 160.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(80.0f, 160.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(160.0f, 160.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(240.0f, 160.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(320.0f, 160.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(400.0f, 160.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(480.0f, 160.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(560.0f, 160.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(640.0f, 160.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(720.0f, 160.0f);
	addChild(m_pTile);


	//Tile Texture
	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(0.0f, 240.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(80.0f, 240.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(160.0f, 240.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(240.0f, 240.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(320.0f, 240.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(400.0f, 240.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(480.0f, 240.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(560.0f, 240.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(640.0f, 240.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(720.0f, 240.0f);
	addChild(m_pTile);

	//Tile Texture
	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(0.0f, 320.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(80.0f, 320.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(160.0f, 320.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(240.0f, 320.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(320.0f, 320.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(400.0f, 320.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(480.0f, 320.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(560.0f, 320.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(640.0f, 320.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(720.0f, 320.0f);
	addChild(m_pTile);


	//Tile Texture
	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(0.0f, 400.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(80.0f, 400.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(160.0f, 400.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(240.0f, 400.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(320.0f, 400.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(400.0f, 400.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(480.0f, 400.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(560.0f, 400.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(640.0f, 400.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(720.0f, 400.0f);
	addChild(m_pTile);

	//Tile Texture
	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(0.0f, 480.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(80.0f, 480.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(160.0f, 480.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(240.0f, 480.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(320.0f, 480.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(400.0f, 480.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(480.0f, 480.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(560.0f, 480.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(640.0f, 480.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(720.0f, 480.0f);
	addChild(m_pTile);

	//Tile Texture
	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(0.0f, 560.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(80.0f, 560.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(160.0f, 560.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(240.0f, 560.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(320.0f, 560.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(400.0f, 560.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(480.0f, 560.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(560.0f, 560.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(640.0f, 560.0f);
	addChild(m_pTile);

	m_pTile = new Tile();
	m_pTile->setWidth((80.0f));
	m_pTile->setHeight(80.0f);
	m_pTile->getTransform()->position = glm::vec2(720.0f, 560.0f);
	addChild(m_pTile);

	// Obstacle Texture
	m_pObstacle = new Obstacle();
	m_pObstacle->setWidth(80.0f);
	m_pObstacle->setHeight(80.0f);
	m_pObstacle->getTransform()->position = glm::vec2(400.0f, 160.0f);
	addChild(m_pObstacle);

	m_pObstacle = new Obstacle();
	m_pObstacle->setWidth(80.0f);
	m_pObstacle->setHeight(80.0f);
	m_pObstacle->getTransform()->position = glm::vec2(480.0f, 160.0f);
	addChild(m_pObstacle);

	// Plane Sprite
	m_pPlaneSprite = new Plane();
	m_pPlaneSprite->getTransform()->position = m_pPatrolPath[0]->getTransform()->position;
	m_pPlaneSprite->getRigidBody()->maxSpeed = 5.0f;
	addChild(m_pPlaneSprite);

	// Player Sprite
	m_pPlayer = new Player();
	m_pPlayer->getTransform()->position = glm::vec2(600.0f, 440.0f);
	addChild(m_pPlayer);
	m_playerFacingRight = true;
}


