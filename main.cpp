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
	olc::vi2d vBlockSize = { 16, 16 };
	std::unique_ptr<int[]> blocks;
	
	std::unique_ptr<olc::Sprite> sprTile;

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
		
		// Load the sprite
		sprTile = std::make_unique<olc::Sprite>("./textures/tile_0011.png");
		return true;
	}
	
	bool OnUserUpdate(float fElapsedTime) override
	{
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