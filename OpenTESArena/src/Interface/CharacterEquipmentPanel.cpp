#include <cassert>

#include "SDL.h"

#include "CharacterEquipmentPanel.h"

#include "Button.h"
#include "CharacterPanel.h"
#include "TextBox.h"
#include "../Entities/CharacterClass.h"
#include "../Entities/CharacterRace.h"
#include "../Entities/Player.h"
#include "../Game/GameData.h"
#include "../Game/GameState.h"
#include "../Math/Constants.h"
#include "../Math/Int2.h"
#include "../Media/FontName.h"
#include "../Media/PaletteName.h"
#include "../Media/PortraitFile.h"
#include "../Media/TextureFile.h"
#include "../Media/TextureManager.h"
#include "../Media/TextureName.h"
#include "../Rendering/Renderer.h"

CharacterEquipmentPanel::CharacterEquipmentPanel(GameState *gameState)
	: Panel(gameState)
{
	this->playerNameTextBox = [gameState]()
	{
		int x = 10;
		int y = 8;
		Color color(199, 199, 199);
		std::string text = gameState->getGameData()->getPlayer().getDisplayName();
		auto fontName = FontName::Arena;
		return std::unique_ptr<TextBox>(new TextBox(
			x,
			y,
			color,
			text,
			fontName,
			gameState->getTextureManager(),
			gameState->getRenderer()));
	}();

	this->playerRaceTextBox = [gameState]()
	{
		int x = 10;
		int y = 17;
		Color color(199, 199, 199);
		std::string text = CharacterRace(gameState->getGameData()->getPlayer()
			.getRaceName()).toString();
		auto fontName = FontName::Arena;
		return std::unique_ptr<TextBox>(new TextBox(
			x,
			y,
			color,
			text,
			fontName,
			gameState->getTextureManager(),
			gameState->getRenderer()));
	}();

	this->playerClassTextBox = [gameState]()
	{
		int x = 10;
		int y = 26;
		Color color(199, 199, 199);
		std::string text = gameState->getGameData()->getPlayer().getCharacterClass()
			.getDisplayName();
		auto fontName = FontName::Arena;
		return std::unique_ptr<TextBox>(new TextBox(
			x,
			y,
			color,
			text,
			fontName,
			gameState->getTextureManager(),
			gameState->getRenderer()));
	}();

	this->backToStatsButton = []()
	{
		int x = 0;
		int y = 188;
		int width = 47;
		int height = 12;
		auto function = [](GameState *gameState)
		{
			std::unique_ptr<Panel> characterPanel(new CharacterPanel(gameState));
			gameState->setPanel(std::move(characterPanel));
		};
		return std::unique_ptr<Button>(new Button(x, y, width, height, function));
	}();

	this->spellbookButton = []()
	{
		int x = 47;
		int y = 188;
		int width = 76;
		int height = 12;
		auto function = [](GameState *gameState)
		{
			// Nothing yet.
		};
		return std::unique_ptr<Button>(new Button(x, y, width, height, function));
	}();

	this->dropButton = []()
	{
		int x = 123;
		int y = 188;
		int width = 48;
		int height = 12;
		auto function = [](GameState *gameState)
		{
			// Nothing yet.
		};
		return std::unique_ptr<Button>(new Button(x, y, width, height, function));
	}();

	this->scrollDownButton = []()
	{
		Int2 center(16, 131);
		int width = 9;
		int height = 9;
		auto function = [](GameState *gameState)
		{
			// Nothing yet.
		};
		return std::unique_ptr<Button>(new Button(center, width, height, function));
	}();

	this->scrollUpButton = []()
	{
		Int2 center(152, 131);
		int width = 9;
		int height = 9;
		auto function = [](GameState *gameState)
		{
			// Nothing yet.
		};
		return std::unique_ptr<Button>(new Button(center, width, height, function));
	}();
}

CharacterEquipmentPanel::~CharacterEquipmentPanel()
{

}

void CharacterEquipmentPanel::handleEvents(bool &running)
{
	auto mousePosition = this->getMousePosition();
	auto mouseOriginalPoint = this->getGameState()->getRenderer()
		.nativePointToOriginal(mousePosition);

	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		bool applicationExit = (e.type == SDL_QUIT);
		bool resized = (e.type == SDL_WINDOWEVENT) &&
			(e.window.event == SDL_WINDOWEVENT_RESIZED);
		bool escapePressed = (e.type == SDL_KEYDOWN) &&
			(e.key.keysym.sym == SDLK_ESCAPE);
		bool tabPressed = (e.type == SDL_KEYDOWN) &&
			(e.key.keysym.sym == SDLK_TAB);

		if (applicationExit)
		{
			running = false;
		}
		if (resized)
		{
			int width = e.window.data1;
			int height = e.window.data2;
			this->getGameState()->resizeWindow(width, height);
		}
		if (escapePressed || tabPressed)
		{
			this->backToStatsButton->click(this->getGameState());
		}

		bool leftClick = (e.type == SDL_MOUSEBUTTONDOWN) &&
			(e.button.button == SDL_BUTTON_LEFT);

		if (leftClick)
		{
			if (this->backToStatsButton->containsPoint(mouseOriginalPoint))
			{
				this->backToStatsButton->click(this->getGameState());
			}
			else if (this->spellbookButton->containsPoint(mouseOriginalPoint))
			{
				this->spellbookButton->click(this->getGameState());
			}
			else if (this->dropButton->containsPoint(mouseOriginalPoint))
			{
				this->dropButton->click(this->getGameState());
			}
			else if (this->scrollUpButton->containsPoint(mouseOriginalPoint))
			{
				this->scrollUpButton->click(this->getGameState());
			}
			else if (this->scrollDownButton->containsPoint(mouseOriginalPoint))
			{
				this->scrollDownButton->click(this->getGameState());
			}
		}
	}
}

void CharacterEquipmentPanel::handleMouse(double dt)
{
	static_cast<void>(dt);
}

void CharacterEquipmentPanel::handleKeyboard(double dt)
{
	static_cast<void>(dt);
}

void CharacterEquipmentPanel::tick(double dt, bool &running)
{
	static_cast<void>(dt);

	this->handleEvents(running);
}

void CharacterEquipmentPanel::render(Renderer &renderer)
{
	assert(this->getGameState()->gameDataIsActive());

	// Clear full screen.
	renderer.clearNative();

	// Set palette.
	auto &textureManager = this->getGameState()->getTextureManager();
	textureManager.setPalette(PaletteName::CharSheet);

	// Draw character equipment background.
	auto *equipmentBackground = textureManager.getTexture(
		TextureFile::fromName(TextureName::CharacterEquipment));
	renderer.drawToOriginal(equipmentBackground);

	// Get a reference to the active player data.
	const auto &player = this->getGameState()->getGameData()->getPlayer();

	// Get the filenames for the portraits.
	auto portraitStrings = PortraitFile::getGroup(player.getGenderName(),
		player.getRaceName(), player.getCharacterClass().canCastMagic());

	// Draw the player's portrait.
	auto *portrait = textureManager.getTexture(
		portraitStrings.at(player.getPortraitID()));
	int portraitWidth, portraitHeight;
	SDL_QueryTexture(portrait, nullptr, nullptr, &portraitWidth, &portraitHeight);
	renderer.drawToOriginal(portrait, ORIGINAL_WIDTH - portraitWidth, 0);

	// Draw text boxes: player name, race, class.
	renderer.drawToOriginal(this->playerNameTextBox->getSurface(),
		this->playerNameTextBox->getX(), this->playerNameTextBox->getY());
	renderer.drawToOriginal(this->playerRaceTextBox->getSurface(),
		this->playerRaceTextBox->getX(), this->playerRaceTextBox->getY());
	renderer.drawToOriginal(this->playerClassTextBox->getSurface(),
		this->playerClassTextBox->getX(), this->playerClassTextBox->getY());

	// Scale the original frame buffer onto the native one.
	renderer.drawOriginalToNative();

	// Draw cursor.
	const auto &cursor = textureManager.getSurface(
		TextureFile::fromName(TextureName::SwordCursor));
	SDL_SetColorKey(cursor.getSurface(), SDL_TRUE,
		renderer.getFormattedARGB(Color::Black));
	const auto mousePosition = this->getMousePosition();
	renderer.drawToNative(cursor.getSurface(),
		mousePosition.getX(), mousePosition.getY(),
		static_cast<int>(cursor.getWidth() * this->getCursorScale()),
		static_cast<int>(cursor.getHeight() * this->getCursorScale()));
}
