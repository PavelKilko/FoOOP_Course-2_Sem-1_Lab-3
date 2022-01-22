#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "stdio.h"
#include "mem.h"

// Compile code with mingw in console:
// cd your_file_path
// g++ -o main.exe main.cpp -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -lstdc++fs -lDwmapi -static -std=c++17
// main.exe

class NewPlatformer : public olc::PixelGameEngine
{
// Parameters of class NewPlatformer
private:

	// Program status
		// 0 - main menu
		// 1 - game
		// 2 - level editor
	int iNewPlatformerStatus = 0;

	// Window cell (in abbreviated form - wc) size 
	olc::vi2d viWindowCellSize = { 16, 16 };

	// Block
	struct Block
	{
		// Block status
			// 0 - decorative block
			// 1 - physical block
			// 2 - "exit" block
			// 3 - "spike" block
			// 4 - player sprite
		int iStatus = 0;
		olc::vi2d viExternalSize = { 0, 0 };
		olc::vi2d viInternalSize = { 0, 0 };
		olc::vi2d viInternalSizePosition = { 0, 0 };
	};

	// Block data
	Block blkData[400];

// Methods of class NewPlatformer
private:

	void update_block_data()
	{
		char cBlockDataFileName[20] = "parameters.txt";
		FILE *fpBlockDataFile = fopen(cBlockDataFileName, "r");
		for (int i = 0; i < 400; i++)
		{
			fscanf(fpBlockDataFile, 
				   "%d%d%d%d%d%d%d",
				   &blkData[i].iStatus,
				   &blkData[i].viExternalSize.x,
				   &blkData[i].viExternalSize.y,
				   &blkData[i].viInternalSize.x,
				   &blkData[i].viInternalSize.y,
				   &blkData[i].viInternalSizePosition.x,
				   &blkData[i].viInternalSizePosition.y);
		} 
	}

// Description of subclass MainMenu:
// Parameters of subclass MainMenu
private:

	// Main menu options
	std::string sMainMenuOptions[3] = {"Game", "Level editor", "Exit"};

	// Cursor main menu status
		// 0 - game
		// 1 - level editor
		// 2 - exit
	int iCursorMainMenuStatus = 0;


// Methods of subclass MainMenu
private:

	void check_main_menu_buttons()
	{
		// Input with arrow keys:
		if(GetKey(olc::Key::DOWN).bPressed)	iCursorMainMenuStatus == 2 ? iCursorMainMenuStatus = 0 : iCursorMainMenuStatus++;
		if(GetKey(olc::Key::UP).bPressed)   iCursorMainMenuStatus == 0 ? iCursorMainMenuStatus = 2 : iCursorMainMenuStatus--;
		// Input with miscellaneous keys:
		if(GetKey(olc::Key::ENTER).bPressed)
		{
			switch(iCursorMainMenuStatus)
			{
				case 0: iNewPlatformerStatus = 1; break; // Game
				case 1: iNewPlatformerStatus = 2; break; // Level editor
				case 2: exit(0); break; // Exit
			}
		}
	}

	void show_main_menu()
	{
		check_main_menu_buttons();
		Clear(olc::BLACK);
		for (int i = 0; i < 3; i++) DrawString(1, 1+8*i, (i==iCursorMainMenuStatus?">>":"  ")+sMainMenuOptions[i]);
	}




// Description of subclass Game:
// Parameters of subclass Game
private:

	// Game status
		// 0 - game level opening menu
		// 1 - game window
	int iGameStatus = 0;


	// Game level opening menu:

		// Game level file name
		std::string sGameLevelFileName = "";

		// Game caret position
		int iGameCaretPosition = 0;


	// Game window:

		// Player:

			// Game start map player position (in wc)
			olc::vi2d viGameStartMapPlayerPosition = { 0, 0 };

			// Game map player position (in px)
			olc::vf2d vfGameMapPlayerPosition = { 0.0f, 0.0f }; 

			// Game map player last position (in px)
			olc::vf2d vfGameMapPlayerLastPosition = { 0.0f, 0.0f };

			// Game player speed (in px)
			olc::vf2d vfGamePlayerSpeed = { 64.0f, 0.0f };

			// Game player gravity acceleration (in px)
			float fGamePlayerGravityAcceleration = 96.0f; 

			// Player up side move
			bool bPlayerUpSideMove = false;
			
			// Player down side move
			bool bPlayerDownSideMove = false;
			
			// Player right side move
			bool bPlayerRightSideMove = false;
			
			// Player left side move
			bool bPlayerLeftSideMove = false;

			// Player space key status
			bool bIsSpaceKeyPressed = false; 

			// Player upper left corner collision
			int iPlayerUpperLeftCornerCollision = 0;

			// Player upper right corner collision
			int iPlayerUpperRightCornerCollision = 0;

			// Player lower left corner collision
			int iPlayerLowerLeftCornerCollision = 0;

			// Player lower right corner collision 
			int iPlayerLowerRightCornerCollision = 0;

			// Player sprites
			int iPlayerSprites[7] = { 360, 361, 362, 363, 364, 365, 366 };

			// Player status
				// 0 - check status
				// 1 - first move status
				// 2 - second move status
				// 3 - in jump status
				// 4 - before jump status
				// 5 - after jump status
				// 6 - game over status
			int iPlayerStatus = 0;


		// Game map:

			// Game map size (in wc)
			olc::vi2d viGameMapSize = { 34, 21 };

			// Game map 
			int iGameMap[714] = {};

			// Game map window position (in wc)
			olc::vi2d viGameMapWindowPosition = { 6, 2 };	


// Methods of subclass Game
private:

	bool check_game_buttons(float fElapsedTime = 0.0f)
	{
		if(iGameStatus == 0) // Game level opening menu
		{
			// Input with arrow keys:
			if(GetKey(olc::Key::LEFT).bPressed)  if(iGameCaretPosition) iGameCaretPosition--;
			if(GetKey(olc::Key::RIGHT).bPressed) if(iGameCaretPosition < sGameLevelFileName.size()) iGameCaretPosition++; 			
			// Input with standart alphanumeric keys:
			if(GetKey(olc::Key::A).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'A') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'a');
			if(GetKey(olc::Key::B).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'B') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'b');
			if(GetKey(olc::Key::C).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'C') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'c');
			if(GetKey(olc::Key::D).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'D') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'd');
			if(GetKey(olc::Key::E).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'E') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'e');
			if(GetKey(olc::Key::F).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'F') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'f');
			if(GetKey(olc::Key::G).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'G') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'g');
			if(GetKey(olc::Key::H).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'H') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'h');
			if(GetKey(olc::Key::I).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'I') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'i');
			if(GetKey(olc::Key::J).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'J') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'j');
			if(GetKey(olc::Key::K).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'K') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'k');
			if(GetKey(olc::Key::L).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'L') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'l');
			if(GetKey(olc::Key::M).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'M') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'm');
			if(GetKey(olc::Key::N).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'N') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'n');
			if(GetKey(olc::Key::O).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'O') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'o');
			if(GetKey(olc::Key::P).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'P') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'p');
			if(GetKey(olc::Key::Q).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'Q') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'q');
			if(GetKey(olc::Key::R).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'R') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'r');
			if(GetKey(olc::Key::S).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'S') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 's');
			if(GetKey(olc::Key::T).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'T') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 't');
			if(GetKey(olc::Key::U).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'U') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'u');
			if(GetKey(olc::Key::V).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'V') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'v');
			if(GetKey(olc::Key::W).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'W') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'w');
			if(GetKey(olc::Key::X).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'X') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'x');
			if(GetKey(olc::Key::Y).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'Y') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'y');
			if(GetKey(olc::Key::Z).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sGameLevelFileName.insert(iGameCaretPosition++, 1, 'Z') : sGameLevelFileName.insert(iGameCaretPosition++, 1, 'z');
			// Input with standart numeric keys:
			if(GetKey(olc::Key::K0).bPressed) sGameLevelFileName.insert(iGameCaretPosition++, 1, '0');
			if(GetKey(olc::Key::K1).bPressed) sGameLevelFileName.insert(iGameCaretPosition++, 1, '1');
			if(GetKey(olc::Key::K2).bPressed) sGameLevelFileName.insert(iGameCaretPosition++, 1, '2');
			if(GetKey(olc::Key::K3).bPressed) sGameLevelFileName.insert(iGameCaretPosition++, 1, '3');
			if(GetKey(olc::Key::K4).bPressed) sGameLevelFileName.insert(iGameCaretPosition++, 1, '4');
			if(GetKey(olc::Key::K5).bPressed) sGameLevelFileName.insert(iGameCaretPosition++, 1, '5');
			if(GetKey(olc::Key::K6).bPressed) sGameLevelFileName.insert(iGameCaretPosition++, 1, '6');
			if(GetKey(olc::Key::K7).bPressed) sGameLevelFileName.insert(iGameCaretPosition++, 1, '7');
			if(GetKey(olc::Key::K8).bPressed) sGameLevelFileName.insert(iGameCaretPosition++, 1, '8');
			if(GetKey(olc::Key::K9).bPressed) sGameLevelFileName.insert(iGameCaretPosition++, 1, '9');
			// Input with miscellaneous keys:
			if(GetKey(olc::Key::SPACE).bPressed) sGameLevelFileName.insert(iGameCaretPosition++, 1, ' ');
			if(GetKey(olc::Key::BACK).bPressed) if(sGameLevelFileName.size()) sGameLevelFileName.erase(--iGameCaretPosition, 1);
			if(GetKey(olc::Key::DEL).bPressed)  if(sGameLevelFileName.size()) sGameLevelFileName.erase(iGameCaretPosition, 1);	
			if(GetKey(olc::Key::ESCAPE).bPressed) 
			{
				iNewPlatformerStatus = 0;
				sGameLevelFileName = "";
				iGameCaretPosition = 0;
				show_main_menu();
			}
			if(GetKey(olc::Key::ENTER).bPressed)
			{
				sGameLevelFileName += ".lvl";
				return true;	
			}
			return false;
		}
		if(iGameStatus == 1) // Game window
		{
			// Input with keys:
			if(GetKey(olc::Key::LEFT).bHeld)
			{
				vfGameMapPlayerPosition.x -= vfGamePlayerSpeed.x * fElapsedTime;
				bPlayerLeftSideMove = true;
				bPlayerRightSideMove = false;
			}
			if(GetKey(olc::Key::RIGHT).bHeld)
			{
				vfGameMapPlayerPosition.x += vfGamePlayerSpeed.x * fElapsedTime;
				bPlayerLeftSideMove = false;
				bPlayerRightSideMove = true;
			}
			if(GetKey(olc::Key::SPACE).bPressed && !bIsSpaceKeyPressed)
			{
				vfGamePlayerSpeed.y = 96.0f;
				bIsSpaceKeyPressed = true;
			}
			if(GetKey(olc::Key::ESCAPE).bPressed)
			{
				reset_game();
				iGameStatus = 0;
				iNewPlatformerStatus = 0;
				sGameLevelFileName = "";
				iGameCaretPosition = 0;
				show_main_menu();
			}
			return false;
		}
		return false;
	}

	void open_game_level_file(std::string sGameLevelFileName)
	{
		char cGameLevelFileName[sGameLevelFileName.size()+1];
		strcpy(cGameLevelFileName, sGameLevelFileName.c_str());
		FILE *fpGameLevelFile = fopen(cGameLevelFileName, "rb");
		fread(&viGameMapSize.x, sizeof(int), 1, fpGameLevelFile);
		fread(&viGameMapSize.y, sizeof(int), 1, fpGameLevelFile);
		fread(iGameMap, sizeof(int), viGameMapSize.x*viGameMapSize.y, fpGameLevelFile);
		fread(&viGameStartMapPlayerPosition.x, sizeof(int), 1, fpGameLevelFile);
		fread(&viGameStartMapPlayerPosition.y, sizeof(int), 1, fpGameLevelFile);
		fclose(fpGameLevelFile);
	}

	void show_game_level_opening_menu()
	{
		if(sGameLevelFileName[iGameCaretPosition] == '_') sGameLevelFileName.erase(iGameCaretPosition, 1);
		if(check_game_buttons())
		{
			if(is_exist_level_file_name(sGameLevelFileName))
			{
				open_game_level_file(sGameLevelFileName);
				update_block_data();
				vfGameMapPlayerPosition = olc::vf2d(viGameStartMapPlayerPosition)*olc::vf2d(viWindowCellSize)+olc::vf2d(blkData[360].viInternalSizePosition);
				iGameStatus = 1;
			}
			else sGameLevelFileName.erase(sGameLevelFileName.size()-4, 4);
		}
		else
		{
			Clear(olc::BLACK);
			DrawString(1, 1, "Game level opening");
			if(!is_exist_level_file_name(sGameLevelFileName+".lvl")) DrawString(1, 17, "Level file not found", olc::RED);
			else DrawString(1, 17, "Level file found", olc::GREEN);
			sGameLevelFileName.insert(iGameCaretPosition, 1, '_');
			DrawString(1, 9, "Enter existing game level name: "+sGameLevelFileName);
		}
	}

	void check_game_parameters(float fElapsedTime)
	{
		// Player up/down side move check
		if(vfGamePlayerSpeed.y >= 0) 
		{ 
			bPlayerDownSideMove = false;
			bPlayerUpSideMove = true;
		}
		if(vfGamePlayerSpeed.y < 0) 
		{
			bPlayerDownSideMove = true;
			bPlayerUpSideMove = false;
		}

		// Gravity emulation
		if(bPlayerDownSideMove || bPlayerUpSideMove)
		{
			vfGameMapPlayerPosition.y -= vfGamePlayerSpeed.y*fElapsedTime;
			vfGameMapPlayerPosition.y += fGamePlayerGravityAcceleration*fElapsedTime*fElapsedTime/2.0f;
			vfGamePlayerSpeed.y -= fGamePlayerGravityAcceleration*fElapsedTime;	
		}

		check_collisions(fElapsedTime);

		// Reset move parameters
		bPlayerRightSideMove = false;
		bPlayerLeftSideMove = false;
		bPlayerUpSideMove = false;
		bPlayerDownSideMove = false;
	}

	void check_collisions(float fElapsedTime)
	{
		// The player is outside the map area 
		if(vfGameMapPlayerPosition.x < 0 || vfGameMapPlayerPosition.x+float(blkData[360].viInternalSize.x) > viGameMapSize.x*16.0f
		|| vfGameMapPlayerPosition.y < 0 || vfGameMapPlayerPosition.y+float(blkData[360].viInternalSize.y) > viGameMapSize.y*16.0f)
		{
			reset_game();
		}

		detect_collisions();

		process_collisions(fElapsedTime);
	}

	void reset_game()
	{
		vfGamePlayerSpeed = olc::vf2d(64.0f, 0);
		vfGameMapPlayerPosition = olc::vf2d(viGameStartMapPlayerPosition)*olc::vf2d(viWindowCellSize)+olc::vf2d(blkData[360].viInternalSizePosition);
		bPlayerRightSideMove = false;
		bPlayerLeftSideMove =  false;
		bPlayerUpSideMove =    false;
		bPlayerDownSideMove =  false;
		bIsSpaceKeyPressed =   false;
	}

	bool detect_collisions()
	{
		// // Var 1
		// int iUpperLeftCornerBlockNumber =  int(vfGameMapPlayerPosition.y)/16*viGameMapSize.x+int(vfGameMapPlayerPosition.x)/16;
		// int iUpperRightCornerBlockNumber = iUpperLeftCornerBlockNumber+1;
		// int iLowerLeftCornerBlockNumber =  iUpperLeftCornerBlockNumber+viGameMapSize.x;
		// int iLowerRightCornerBlockNumber = iLowerLeftCornerBlockNumber+1;

		// Var 2
		int iUpperLeftCornerBlockNumber =  int(vfGameMapPlayerPosition.y)/16*viGameMapSize.x+int(vfGameMapPlayerPosition.x)/16;
		int iUpperRightCornerBlockNumber = int(vfGameMapPlayerPosition.y)/16*viGameMapSize.x+(int(vfGameMapPlayerPosition.x)+blkData[360].viInternalSize.x-1)/16;
		int iLowerLeftCornerBlockNumber =  (int(vfGameMapPlayerPosition.y)+blkData[360].viInternalSize.y-1)/16*viGameMapSize.x+int(vfGameMapPlayerPosition.x)/16;
		int iLowerRightCornerBlockNumber = (int(vfGameMapPlayerPosition.y)+blkData[360].viInternalSize.y-1)/16*viGameMapSize.x+(int(vfGameMapPlayerPosition.x)+blkData[360].viInternalSize.x-1)/16;


		iPlayerUpperLeftCornerCollision =  detect_collision(iUpperLeftCornerBlockNumber);
		iPlayerUpperRightCornerCollision = detect_collision(iUpperRightCornerBlockNumber);
		iPlayerLowerLeftCornerCollision =  detect_collision(iLowerLeftCornerBlockNumber);
		iPlayerLowerRightCornerCollision = detect_collision(iLowerRightCornerBlockNumber);

		if(!iPlayerLowerLeftCornerCollision && !iPlayerLowerRightCornerCollision && !iPlayerUpperLeftCornerCollision && !iPlayerUpperRightCornerCollision)
		return false;

		std::cout << "{" << std::endl;
		std::cout << iPlayerUpperLeftCornerCollision << iPlayerUpperRightCornerCollision << std::endl;
		std::cout << iPlayerLowerLeftCornerCollision << iPlayerLowerRightCornerCollision << std::endl;
		std::cout << "}" << std::endl;

		return true;
	}

	int detect_collision(int iMapBlockNumber = 0)
	{
		if(iMapBlockNumber > 713) return 0;
		// Block object
		Block blkObject = blkData[iGameMap[iMapBlockNumber]];
		
		if(blkObject.iStatus == 0) return 0;

		olc::vf2d vfObjectMinimumPosition = olc::vf2d(olc::vi2d(iMapBlockNumber%viGameMapSize.x, iMapBlockNumber/viGameMapSize.x)*viWindowCellSize)+olc::vf2d(blkObject.viInternalSizePosition);
		olc::vf2d vfObjectMaximumPosition = vfObjectMinimumPosition+olc::vf2d(blkObject.viInternalSize);

		// Block player
		Block blkPlayer = blkData[iPlayerSprites[iPlayerStatus]];
		olc::vf2d vfPlayerMinimumPosition = vfGameMapPlayerPosition;
		olc::vf2d vfPlayerMaximumPosition = vfPlayerMinimumPosition+olc::vf2d(blkPlayer.viInternalSize)-olc::vf2d(1.0f, 1.0f);

		if(vfPlayerMaximumPosition.x < vfObjectMinimumPosition.x || vfPlayerMinimumPosition.x > vfObjectMaximumPosition.x) return 0;
		if(vfPlayerMaximumPosition.y < vfObjectMinimumPosition.y || vfPlayerMinimumPosition.y > vfObjectMaximumPosition.y) return 0;
		
		return blkObject.iStatus;
	}

	void process_collisions(float fElapsedTime) 
	{
		if(((iPlayerLowerLeftCornerCollision == 1 && iPlayerLowerRightCornerCollision == 1) || (iPlayerLowerLeftCornerCollision == 1) || (iPlayerLowerRightCornerCollision == 1))
		&& bPlayerDownSideMove)
		{
			vfGameMapPlayerPosition.y -= 1.0f;
			if(!detect_collisions()) 
			{
				vfGamePlayerSpeed.y = 0.0f;
				bIsSpaceKeyPressed = false;
				bPlayerDownSideMove = false;
				return;
			}
			//else process_collisions(fElapsedTime);
		}
		if(((iPlayerUpperLeftCornerCollision == 1 && iPlayerUpperRightCornerCollision == 1) || (iPlayerUpperLeftCornerCollision == 1) || (iPlayerUpperRightCornerCollision == 1))
		&& bPlayerUpSideMove)
		{
			vfGameMapPlayerPosition.y += 1.0f;
			if(!detect_collisions())
			{
				vfGamePlayerSpeed.y = 0.0f;
				bPlayerUpSideMove = false;
				return;
			}
			//else process_collisions(fElapsedTime);
		}
		if(((iPlayerLowerLeftCornerCollision == 1 && iPlayerUpperLeftCornerCollision == 1) || (iPlayerLowerLeftCornerCollision == 1) || (iPlayerUpperLeftCornerCollision == 1)) 
		&& bPlayerLeftSideMove)
		{
			vfGameMapPlayerPosition.x += 1.0f;
			if(!detect_collisions())
			{
				bPlayerLeftSideMove = false;
				return;
			}
		}
		if(((iPlayerLowerRightCornerCollision == 1 && iPlayerUpperRightCornerCollision == 1) || (iPlayerLowerRightCornerCollision == 1) || (iPlayerUpperRightCornerCollision == 1)) 
		&& bPlayerRightSideMove)
		{
			vfGameMapPlayerPosition.x -= 1.0f;
			if(!detect_collisions())
			{
				bPlayerRightSideMove = false;
				return;
			}
		}
		if(iPlayerUpperLeftCornerCollision == 3 || iPlayerUpperRightCornerCollision == 3 || iPlayerLowerLeftCornerCollision == 3 || iPlayerLowerRightCornerCollision == 3)
		{
			reset_game();
			return;
		}
		if(iPlayerUpperLeftCornerCollision == 2 || iPlayerUpperRightCornerCollision == 2 || iPlayerLowerLeftCornerCollision == 2 || iPlayerLowerRightCornerCollision == 2)
		{
			reset_game();
			iGameStatus = 0;
			iNewPlatformerStatus = 0;
			sGameLevelFileName = "";
			iGameCaretPosition = 0;
			show_main_menu();
			return;
		}
	}

	void show_game_window(float fElapsedTime)
	{
		// // Update game map player last position
		// vfGameMapPlayerLastPosition = vfGameMapPlayerPosition;

		check_game_buttons(fElapsedTime);
		
		check_game_parameters(fElapsedTime);

		// Drawing game map
		Clear(olc::BLACK);
		DrawRect((viGameMapWindowPosition)*viWindowCellSize-olc::vi2d(2, 2), viGameMapSize*viWindowCellSize+olc::vi2d(3, 3));
		for (int i = 0; i < viGameMapSize.y; i++)
		{
			for (int j = 0; j < viGameMapSize.x; j++)
			{
				DrawPartialSprite((viGameMapWindowPosition+olc::vi2d(j, i))*viWindowCellSize,
								  sprTileMap.get(),
								  olc::vi2d(iGameMap[viGameMapSize.x*i+j]%20, iGameMap[viGameMapSize.x*i+j]/20)*viWindowCellSize,
								  viWindowCellSize); 
			}
		}

		// Drawing game map player position - test version
		DrawPartialSprite(olc::vf2d(viGameMapWindowPosition*viWindowCellSize)+vfGameMapPlayerPosition,
						  sprTileMap.get(),
						  olc::vi2d(0, 18)*viWindowCellSize+blkData[360].viInternalSizePosition, 
						  blkData[360].viInternalSize);
	}

	void show_game(float fElapsedTime)
	{
		switch(iGameStatus)
		{
			case 0: show_game_level_opening_menu(); break;
			case 1: show_game_window(fElapsedTime); break;
		}
	}




// Description of subclass LevelEditor:
// Parameters of subclass LevelEditor
private:

	// Level editor status
		// 0 - level editor menu
		// 1 - level creation menu
			// 3 - level creation exception window 
		// 2 - level opening menu
			// 4 - level opening exception window
		// 5 - level editor window
	int iLevelEditorStatus = 0;


	// Level editor menu:

		// Level editor menu options
		std::string sLevelEditorMenuOptions[3] = {"Create a new level", "Open an existing level", "Back"};

		// Cursor level editor menu status
			// 0 - create a new level
			// 1 - open an existing level
			// 2 - back
		int iCursorLevelEditorMenuStatus = 0;


	// Level creation/opening menu:

		// Level file name 
		std::string sLevelFileName = "";
		
		// Caret position
		int iCaretPosition = 0;


	// Level creation exception window:

		// Level creation exception window options
		std::string sLevelCreationExceptionWindowOptions[3] = {"Rewrite and open level", "Open level", "Back"};

		// Cursor level creation exception window status
			// 0 - rewrite and open level
			// 1 - open level
			// 2 - back to level creation menu
		int iCursorLevelCreationExceptionWindowStatus = 0;


	// Level opening exception window:

		// Level opening exception window
		std::string sLevelOpeningExceptionWindowOptions[2] = {"Create and open level", "Back"};

		// Cursor level opening exception window status
			// 0 - create and open level
			// 1 - back to level opening menu
		int iCursorLevelOpeningExceptionWindowStatus = 0;


	// Level editor window:

		// Mouse window position (in wc)
		olc::vi2d viMouseWindowPosition = { 0, 0 };


		// Level file save status:

			// Level file save status 
			bool bIsLevelFileSaved = true;

			// Level file save status window position (in wc)
			olc::vi2d viLevelFileSaveStatusWindowPosition = { 21, 2 }; // { 33, 2 }


		// Editor map:

			// Editor map size (in wc)
			olc::vi2d viEditorMapSize = { 34, 21 };

			// Editor map 
			int iEditorMap[714] = {};

			// Editor map window position (in wc)
			olc::vi2d viEditorMapWindowPosition = { 10, 4 };

			// Sprite tile map
			std::unique_ptr<olc::Sprite> sprTileMap = std::make_unique<olc::Sprite>("./textures/monochrome_tilemap_packed.png");

			// Sprite tile transparent map
			std::unique_ptr<olc::Sprite> sprTileTransparentMap = std::make_unique<olc::Sprite>("./textures/monochrome_tilemap_transparent_packed.png");

			// Editor start map player position
			olc::vi2d viEditorStartMapPlayerPosition = { 0, 0 }; 


		// Selected block:

			// Selected block code
			int iSelectedBlockCode = 0;

			// Selected block window position (in wc)
			olc::vi2d viSelectedBlockWindowPosition = { 3, 19 };


		// Exit button:

			// Exit button window position (in wc)
			olc::vi2d viExitButtonWindowPosition = { 1, 1 };

			// Exit button size (in wc)
			olc::vi2d viExitButtonSize = { 8, 2 };


		// Save button:

			// Save button window position (in wc)
			olc::vi2d viSaveButtonWindowPosition = { 11, 1 };

			// Save button size (in wc)
			olc::vi2d viSaveButtonSize = { 8, 2 }; 


		// Clear button:

			// Clear button window position (in wc)
			olc::vi2d viClearButtonWindowPosition = { 33, 1 }; // { 21, 1 }

			// Clear button size (in wc)
			olc::vi2d viClearButtonSize = { 10, 2 };


		// Editor block map:

			// Editor block map page status
			int iEditorBlockMapPageStatus = 0;

			// Editor block map size (in wc)
			olc::vi2d viEditorBlockMapSize = { 8, 10 };

			// Editor block map window position (in wc)
			olc::vi2d viEditorBlockMapWindowPosition = {1, 6};


		// Back button:

			// Back button window position (in wc)
			olc::vi2d viBackButtonWindowPosition = { 2, 17 };

			// Back button size (in wc)
			olc::vi2d viBackButtonSize = { 2, 1 };


		// Next button:

			// Next button window position (in wc)
			olc::vi2d viNextButtonWindowPosition = { 6, 17 };

			// Next button size (in wc)
			olc::vi2d viNextButtonSize = { 2, 1 };


// Methods of subclass LevelEditor
private:

	bool check_level_editor_buttons()
	{
		if(iLevelEditorStatus == 0) // Level editor menu
		{
			// Input with arrow keys:
			if(GetKey(olc::Key::DOWN).bPressed)	iCursorLevelEditorMenuStatus == 2 ? iCursorLevelEditorMenuStatus = 0 : iCursorLevelEditorMenuStatus++;
			if(GetKey(olc::Key::UP).bPressed)   iCursorLevelEditorMenuStatus == 0 ? iCursorLevelEditorMenuStatus = 2 : iCursorLevelEditorMenuStatus--;
			// Input with miscellaneous keys:
			if(GetKey(olc::Key::ENTER).bPressed)
			{
				switch(iCursorLevelEditorMenuStatus)
				{
					case 0: iLevelEditorStatus = 1; break; // Level creation menu
					case 1: iLevelEditorStatus = 2; break; // Level opening menu
					case 2: iNewPlatformerStatus = 0; break; // Back
				}
			}	
			return false;
		}
		if(iLevelEditorStatus == 1 || iLevelEditorStatus == 2) // Level creation/opening menu
		{
			// Input with arrow keys:
			if(GetKey(olc::Key::LEFT).bPressed)  if(iCaretPosition) iCaretPosition--;
			if(GetKey(olc::Key::RIGHT).bPressed) if(iCaretPosition < sLevelFileName.size()) iCaretPosition++; 			
			// Input with standart alphanumeric keys:
			if(GetKey(olc::Key::A).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'A') : sLevelFileName.insert(iCaretPosition++, 1, 'a');
			if(GetKey(olc::Key::B).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'B') : sLevelFileName.insert(iCaretPosition++, 1, 'b');
			if(GetKey(olc::Key::C).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'C') : sLevelFileName.insert(iCaretPosition++, 1, 'c');
			if(GetKey(olc::Key::D).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'D') : sLevelFileName.insert(iCaretPosition++, 1, 'd');
			if(GetKey(olc::Key::E).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'E') : sLevelFileName.insert(iCaretPosition++, 1, 'e');
			if(GetKey(olc::Key::F).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'F') : sLevelFileName.insert(iCaretPosition++, 1, 'f');
			if(GetKey(olc::Key::G).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'G') : sLevelFileName.insert(iCaretPosition++, 1, 'g');
			if(GetKey(olc::Key::H).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'H') : sLevelFileName.insert(iCaretPosition++, 1, 'h');
			if(GetKey(olc::Key::I).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'I') : sLevelFileName.insert(iCaretPosition++, 1, 'i');
			if(GetKey(olc::Key::J).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'J') : sLevelFileName.insert(iCaretPosition++, 1, 'j');
			if(GetKey(olc::Key::K).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'K') : sLevelFileName.insert(iCaretPosition++, 1, 'k');
			if(GetKey(olc::Key::L).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'L') : sLevelFileName.insert(iCaretPosition++, 1, 'l');
			if(GetKey(olc::Key::M).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'M') : sLevelFileName.insert(iCaretPosition++, 1, 'm');
			if(GetKey(olc::Key::N).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'N') : sLevelFileName.insert(iCaretPosition++, 1, 'n');
			if(GetKey(olc::Key::O).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'O') : sLevelFileName.insert(iCaretPosition++, 1, 'o');
			if(GetKey(olc::Key::P).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'P') : sLevelFileName.insert(iCaretPosition++, 1, 'p');
			if(GetKey(olc::Key::Q).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'Q') : sLevelFileName.insert(iCaretPosition++, 1, 'q');
			if(GetKey(olc::Key::R).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'R') : sLevelFileName.insert(iCaretPosition++, 1, 'r');
			if(GetKey(olc::Key::S).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'S') : sLevelFileName.insert(iCaretPosition++, 1, 's');
			if(GetKey(olc::Key::T).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'T') : sLevelFileName.insert(iCaretPosition++, 1, 't');
			if(GetKey(olc::Key::U).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'U') : sLevelFileName.insert(iCaretPosition++, 1, 'u');
			if(GetKey(olc::Key::V).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'V') : sLevelFileName.insert(iCaretPosition++, 1, 'v');
			if(GetKey(olc::Key::W).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'W') : sLevelFileName.insert(iCaretPosition++, 1, 'w');
			if(GetKey(olc::Key::X).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'X') : sLevelFileName.insert(iCaretPosition++, 1, 'x');
			if(GetKey(olc::Key::Y).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'Y') : sLevelFileName.insert(iCaretPosition++, 1, 'y');
			if(GetKey(olc::Key::Z).bPressed) GetKey(olc::Key::SHIFT).bHeld ? sLevelFileName.insert(iCaretPosition++, 1, 'Z') : sLevelFileName.insert(iCaretPosition++, 1, 'z');
			// Input with standart numeric keys:
			if(GetKey(olc::Key::K0).bPressed) sLevelFileName.insert(iCaretPosition++, 1, '0');
			if(GetKey(olc::Key::K1).bPressed) sLevelFileName.insert(iCaretPosition++, 1, '1');
			if(GetKey(olc::Key::K2).bPressed) sLevelFileName.insert(iCaretPosition++, 1, '2');
			if(GetKey(olc::Key::K3).bPressed) sLevelFileName.insert(iCaretPosition++, 1, '3');
			if(GetKey(olc::Key::K4).bPressed) sLevelFileName.insert(iCaretPosition++, 1, '4');
			if(GetKey(olc::Key::K5).bPressed) sLevelFileName.insert(iCaretPosition++, 1, '5');
			if(GetKey(olc::Key::K6).bPressed) sLevelFileName.insert(iCaretPosition++, 1, '6');
			if(GetKey(olc::Key::K7).bPressed) sLevelFileName.insert(iCaretPosition++, 1, '7');
			if(GetKey(olc::Key::K8).bPressed) sLevelFileName.insert(iCaretPosition++, 1, '8');
			if(GetKey(olc::Key::K9).bPressed) sLevelFileName.insert(iCaretPosition++, 1, '9');
			// Input with miscellaneous keys:
			if(GetKey(olc::Key::SPACE).bPressed) sLevelFileName.insert(iCaretPosition++, 1, ' ');
			if(GetKey(olc::Key::BACK).bPressed) if(sLevelFileName.size()) sLevelFileName.erase(--iCaretPosition, 1);
			if(GetKey(olc::Key::DEL).bPressed)  if(sLevelFileName.size()) sLevelFileName.erase(iCaretPosition, 1);	
			if(GetKey(olc::Key::ESCAPE).bPressed) 
			{
				iLevelEditorStatus = 0;
				sLevelFileName = "";
				iCaretPosition = 0;
				show_level_editor();
			}
			if(GetKey(olc::Key::ENTER).bPressed)
			{
				sLevelFileName += ".lvl";
				return true;	
			}
			return false;
		}
		if(iLevelEditorStatus == 3) // Level creation exception window
		{
			// Input with arrow keys:
			if(GetKey(olc::Key::DOWN).bPressed)	iCursorLevelCreationExceptionWindowStatus == 2 ? iCursorLevelCreationExceptionWindowStatus = 0 : iCursorLevelCreationExceptionWindowStatus++;
			if(GetKey(olc::Key::UP).bPressed)   iCursorLevelCreationExceptionWindowStatus == 0 ? iCursorLevelCreationExceptionWindowStatus = 2 : iCursorLevelCreationExceptionWindowStatus--;
			// Input with miscellaneous keys:
			if(GetKey(olc::Key::ENTER).bPressed)
			{
				switch(iCursorLevelCreationExceptionWindowStatus)
				{
					case 0: rewrite_level_file(sLevelFileName); open_level_file(sLevelFileName); iLevelEditorStatus = 5; break; // Rewrite and open level
					case 1: open_level_file(sLevelFileName); iLevelEditorStatus = 5; break; // Open level
					case 2: sLevelFileName.erase(sLevelFileName.size()-4, 4); iLevelEditorStatus = 1; break; // Back to level creation menu
				}
			}
			return false;
		}
		if(iLevelEditorStatus == 4) // Level opening exception window
		{
			// Input with arrow keys:
			if(GetKey(olc::Key::DOWN).bPressed)	iCursorLevelOpeningExceptionWindowStatus == 1 ? iCursorLevelOpeningExceptionWindowStatus = 0 : iCursorLevelOpeningExceptionWindowStatus++;
			if(GetKey(olc::Key::UP).bPressed)   iCursorLevelOpeningExceptionWindowStatus == 0 ? iCursorLevelOpeningExceptionWindowStatus = 1 : iCursorLevelOpeningExceptionWindowStatus--;
			// Input with miscellaneous keys:
			if(GetKey(olc::Key::ENTER).bPressed)
			{
				switch(iCursorLevelOpeningExceptionWindowStatus)
				{
					case 0: rewrite_level_file(sLevelFileName); open_level_file(sLevelFileName); iLevelEditorStatus = 5; break; // Create and open level 
					case 1: sLevelFileName.erase(sLevelFileName.size()-4, 4); iLevelEditorStatus = 2; break; // Back to level opening menu
				}
			}	
			return false;
		}
		if(iLevelEditorStatus == 5) // Level editor window
		{ 
			// Getting mouse position:
			viMouseWindowPosition = olc::vi2d(GetMouseX(), GetMouseY())/viWindowCellSize;
			// Input with mouse keys:
				// Editor map:
			if(viMouseWindowPosition.x >= viEditorMapWindowPosition.x && viMouseWindowPosition.x < viEditorMapWindowPosition.x+viEditorMapSize.x
		    && viMouseWindowPosition.y >= viEditorMapWindowPosition.y && viMouseWindowPosition.y < viEditorMapWindowPosition.y+viEditorMapSize.y)
			{
				if(GetMouse(0).bHeld) 
				{
					iEditorMap[viEditorMapSize.x*(viMouseWindowPosition.y-viEditorMapWindowPosition.y)+(viMouseWindowPosition.x-viEditorMapWindowPosition.x)] = iSelectedBlockCode;
					bIsLevelFileSaved = false;
				}
				if(GetMouse(1).bHeld) 
				{
					iEditorMap[viEditorMapSize.x*(viMouseWindowPosition.y-viEditorMapWindowPosition.y)+(viMouseWindowPosition.x-viEditorMapWindowPosition.x)] = 0;
					bIsLevelFileSaved = false;
				}
				if(GetMouse(2).bPressed) 
				{
					viEditorStartMapPlayerPosition = viMouseWindowPosition - viEditorMapWindowPosition;
					bIsLevelFileSaved = false;
				}
			}
				// Editor block map:
			if(viMouseWindowPosition.x >= viEditorBlockMapWindowPosition.x && viMouseWindowPosition.x < viEditorBlockMapWindowPosition.x+viEditorBlockMapSize.x
		    && viMouseWindowPosition.y >= viEditorBlockMapWindowPosition.y && viMouseWindowPosition.y < viEditorBlockMapWindowPosition.y+viEditorBlockMapSize.y)
			{
				if(GetMouse(0).bPressed) iSelectedBlockCode = iEditorBlockMapPageStatus*80+viEditorBlockMapSize.x*(viMouseWindowPosition.y-viEditorBlockMapWindowPosition.y)+(viMouseWindowPosition.x-viEditorBlockMapWindowPosition.x);
			}
				// Exit button:
			if(viMouseWindowPosition.x >= viExitButtonWindowPosition.x && viMouseWindowPosition.x < viExitButtonWindowPosition.x+viExitButtonSize.x
		    && viMouseWindowPosition.y >= viExitButtonWindowPosition.y && viMouseWindowPosition.y < viExitButtonWindowPosition.y+viExitButtonSize.y)
			{
				if(GetMouse(0).bPressed)
				{
					memset(iEditorMap, 0, sizeof iEditorMap);
					viEditorStartMapPlayerPosition = olc::vi2d(0, 0);
					iEditorBlockMapPageStatus = 0;
					iSelectedBlockCode = 0;
					sLevelFileName = "";
					iCaretPosition = 0;
					iLevelEditorStatus = 0;
					bIsLevelFileSaved = true;
				}
			}
				// Save button:
			if(viMouseWindowPosition.x >= viSaveButtonWindowPosition.x && viMouseWindowPosition.x < viSaveButtonWindowPosition.x+viSaveButtonSize.x
		    && viMouseWindowPosition.y >= viSaveButtonWindowPosition.y && viMouseWindowPosition.y < viSaveButtonWindowPosition.y+viSaveButtonSize.y)
			{
				if(GetMouse(0).bPressed)
				{
					rewrite_level_file(sLevelFileName);
					bIsLevelFileSaved = true;
				}
			}
				// Clear button:
			if(viMouseWindowPosition.x >= viClearButtonWindowPosition.x && viMouseWindowPosition.x < viClearButtonWindowPosition.x+viClearButtonSize.x
		    && viMouseWindowPosition.y >= viClearButtonWindowPosition.y && viMouseWindowPosition.y < viClearButtonWindowPosition.y+viClearButtonSize.y)
			{
				if(GetMouse(0).bPressed) 
				{
					memset(iEditorMap, 0, sizeof iEditorMap);
					bIsLevelFileSaved = false;
				}
			}
				// Back button:
			if(viMouseWindowPosition.x >= viBackButtonWindowPosition.x && viMouseWindowPosition.x < viBackButtonWindowPosition.x+viBackButtonSize.x
		    && viMouseWindowPosition.y >= viBackButtonWindowPosition.y && viMouseWindowPosition.y < viBackButtonWindowPosition.y+viBackButtonSize.y)
			{
				if(GetMouse(0).bPressed) iEditorBlockMapPageStatus == 0 ? iEditorBlockMapPageStatus = 4 : iEditorBlockMapPageStatus--;
			}
				// Next button:
			if(viMouseWindowPosition.x >= viNextButtonWindowPosition.x && viMouseWindowPosition.x < viNextButtonWindowPosition.x+viNextButtonSize.x
		    && viMouseWindowPosition.y >= viNextButtonWindowPosition.y && viMouseWindowPosition.y < viNextButtonWindowPosition.y+viNextButtonSize.y)
			{
				if(GetMouse(0).bPressed) iEditorBlockMapPageStatus == 4 ? iEditorBlockMapPageStatus = 0 : iEditorBlockMapPageStatus++;
			}
			// Input with arrow keys:
			if(GetKey(olc::Key::DOWN).bPressed) iSelectedBlockCode == 399 ? iSelectedBlockCode = 0 : iSelectedBlockCode++;
			if(GetKey(olc::Key::UP).bPressed) 	iSelectedBlockCode == 0 ? iSelectedBlockCode = 399 : iSelectedBlockCode--;
			if(GetKey(olc::Key::RIGHT).bPressed) iEditorBlockMapPageStatus == 4 ? iEditorBlockMapPageStatus = 0 : iEditorBlockMapPageStatus++;
			if(GetKey(olc::Key::LEFT).bPressed) iEditorBlockMapPageStatus == 0 ? iEditorBlockMapPageStatus = 4 : iEditorBlockMapPageStatus--;
			// Input with standart alphanumeric keys: 
			if(GetKey(olc::Key::C).bPressed) 
			{
				memset(iEditorMap, 0, sizeof iEditorMap);
				bIsLevelFileSaved = false;
			}
			if(GetKey(olc::Key::S).bPressed) 
			{
				rewrite_level_file(sLevelFileName);
				bIsLevelFileSaved = true;
			}
			// Input with miscellaneous keys:
			if(GetKey(olc::Key::ESCAPE).bPressed)
			{
				memset(iEditorMap, 0, sizeof iEditorMap);
				viEditorStartMapPlayerPosition = olc::vi2d(0, 0);
				iEditorBlockMapPageStatus = 0;
				iSelectedBlockCode = 0;
				sLevelFileName = "";
				iCaretPosition = 0;
				iLevelEditorStatus = 0;
				bIsLevelFileSaved = false;
			}  
			return false;
		}
		return false;
	}

	bool is_exist_level_file_name(std::string sLevelFileName)
	{
		char cLevelFileName[sLevelFileName.size()+1];
		strcpy(cLevelFileName, sLevelFileName.c_str());
		FILE *fpLevelFile = fopen(cLevelFileName, "rb");
		if(!fpLevelFile) return false;
		else fclose(fpLevelFile); 
		return true;	
	}

	void show_level_creation_menu()
	{
		if(sLevelFileName[iCaretPosition] == '_') sLevelFileName.erase(iCaretPosition, 1);
		if(check_level_editor_buttons())
		{
			if(is_exist_level_file_name(sLevelFileName)) iLevelEditorStatus = 3;
			else
			{
				rewrite_level_file(sLevelFileName);
				open_level_file(sLevelFileName);
				iLevelEditorStatus = 5;
			}
		}
		else
		{
			sLevelFileName.insert(iCaretPosition, 1, '_');
			Clear(olc::BLACK);
			DrawString(1, 1, "Level creation");
			DrawString(1, 9, "Enter new level name: "+sLevelFileName);
		}
	}

	void show_level_creation_exception_window()
	{
		check_level_editor_buttons();
		Clear(olc::BLACK);
		DrawString(1, 1, "A level with this name already exists. What do you want to do?");
		for (int i = 0; i < 3; i++) DrawString(1, 9+8*i, (i==iCursorLevelCreationExceptionWindowStatus?">>":"  ")+sLevelCreationExceptionWindowOptions[i]);
	}

	void show_level_opening_menu()
	{
		if(sLevelFileName[iCaretPosition] == '_') sLevelFileName.erase(iCaretPosition, 1);
		if(check_level_editor_buttons())
		{
			if(!is_exist_level_file_name(sLevelFileName)) iLevelEditorStatus = 4;
			else
			{
				open_level_file(sLevelFileName);
				iLevelEditorStatus = 5;
			}
		}
		else
		{
			sLevelFileName.insert(iCaretPosition, 1, '_');
			Clear(olc::BLACK);
			DrawString(1, 1, "Level opening");
			DrawString(1, 9, "Enter existing level name: "+sLevelFileName);
		}
	}

	void show_level_opening_exception_window()
	{
		check_level_editor_buttons();
		Clear(olc::BLACK);
		DrawString(1, 1, "There is no level with this name. What do you want to do?");
		for (int i = 0; i < 2; i++) DrawString(1, 9+8*i, (i==iCursorLevelOpeningExceptionWindowStatus?">>":"  ")+sLevelOpeningExceptionWindowOptions[i]);
	}

	void rewrite_level_file(std::string sLevelFileName)
	{
		char cLevelFileName[sLevelFileName.size()+1];
		strcpy(cLevelFileName, sLevelFileName.c_str());
		FILE *fpLevelFile = fopen(cLevelFileName, "wb");
		fwrite(&viEditorMapSize.x, sizeof(int), 1, fpLevelFile);
		fwrite(&viEditorMapSize.y, sizeof(int), 1, fpLevelFile);
		for (int i = 0; i < viEditorMapSize.y; i++) 
		{ 
			for (int j = 0; j < viEditorMapSize.x; j++) fwrite(&iEditorMap[viEditorMapSize.x*i+j], sizeof(int), 1, fpLevelFile);
		}
		fwrite(&viEditorStartMapPlayerPosition.x, sizeof(int), 1, fpLevelFile);
		fwrite(&viEditorStartMapPlayerPosition.y, sizeof(int), 1, fpLevelFile);
		fclose(fpLevelFile);
	}

	void open_level_file(std::string sLevelFileName)
	{
		char cLevelFileName[sLevelFileName.size()+1];
		strcpy(cLevelFileName, sLevelFileName.c_str());
		FILE *fpLevelFile = fopen(cLevelFileName, "rb");
		fread(&viEditorMapSize.x, sizeof(int), 1, fpLevelFile);
		fread(&viEditorMapSize.y, sizeof(int), 1, fpLevelFile);
		fread(iEditorMap, sizeof(int), viEditorMapSize.x*viEditorMapSize.y, fpLevelFile);
		fread(&viEditorStartMapPlayerPosition.x, sizeof(int), 1, fpLevelFile);
		fread(&viEditorStartMapPlayerPosition.y, sizeof(int), 1, fpLevelFile);
		fclose(fpLevelFile);
	}

	void show_level_editor_window()
	{
		check_level_editor_buttons();
		Clear(olc::BLACK);
		
		// Drawing editor map
		DrawRect((viEditorMapWindowPosition)*viWindowCellSize-olc::vi2d(2, 2), viEditorMapSize*viWindowCellSize+olc::vi2d(3, 3));
		for (int i = 0; i < viEditorMapSize.y; i++)
		{
			for (int j = 0; j < viEditorMapSize.x; j++)
			{
				DrawPartialSprite((viEditorMapWindowPosition+olc::vi2d(j, i))*viWindowCellSize,
								  sprTileMap.get(),
								  olc::vi2d(iEditorMap[viEditorMapSize.x*i+j]%20, iEditorMap[viEditorMapSize.x*i+j]/20)*viWindowCellSize,
								  viWindowCellSize); 
			}
		}

		// Drawing editor start map player position
		DrawRect((viEditorMapWindowPosition+viEditorStartMapPlayerPosition)*viWindowCellSize-olc::vi2d(1, 1), viWindowCellSize+olc::vi2d(1, 1), olc::BLUE);

		// Drawing editor map mouse cursor
		if(viMouseWindowPosition.x >= viEditorMapWindowPosition.x && viMouseWindowPosition.x < viEditorMapWindowPosition.x+viEditorMapSize.x
		&& viMouseWindowPosition.y >= viEditorMapWindowPosition.y && viMouseWindowPosition.y < viEditorMapWindowPosition.y+viEditorMapSize.y)
		{
			DrawRect(viMouseWindowPosition*viWindowCellSize-olc::vi2d(1, 1), viWindowCellSize+olc::vi2d(1, 1), olc::RED);
		}

		// Drawing selected block
		DrawPartialSprite(viSelectedBlockWindowPosition*viWindowCellSize,
						  sprTileMap.get(),
						  olc::vi2d(iSelectedBlockCode%20, iSelectedBlockCode/20)*viWindowCellSize,
						  viWindowCellSize, 4);
		DrawRect((viSelectedBlockWindowPosition)*viWindowCellSize-olc::vi2d(2, 2), olc::vi2d(4, 4)*viWindowCellSize+olc::vi2d(3, 3));
		
		// Drawing editor block map
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				DrawPartialSprite((viEditorBlockMapWindowPosition+olc::vi2d(j, i))*viWindowCellSize,
								  sprTileMap.get(),
								  olc::vi2d((i*8+j+iEditorBlockMapPageStatus*80)%20, (i*8+j+iEditorBlockMapPageStatus*80)/20)*viWindowCellSize,
								  viWindowCellSize);
			}
		}
		DrawRect((viEditorBlockMapWindowPosition)*viWindowCellSize-olc::vi2d(2, 2),
				 viEditorBlockMapSize*viWindowCellSize+olc::vi2d(3, 3));
		
		// Drawing editor block map selected block cursor
		if(iEditorBlockMapPageStatus*80 <= iSelectedBlockCode && (iEditorBlockMapPageStatus+1)*80 > iSelectedBlockCode)
		{
			DrawRect((viEditorBlockMapWindowPosition+olc::vi2d(iSelectedBlockCode%80%8, iSelectedBlockCode%80/8))*viWindowCellSize-olc::vi2d(1, 1),
					 viWindowCellSize+olc::vi2d(1, 1), olc::GREEN);
		}

		// Drawing back button
		DrawString(viBackButtonWindowPosition*viWindowCellSize, "<<", olc::WHITE, 2);
		DrawRect(viBackButtonWindowPosition*viWindowCellSize-olc::vi2d(2, 2),
				 viBackButtonSize*viWindowCellSize+olc::vi2d(3, 3));
		if(viMouseWindowPosition.x >= viBackButtonWindowPosition.x && viMouseWindowPosition.x < viBackButtonWindowPosition.x+viBackButtonSize.x
		&& viMouseWindowPosition.y >= viBackButtonWindowPosition.y && viMouseWindowPosition.y < viBackButtonWindowPosition.y+viBackButtonSize.y)
		{
			DrawRect(viBackButtonWindowPosition*viWindowCellSize-olc::vi2d(4, 4), viBackButtonSize*viWindowCellSize+olc::vi2d(7, 7));
		}
		
		// Drawing next button
		DrawString(viNextButtonWindowPosition*viWindowCellSize, ">>", olc::WHITE, 2);
		DrawRect(viNextButtonWindowPosition*viWindowCellSize-olc::vi2d(2, 2),
				 viNextButtonSize*viWindowCellSize+olc::vi2d(3, 3));
		if(viMouseWindowPosition.x >= viNextButtonWindowPosition.x && viMouseWindowPosition.x < viNextButtonWindowPosition.x+viNextButtonSize.x
		&& viMouseWindowPosition.y >= viNextButtonWindowPosition.y && viMouseWindowPosition.y < viNextButtonWindowPosition.y+viNextButtonSize.y)
		{
			DrawRect(viNextButtonWindowPosition*viWindowCellSize-olc::vi2d(4, 4), viNextButtonSize*viWindowCellSize+olc::vi2d(7, 7));
		}
		
		// Drawing exit button
		DrawString(viExitButtonWindowPosition*viWindowCellSize, "EXIT", olc::WHITE, 4);
		DrawRect(viExitButtonWindowPosition*viWindowCellSize-olc::vi2d(2, 2),
				 viExitButtonSize*viWindowCellSize+olc::vi2d(3, 3));
		if(viMouseWindowPosition.x >= viExitButtonWindowPosition.x && viMouseWindowPosition.x < viExitButtonWindowPosition.x+viExitButtonSize.x
		&& viMouseWindowPosition.y >= viExitButtonWindowPosition.y && viMouseWindowPosition.y < viExitButtonWindowPosition.y+viExitButtonSize.y)
		{
			DrawRect(viExitButtonWindowPosition*viWindowCellSize-olc::vi2d(4, 4), viExitButtonSize*viWindowCellSize+olc::vi2d(7, 7));
		}
		
		// Drawing save button
		DrawString(viSaveButtonWindowPosition*viWindowCellSize, "SAVE", olc::WHITE, 4);
		DrawRect(viSaveButtonWindowPosition*viWindowCellSize-olc::vi2d(2, 2),
				 viSaveButtonSize*viWindowCellSize+olc::vi2d(3, 3));
		if(viMouseWindowPosition.x >= viSaveButtonWindowPosition.x && viMouseWindowPosition.x < viSaveButtonWindowPosition.x+viSaveButtonSize.x
		&& viMouseWindowPosition.y >= viSaveButtonWindowPosition.y && viMouseWindowPosition.y < viSaveButtonWindowPosition.y+viSaveButtonSize.y)
		{
			DrawRect(viSaveButtonWindowPosition*viWindowCellSize-olc::vi2d(4, 4), viSaveButtonSize*viWindowCellSize+olc::vi2d(7, 7));
		}
		
		// Drawing level file save status
		if(bIsLevelFileSaved) DrawString(viLevelFileSaveStatusWindowPosition*viWindowCellSize, "Level saved", olc::GREEN);
		else DrawString(viLevelFileSaveStatusWindowPosition*viWindowCellSize, "Level not saved yet", olc::RED);

		// Drawing clear button
		DrawString(viClearButtonWindowPosition*viWindowCellSize, "CLEAR", olc::WHITE, 4);
		DrawRect(viClearButtonWindowPosition*viWindowCellSize-olc::vi2d(2, 2),
				 viClearButtonSize*viWindowCellSize+olc::vi2d(3, 3));
		if(viMouseWindowPosition.x >= viClearButtonWindowPosition.x && viMouseWindowPosition.x < viClearButtonWindowPosition.x+viClearButtonSize.x
		&& viMouseWindowPosition.y >= viClearButtonWindowPosition.y && viMouseWindowPosition.y < viClearButtonWindowPosition.y+viClearButtonSize.y)
		{
			DrawRect(viClearButtonWindowPosition*viWindowCellSize-olc::vi2d(4, 4), viClearButtonSize*viWindowCellSize+olc::vi2d(7, 7));
		}
	}

	void show_level_editor_menu()
	{
		check_level_editor_buttons();
		Clear(olc::BLACK);
		for (int i = 0; i < 3; i++) DrawString(1, 1+8*i, (i==iCursorLevelEditorMenuStatus?">>":"  ")+sLevelEditorMenuOptions[i]);
	}

	void show_level_editor()
	{
		switch(iLevelEditorStatus)
		{
			case 0:	show_level_editor_menu(); break; // Level editor menu
			case 1: show_level_creation_menu();	break; // Level creation menu
			case 2: show_level_opening_menu(); break; // Level opening menu
			case 3: show_level_creation_exception_window(); break; // Level creation exception window
			case 4: show_level_opening_exception_window(); break; // Level opening excepiton window
			case 5: show_level_editor_window(); break; // Level editor window
		}
	}




// Constructor of class NewPlatformer
public:
	NewPlatformer()
	{
		sAppName = "Test";
	}

// Methods of class NewPlatformer
public:

	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		switch(iNewPlatformerStatus)
		{
			case 0: show_main_menu(); break;
			case 1: show_game(fElapsedTime); break; 
			case 2: show_level_editor(); break;
		}
		return true;
	}
};

int main()
{
	NewPlatformer demo;
	if (demo.Construct(736, 416, 2, 2, true))
		demo.Start();

	return 0;
}