#include "game.h"
#include "utils.h"

Game::Game()
{
	LoadTextures();
	// Initialis sprite for background.
	background = std::make_unique<sf::Sprite>(*textures["ground_sand"]);
	background->setTextureRect(sf::IntRect({ 0, 0 }, { 640, 480 }));

	// We're not loading the tank by default any more.
}

void Game::LoadTextures() {
	textures["ground_sand"] = std::make_shared<sf::Texture>("Assets/tileSand1.png");
	textures["ground_sand"]->setRepeated(true);

	textures["barrel_black"] = std::make_shared<sf::Texture>("Assets/blackBarrel.png");
	textures["body_black"] = std::make_shared<sf::Texture>("Assets/blackTank.png");
	textures["barrel_blue"] = std::make_shared<sf::Texture>("Assets/blueBarrel.png");
	textures["body_blue"] =	std::make_shared<sf::Texture>("Assets/blueTank.png");
	textures["barrel_green"] = std::make_shared<sf::Texture>("Assets/greenBarrel.png");
	textures["body_green"] = std::make_shared<sf::Texture>("Assets/greenTank.png");
}

void Game::HandleEvents(const std::optional<sf::Event> event)
{
	// Handle key press events passed from window.
	if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
		if (keyPressed->scancode == sf::Keyboard::Scancode::W) {
			tanks.at(0)->isMoving.forward = true;
			tanks.at(0)->isMoving.backward = false;
		}
		else if (keyPressed->scancode == sf::Keyboard::Scancode::S) {
			tanks.at(0)->isMoving.forward = false;
			tanks.at(0)->isMoving.backward = true;
		}
		if (keyPressed->scancode == sf::Keyboard::Scancode::A) {
			tanks.at(0)->isMoving.left = true;
			tanks.at(0)->isMoving.right = false;
		}
		else if (keyPressed->scancode == sf::Keyboard::Scancode::D) {
			tanks.at(0)->isMoving.left = false;
			tanks.at(0)->isMoving.right = true;
		}
		if (keyPressed->scancode == sf::Keyboard::Scancode::Q) {
			tanks.at(0)->isMoving.CCW = true;
			tanks.at(0)->isMoving.CW = false;
			
		}
		else if (keyPressed->scancode == sf::Keyboard::Scancode::E)
		{
			tanks.at(0)->isMoving.CW = true;
			tanks.at(0)->isMoving.CCW = false;
		}
	}

	// Handle key release events passed from window.
	else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
		if (keyReleased->scancode == sf::Keyboard::Scancode::W)
			tanks.at(0)->isMoving.forward = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::S)
			tanks.at(0)->isMoving.backward = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::A)
			tanks.at(0)->isMoving.left = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::D)
			tanks.at(0)->isMoving.right = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::Q)
			tanks.at(0)->isMoving.CCW = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::E)
			tanks.at(0)->isMoving.CW = false;
	}
}

void Game::Update(float dt)
{
	for (int i = 0; i < tanks.size(); i++) {
		tanks.at(0)->Update(dt);
	}
}

// Tank data now includes player id for Observer to update multiple players.
void Game::NetworkUpdate(float dt, TankMessage data) {
	// Force position updates from network data.

	auto& tank = tanks.at(data.id);
	tank->position = { data.x, data.y};
	tank->bodyRotation = sf::degrees(data.bodyRotation);
	tank->barrelRotation = sf::degrees(data.barrelRotation);
	// Update tank with new position.
	// NOTE: This assumets no inputs were detected and so the tank will only move according to 
	// network updates. This is not ideal and prone to unexpected behaviour if game is extended
	// to be fully multiplayer. 
	tank->Update(dt);
}

void Game::Render(sf::RenderWindow& window)
{
	window.draw(*background);
	for (int i = 0; i < tanks.size(); i++) {
		tanks.at(i)->Render(window);
	}
}

void Game::AddTank(std::string body_tex, std::string barrel_tex, sf::Vector2f position)
{
	std::unique_ptr<Tank> tank = std::make_unique<Tank>(Tank(textures[body_tex], textures[barrel_tex]));
	tank->position = position;
	tanks.push_back(std::move(tank));
}

TankMessage Game::GetNetworkUpdate()
{
	// This is hardcoded for the local tan for Player.
	// First value can be anything as we don't care about player ID for local Player updates.
	auto &tank = tanks.at(0);
	return {0, tank->position.x, tank->position.y, tank->bodyRotation.asDegrees(), tank->barrelRotation.asDegrees()};
}
