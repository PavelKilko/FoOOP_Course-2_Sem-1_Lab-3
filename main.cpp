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
	olc::vf2d fPlayerPos = { 16.0f, 80.0f };
	olc::vf2d fPlayerSpeed = { 30.0f, 0.0f };

	float fGravityAcceleration = 60.0f;

	olc::vi2d vBlockSize = { 16, 16 };
	std::unique_ptr<int[]> blocks;
	
	std::unique_ptr<olc::Sprite> sprTile;
	std::unique_ptr<olc::Sprite> sprPlayer;
public:
	bool OnUserCreate() override
	{
		blocks = std::make_unique<int[]>(14 * 7);
		for (int y = 0; y < 7; y++)
		{
			for (int x = 0; x < 14; x++)
			{
				if (x == 0 || y == 0 || x == 13 || y == 6)
					blocks[y * 14 + x] = 10;
				else
					blocks[y * 14 + x] = 0;
			}
		}
		
		// Load the sprites
		sprTile = std::make_unique<olc::Sprite>("./textures/tile_0011.png");
		sprPlayer = std::make_unique<olc::Sprite>("./textures/tile_0240.png");
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
				switch (blocks[y * 14 + x])
				{
				case 0: // Do nothing
					break;
				case 10: // Draw Boundary
					DrawSprite(olc::vi2d(x, y) * vBlockSize, sprTile.get());
					break;
				}
			}
		}
		// Draw Player Spite
		DrawSprite(fPlayerPos, sprPlayer.get());
		return true;
	}
};

int main()
{
		Platformer demo;
		if (demo.Construct(224, 112, 4, 4))
			demo.Start();
		return 0;
}