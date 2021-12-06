#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Platformer : public olc::PixelGameEngine
{
public:
	Platformer()
	{
		sAppName = "Platformer";
	}

private:
	// Player parametrs
	olc::vf2d fPlayerStartPos = { 0.0f, 0.0f };
	olc::vf2d fPlayerPos = { 16.0f, 80.0f };
	olc::vf2d fPlayerSpeed = { 40.0f, 0.0f };
	
	bool bRightMove = false;
	bool bLeftMove = false;
	bool bUpMove = false;
	bool bDownMove = false;
    bool bSpace = false;
	
	// Map parametrs
	olc::vi2d vMapSize = { 0, 0 };
	struct Block
	{
		int BlockId;
		olc::vi2d vRealBlockPos;
		olc::vi2d vRealBlockSize;
	};
	olc::vi2d vBlockSize = { 16, 16 };
	std::unique_ptr<Block[]> blocks;
	std::unique_ptr<olc::Sprite> sprTileMap;
	std::unique_ptr<olc::Sprite> sprTileTranspMap;
	
	// Map physics
	float fGravityAcceleration = 60.0f;
public:
	bool OnUserCreate() override
	{
		// Load the sprites
		sprTileMap = std::make_unique<olc::Sprite>("./textures/monochrome_tilemap_packed.png");
		sprTileTranspMap = std::make_unique<olc::Sprite>("./textures/monochrome_tilemap_transparent_packed.png");
		return true;
	}
	
	bool OnUserUpdate(float fElapsedTime) override
	{
		std::cout << fPlayerPos << "\n"; 
		// Handle User Input 
		if (GetKey(olc::Key::R).bPressed)
		{
			fPlayerSpeed.y = 0.0f;
			fPlayerPos = fPlayerStartPos;
			bRightMove = false;
			bLeftMove = false;
			bUpMove = false;
			bDownMove = false;
			bSpace = false;
		}
		if (GetKey(olc::Key::LEFT).bHeld) 
		{
			fPlayerPos.x -= fPlayerSpeed.x * fElapsedTime;
			bLeftMove = true;
		}
		if (GetKey(olc::Key::RIGHT).bHeld)
		{
			fPlayerPos.x += fPlayerSpeed.x * fElapsedTime;
			bRightMove = true;
		}
		if (GetKey(olc::Key::SPACE).bPressed)
			if (!bSpace)
			{
				fPlayerSpeed.y = 60.0f;
				bSpace = true;
			}
		if (fPlayerSpeed.y > 0) bUpMove = true;
		if (fPlayerSpeed.y < 0) bDownMove = true;
		
		// Gravity emulation
		fPlayerPos.y = fPlayerPos.y - fPlayerSpeed.y * fElapsedTime + fGravityAcceleration * fElapsedTime * fElapsedTime / 2.0f;
		fPlayerSpeed.y -= fGravityAcceleration * fElapsedTime; 
/*
		// Collisions check
		if (fPlayerPos.x < 16.0f) fPlayerPos.x = 16.0f;
		if (fPlayerPos.x + 16.0f > 208.0f) fPlayerPos.x = 192.0f;
		if (fPlayerPos.y < 16.0f) fPlayerPos.y = 16.0f;
		if (fPlayerPos.y + 16.0f > 96.0f) 
		{
			fPlayerPos.y = 80.0f;
			fPlayerSpeed.y = 0.0f;
		}
*/		
		// New Collisions check
		while (1)
		{
			if (!HaveCollision())
				break;
		}
			
		// Draw Game Field
		Clear(olc::BLACK);
		for (int y = 0; y < vMapSize.y; y++)
		{
			for (int x = 0; x < vMapSize.x; x++)
			{
				DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, 
								  sprTileMap.get(), 
								  olc::vi2d(blocks[y * vMapSize.x + x].BlockId % 20, blocks[y * vMapSize.x + x].BlockId / 20) * vBlockSize, 
								  vBlockSize);
			}
		}
		
		
		// Draw Player Spite
		SetPixelMode(olc::Pixel::MASK);
		DrawPartialSprite(fPlayerPos, sprTileTranspMap.get(), olc::vi2d(0, 17) * vBlockSize, vBlockSize);
		SetPixelMode(olc::Pixel::NORMAL);
		
		bRightMove = false;
		bLeftMove = false;
		bUpMove = false;
		bDownMove = false;
		
		return true;
	}
	
	void LoadTileMap()
	{
		// Setup Map Size
		std::cin >> vMapSize.x >> vMapSize.y;
		std::cin >> fPlayerStartPos.x >> fPlayerStartPos.y;
		blocks = std::make_unique<Block[]>(vMapSize.x * vMapSize.y);
		// Setup Types of Blocks
		for (int y = 0; y < vMapSize.y; y++)
		{
			for (int x = 0; x < vMapSize.x; x++)
			{
				std::cin >> blocks[y * vMapSize.x + x].BlockId;
				blocks[y * vMapSize.x + x].vRealBlockPos = { 0, 0 };
				blocks[y * vMapSize.x + x].vRealBlockSize = { 16, 16 };
			}
		}
	}
	
	bool HaveCollision()
	{
		bool bDead = false; 
		bool bLevelComplete = false;
		// Check left-up corner
		olc::vi2d vBlockPos = { int(fPlayerPos.x) / 16, int(fPlayerPos.y) / 16 };
		int blockNumber = vBlockPos.y * vMapSize.x + vBlockPos.x;
		if (blocks[blockNumber].BlockId == 11)
		{
			if (int(fPlayerPos.x) >= vBlockPos.x * 16 && int(fPlayerPos.x) <= (vBlockPos.x + 1) * 16 &&
			    int(fPlayerPos.y) >= vBlockPos.y * 16 && int(fPlayerPos.y) <= (vBlockPos.y + 1) * 16)
			{
				if (bLeftMove)
				{
					fPlayerPos.x = float((vBlockPos.x + 1) * 16);
					bLeftMove = false;
					return true;
				}
				if (bUpMove)
				{
					fPlayerPos.y = float((vBlockPos.y + 1) * 16);
					bUpMove = false;
					bDownMove = true;
					fPlayerSpeed.y = 0.0f;
					return true;
				}
			}
		}
		if (blocks[blockNumber].BlockId == 7)
		{
			bDead = true;
		}
		if (blocks[blockNumber].BlockId == 59)
		{
			bLevelComplete = true;
		}
		// Check right-up corner
		vBlockPos = { int(fPlayerPos.x) / 16 + 1, int(fPlayerPos.y) / 16 };
		blockNumber = vBlockPos.y * vMapSize.x + vBlockPos.x;
		if (blocks[blockNumber].BlockId == 11)
		{
			if (int(fPlayerPos.x + 15) >= vBlockPos.x * 16 && int(fPlayerPos.x + 15) <= (vBlockPos.x + 1) * 16 &&
			    int(fPlayerPos.y) >= vBlockPos.y * 16 && int(fPlayerPos.y) <= (vBlockPos.y + 1) * 16)
			{
				if (bRightMove)
				{
					fPlayerPos.x = float((vBlockPos.x - 1) * 16);
					bRightMove = false;
					return true;
				}
				if (bUpMove)
				{
					fPlayerPos.y = float((vBlockPos.y + 1) * 16);
					bUpMove = false;
					bDownMove = true;
					fPlayerSpeed.y = 0.0f;
					return true;
				}
			}
		}
		if (blocks[blockNumber].BlockId == 7)
		{
			bDead = true;
		}
		if (blocks[blockNumber].BlockId == 59)
		{
			bLevelComplete = true;
		}
		// Check left-down corner
		vBlockPos = { int(fPlayerPos.x) / 16, int(fPlayerPos.y) / 16 + 1};
		blockNumber = vBlockPos.y * vMapSize.x + vBlockPos.x;
		if (blocks[blockNumber].BlockId == 11)
		{
			if (int(fPlayerPos.x) >= vBlockPos.x * 16 && int(fPlayerPos.x) <= (vBlockPos.x + 1) * 16 &&
			    int(fPlayerPos.y + 15) >= vBlockPos.y * 16 && int(fPlayerPos.y + 15) <= (vBlockPos.y + 1) * 16)
			{
				if (bDownMove)
				{
					fPlayerPos.y = float((vBlockPos.y - 1) * 16);
					bDownMove = false;
					bSpace = false;
					fPlayerSpeed.y = 0.0f;
					return true;
				}
			}
		}
		if (blocks[blockNumber].BlockId == 7)
		{
			bDead = true;
		}
		if (blocks[blockNumber].BlockId == 59)
		{
			bLevelComplete = true;
		}
		// Check right-down corner
		vBlockPos = { int(fPlayerPos.x) / 16 + 1, int(fPlayerPos.y) / 16 + 1};
		blockNumber = vBlockPos.y * vMapSize.x + vBlockPos.x;
		if (blocks[blockNumber].BlockId == 11)
		{
			if (int(fPlayerPos.x + 15) >= vBlockPos.x * 16 && int(fPlayerPos.x + 15) <= (vBlockPos.x + 1) * 16 &&
			    int(fPlayerPos.y + 15) >= vBlockPos.y * 16 && int(fPlayerPos.y + 15) <= (vBlockPos.y + 1) * 16)
			{
				if (bDownMove)
				{
					fPlayerPos.y = float((vBlockPos.y - 1) * 16);
					bDownMove = false;
					bSpace = false;
					fPlayerSpeed.y = 0.0f;
					return true;
				}
			}
		}
		if (blocks[blockNumber].BlockId == 7)
		{
			bDead = true;
		}
		if (blocks[blockNumber].BlockId == 59)
		{
			bLevelComplete = true;
		}
		if (bDead)
		{
			fPlayerSpeed.y = 0.0f;
			fPlayerPos = fPlayerStartPos;
			bSpace = false;
		}
		else if (bLevelComplete)
		{
			exit(1);
		}
		return false;
	}
};



int main()
{
		Platformer demo;
		if (demo.Construct(176, 304, 4, 4))
		{
			demo.LoadTileMap();
			demo.Start();
		}
		return 0;
}