#include <cassert>
#include <map>

#include "SDL.h"

#include "ChooseNamePanel.h"

#include "Button.h"
#include "ChooseClassPanel.h"
#include "ChooseGenderPanel.h"
#include "TextBox.h"
#include "../Entities/CharacterClass.h"
#include "../Game/GameState.h"
#include "../Math/Constants.h"
#include "../Math/Int2.h"
#include "../Media/Color.h"
#include "../Media/FontName.h"
#include "../Media/PaletteName.h"
#include "../Media/TextureFile.h"
#include "../Media/TextureManager.h"
#include "../Media/TextureName.h"
#include "../Rendering/Renderer.h"

const int ChooseNamePanel::MAX_NAME_LENGTH = 25;

ChooseNamePanel::ChooseNamePanel(GameState *gameState, const CharacterClass &charClass)
	: Panel(gameState)
{
	this->parchment = [gameState]()
	{
		auto *surface = gameState->getTextureManager().getSurface(
			TextureFile::fromName(TextureName::ParchmentPopup)).getSurface();
		return std::unique_ptr<Surface>(new Surface(surface));
	}();

	this->titleTextBox = [gameState, charClass]()
	{
		Int2 center(ORIGINAL_WIDTH / 2, 90);
		Color color(48, 12, 12);
		std::string text = "What will be thy name,\n" + charClass.getDisplayName() + "?";
		auto fontName = FontName::A;
		return std::unique_ptr<TextBox>(new TextBox(
			center,
			color,
			text,
			fontName,
			gameState->getTextureManager(),
			gameState->getRenderer()));
	}();

	this->nameTextBox = [gameState]()
	{
		Int2 center(ORIGINAL_WIDTH / 2, 110);
		auto color = Color::White;
		std::string text = "";
		auto fontName = FontName::A;
		return std::unique_ptr<TextBox>(new TextBox(
			center,
			color,
			text,
			fontName,
			gameState->getTextureManager(),
			gameState->getRenderer()));
	}();

	this->backToClassButton = []()
	{
		auto function = [](GameState *gameState)
		{
			std::unique_ptr<Panel> classPanel(new ChooseClassPanel(gameState));
			gameState->setPanel(std::move(classPanel));
		};
		return std::unique_ptr<Button>(new Button(function));
	}();

	this->acceptButton = [this, charClass]()
	{
		auto function = [this, charClass](GameState *gameState)
		{
			std::unique_ptr<Panel> racePanel(new ChooseGenderPanel(
				gameState, charClass, this->name));
			gameState->setPanel(std::move(racePanel));
		};
		return std::unique_ptr<Button>(new Button(function));
	}();

	this->charClass = std::unique_ptr<CharacterClass>(new CharacterClass(charClass));
	this->name = std::string();
}

ChooseNamePanel::~ChooseNamePanel()
{

}

void ChooseNamePanel::handleEvents(bool &running)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		bool applicationExit = (e.type == SDL_QUIT);
		bool resized = (e.type == SDL_WINDOWEVENT) &&
			(e.window.event == SDL_WINDOWEVENT_RESIZED);
		bool escapePressed = (e.type == SDL_KEYDOWN) &&
			(e.key.keysym.sym == SDLK_ESCAPE);
		bool enterPressed = (e.type == SDL_KEYDOWN) &&
			((e.key.keysym.sym == SDLK_RETURN) || (e.key.keysym.sym == SDLK_KP_ENTER));

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
		if (escapePressed)
		{
			this->backToClassButton->click(this->getGameState());
		}

		// Only accept the name if it has a positive size.
		if (enterPressed && (this->name.size() > 0))
		{
			this->acceptButton->click(this->getGameState());
		}

		// Upper and lower case English characters.
		const std::map<SDL_Keycode, std::pair<char, char>> letters =
		{
			{ SDLK_a, { 'A', 'a' } },
			{ SDLK_b, { 'B', 'b' } },
			{ SDLK_c, { 'C', 'c' } },
			{ SDLK_d, { 'D', 'd' } },
			{ SDLK_e, { 'E', 'e' } },
			{ SDLK_f, { 'F', 'f' } },
			{ SDLK_g, { 'G', 'g' } },
			{ SDLK_h, { 'H', 'h' } },
			{ SDLK_i, { 'I', 'i' } },
			{ SDLK_j, { 'J', 'j' } },
			{ SDLK_k, { 'K', 'k' } },
			{ SDLK_l, { 'L', 'l' } },
			{ SDLK_m, { 'M', 'm' } },
			{ SDLK_n, { 'N', 'n' } },
			{ SDLK_o, { 'O', 'o' } },
			{ SDLK_p, { 'P', 'p' } },
			{ SDLK_q, { 'Q', 'q' } },
			{ SDLK_r, { 'R', 'r' } },
			{ SDLK_s, { 'S', 's' } },
			{ SDLK_t, { 'T', 't' } },
			{ SDLK_u, { 'U', 'u' } },
			{ SDLK_v, { 'V', 'v' } },
			{ SDLK_w, { 'W', 'w' } },
			{ SDLK_x, { 'X', 'x' } },
			{ SDLK_y, { 'Y', 'y' } },
			{ SDLK_z, { 'Z', 'z' } }
		};

		// Punctuation (some duplicates exist to keep the shift behavior for quotes).
		const std::map<SDL_Keycode, std::pair<char, char>> punctuation =
		{
			{ SDLK_COMMA, { ',', ',' } },
			{ SDLK_MINUS, { '-', '-' } },
			{ SDLK_PERIOD, { '.', '.' } },
			{ SDLK_QUOTE, { '"', '\'' } }
		};

		if (e.type == SDL_KEYDOWN)
		{
			const auto *keys = SDL_GetKeyboardState(nullptr);
			const auto keyCode = e.key.keysym.sym;
			bool shiftPressed = keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];

			// See if the pressed key is a recognized letter.
			if (letters.find(keyCode) != letters.end())
			{
				// Add the letter to the name if there is room.
				if (this->name.size() < ChooseNamePanel::MAX_NAME_LENGTH)
				{
					const auto &pair = letters.at(keyCode);
					this->name.push_back(shiftPressed ? pair.first : pair.second);
				}
			}
			else if (punctuation.find(keyCode) != punctuation.end())
			{
				// The pressed key is recognized punctuation. Add it.
				if (this->name.size() < ChooseNamePanel::MAX_NAME_LENGTH)
				{
					const auto &pair = punctuation.at(keyCode);
					this->name.push_back(shiftPressed ? pair.first : pair.second);
				}
			}
			else if (keyCode == SDLK_SPACE)
			{
				// The pressed key is space. Add a space.
				if (this->name.size() < ChooseNamePanel::MAX_NAME_LENGTH)
				{
					this->name.push_back(' ');
				}
			}
			else if (keyCode == SDLK_BACKSPACE)
			{
				// The pressed key is backspace. Erase one letter if able.
				if (this->name.size() > 0)
				{
					this->name.pop_back();
				}
			}

			// Update the displayed name.
			this->nameTextBox = [this]
			{
				return std::unique_ptr<TextBox>(new TextBox(
					Int2(ORIGINAL_WIDTH / 2, 110),
					Color::White,
					this->name,
					FontName::A,
					this->getGameState()->getTextureManager(),
					this->getGameState()->getRenderer()));
			}();
		}

		// If (asciiCharacter or space is pressed) then push it onto the string.
		// If (Backspace is pressed) then delete one off if not at the start.

		// If (enter is pressed) then try and accept the string. Save it to a member.
	}
}

void ChooseNamePanel::handleMouse(double dt)
{
	static_cast<void>(dt);
}

void ChooseNamePanel::handleKeyboard(double dt)
{
	static_cast<void>(dt);
}

void ChooseNamePanel::tick(double dt, bool &running)
{
	static_cast<void>(dt);

	this->handleEvents(running);
}

void ChooseNamePanel::render(Renderer &renderer)
{
	// Clear full screen.
	renderer.clearNative();
	
	// Set palette.
	auto &textureManager = this->getGameState()->getTextureManager();
	textureManager.setPalette(PaletteName::Default);

	// Draw background.
	auto *background = textureManager.getTexture(
		TextureFile::fromName(TextureName::CharacterCreation), PaletteName::BuiltIn);
	renderer.drawToOriginal(background);

	// Draw parchment: title.
	this->parchment->setTransparentColor(Color::Magenta);

	const double parchmentXScale = 1.5;
	const double parchmentYScale = 1.65;
	const int parchmentWidth = static_cast<int>(this->parchment->getWidth() * parchmentXScale);
	const int parchmentHeight = static_cast<int>(this->parchment->getHeight() * parchmentYScale);

	renderer.drawToOriginal(this->parchment->getSurface(),
		(ORIGINAL_WIDTH / 2) - (parchmentWidth / 2),
		(ORIGINAL_HEIGHT / 2) - (parchmentHeight / 2),
		parchmentWidth,
		parchmentHeight);
	
	// Draw text: title, name.
	renderer.drawToOriginal(this->titleTextBox->getSurface(),
		this->titleTextBox->getX(), this->titleTextBox->getY());
	renderer.drawToOriginal(this->nameTextBox->getSurface(),
		this->nameTextBox->getX(), this->nameTextBox->getY());

	// Scale the original frame buffer onto the native one.
	renderer.drawOriginalToNative();

	// Draw cursor.
	const auto &cursor = textureManager.getSurface(
		TextureFile::fromName(TextureName::SwordCursor));
	SDL_SetColorKey(cursor.getSurface(), SDL_TRUE,
		renderer.getFormattedARGB(Color::Black));
	auto mousePosition = this->getMousePosition();
	renderer.drawToNative(cursor.getSurface(),
		mousePosition.getX(), mousePosition.getY(),
		static_cast<int>(cursor.getWidth() * this->getCursorScale()),
		static_cast<int>(cursor.getHeight() * this->getCursorScale()));
}
