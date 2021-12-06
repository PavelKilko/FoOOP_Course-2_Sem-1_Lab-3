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
	olc::vi2d vMapSize = { 0, 0 };

	olc::vf2d fPlayerPos = { 16.0f, 80.0f };
	olc::vf2d fPlayerSpeed = { 40.0f, 0.0f };

	float fGravityAcceleration = 60.0f;

	olc::vi2d vBlockSize = { 16, 16 };
	std::unique_ptr<int[]> blocks;
	
	std::unique_ptr<olc::Sprite> sprTileMap;
public:
	bool OnUserCreate() override
	{
		// Load the sprites
		sprTileMap = std::make_unique<olc::Sprite>("./textures/monochrome_tilemap_packed.png");
		return true;
	}
	
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Handle User Input 
		if (GetKey(olc::Key::LEFT).bHeld) fPlayerPos.x -= fPlayerSpeed.x * fElapsedTime;
		if (GetKey(olc::Key::RIGHT).bHeld) fPlayerPos.x += fPlayerSpeed.x * fElapsedTime;
		if (GetKey(olc::Key::SPACE).bPressed)
			if (fPlayerSpeed.y == 0.0f)
				fPlayerSpeed.y = 60.0f;
		
		// Gravity emulation
		fPlayerPos.y = fPlayerPos.y - fPlayerSpeed.y * fElapsedTime + fGravityAcceleration * fElapsedTime * fElapsedTime / 2.0f;
		if (fPlayerSpeed.y != 0.0f) fPlayerSpeed.y -= fGravityAcceleration * fElapsedTime; 
		
		// Collisions check
		if (fPlayerPos.x < 16.0f) fPlayerPos.x = 16.0f;
		if (fPlayerPos.x + 16.0f > 208.0f) fPlayerPos.x = 192.0f;
		if (fPlayerPos.y < 16.0f) fPlayerPos.y = 16.0f;
		if (fPlayerPos.y + 16.0f > 96.0f) 
		{
			fPlayerPos.y = 80.0f;
			fPlayerSpeed.y = 0.0f;
		}
		
		
		
		// Draw Game Field
		Clear(olc::BLACK);
		for (int y = 0; y < 7; y++)
		{
			for (int x = 0; x < 14; x++)
			{
				DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, sprTileMap.get(), olc::vi2d(blocks[y * 14 + x] % 20, blocks[y * 14 + x] / 20) * vBlockSize, vBlockSize);
			}
		}
		
		// Draw Player Spite
		DrawPartialSprite(fPlayerPos, sprTileMap.get(), olc::vi2d(0, 17) * vBlockSize, vBlockSize);
		return true;
	}
	
	void LoadTileMap()
	{
		// Setup Map Size
		std::cin >> vMapSize.x >> vMapSize.y;
		blocks = std::make_unique<int[]>(vMapSize.x * vMapSize.y);
		// Setup Types of Blocks
		for (int y = 0; y < vMapSize.y; y++)
		{
			for (int x = 0; x < vMapSize.x; x++)
			{
				std::cin >> blocks[y * vMapSize.x + x];
			}
		}
	}
};



int main()
{
		Platformer demo;
		if (demo.Construct(224, 112, 4, 4))
		{
			demo.LoadTileMap();
			demo.Start();
		}
		return 0;
}