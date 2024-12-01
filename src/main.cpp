#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

enum gamestate {menu, game_ai, game_2p, pause, over};
enum modes {ai, p2};

const int WINDOW_SIZE = 800;
const int BOARD_SIZE = 8;
const int BLOCK_SIZE = WINDOW_SIZE / BOARD_SIZE;
const int MENU_OPTIONS = 4;
const int PAUSE_OPTIONS = 3;
modes GAME_MODE;
char DISCS[][BOARD_SIZE]{'-'};
int TURN = 0;

gamestate CUR_STATE = menu;
int menuSel = 0;
int pauseSel = 0;

void initBoard(sf::RectangleShape[][BOARD_SIZE]);
void menuInput(sf::RenderWindow &window);
void menuDisplay(sf::RenderWindow &window);
void gameInput(sf::RenderWindow &window, sf::Event &event, bool aiMode);
void gameDisplay(sf::RenderWindow &window, sf::RectangleShape[][BOARD_SIZE]);
void setDiscs(std::ifstream &file);
void makeMove(int, int, char);
bool validDir(int, int, int, int, char);
void countDiscs(int &, int &);
void callAI(int &, int &, char);
void saveFile(std::ofstream &file);
void pauseInput(sf::RenderWindow &window);
void pauseDisplay(sf::RenderWindow &window);

int main() {
	sf::RenderWindow window(
		sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE),
		"Reversi",
		sf::Style::Titlebar | sf::Style::Close
	);
	window.setFramerateLimit(60);

	sf::RectangleShape board[BOARD_SIZE][BOARD_SIZE];
	initBoard(board);
	std::ifstream initFile("board_init.txt");
	setDiscs(initFile);
	initFile.close();

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();

			switch(CUR_STATE) {
				case menu:
					menuInput(window);
					break;
				case game_ai:
					gameInput(window, event, true);
					break;
				case game_2p:
					gameInput(window, event, false);
					break;
				case pause:
					pauseInput(window);
					break;
			}

			window.clear();

			switch (CUR_STATE) {
				case menu:
					menuDisplay(window);
					break;
				case game_ai:
					gameDisplay(window, board);
					break;
				case game_2p:
					gameDisplay(window, board);
					break;
				case pause:
					pauseDisplay(window);
					break;
			}
			window.display();
		}
	}
}

void menuDisplay(sf::RenderWindow &window) {
	sf::Font font;
	font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");

	const char* menuOpts[] = {"New Game with AI", "New Game with 2P", "Load Save", "Exit"};

	sf::Text titleText("REVERSI", font, 50);
	titleText.setFillColor(sf::Color::White);
	titleText.setPosition(300, 100);
	window.draw(titleText);

	for (int i = 0; i < MENU_OPTIONS; ++i) {
		sf::Text optText(menuOpts[i], font, 30);

		if (i == menuSel) {
			optText.setFillColor(sf::Color::Red);
		} else {
			optText.setFillColor(sf::Color::White);
		}

		optText.setPosition(350, 250 + i * 50);
		window.draw(optText);
	}
}

void menuInput(sf::RenderWindow &window) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		menuSel = (menuSel - 1 + MENU_OPTIONS) % MENU_OPTIONS;
		sf::sleep(sf::milliseconds(150));
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		menuSel = (menuSel + 1) % MENU_OPTIONS;
		sf::sleep(sf::milliseconds(150));
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
		switch (menuSel) {
			case 0:
				CUR_STATE = game_ai;
				GAME_MODE = ai;
				menuSel = 0;
				break;
			case 1:
				CUR_STATE = game_2p;
				GAME_MODE = p2;
				menuSel = 0;
				break;
			case 2: {
				int mode;
				std::ifstream load("saved_board.txt");
				setDiscs(load);
				load >> mode >> TURN;
				if(mode == 0) {GAME_MODE = ai; CUR_STATE = game_ai;}
				else if(mode == 1) {GAME_MODE = p2; CUR_STATE = game_2p;}
				load.close();
				break;
			}
			case 3:
				window.close();
				break;
		}
		sf::sleep(sf::milliseconds(200));
	}
}

void gameInput(sf::RenderWindow &window, sf::Event &event, bool aiMode) {
	int posX, posY;
	char curPlayer = (TURN % 2 == 0) ? 'X' : 'O';

	if(aiMode && TURN % 2 == 1) {
		callAI(posX, posY, curPlayer);
		sf::sleep(sf::milliseconds(150));
		makeMove(posX, posY, curPlayer);
		TURN++;

	}
	else if (event.type == sf::Event::MouseButtonPressed) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		posX = mousePos.x / BLOCK_SIZE;
		posY = mousePos.y / BLOCK_SIZE;

		if (DISCS[posY][posX] == '-') {
			makeMove(posX, posY, curPlayer);
			TURN++;
		}
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
		CUR_STATE = pause;
	}

}

void gameDisplay(sf::RenderWindow &window, sf::RectangleShape board[][BOARD_SIZE]) {
	for (int y = 0; y < BOARD_SIZE; ++y) {
		for (int x = 0; x < BOARD_SIZE; ++x) {
			window.draw(board[y][x]);
		}
	}

	for (int y = 0; y < BOARD_SIZE; ++y) {
		for (int x = 0; x < BOARD_SIZE; ++x) {
			if (DISCS[y][x] != '-') {
				sf::CircleShape disc(BLOCK_SIZE / 2 - 5);
				disc.setPointCount(60);

				disc.setPosition(x * BLOCK_SIZE + (BLOCK_SIZE - disc.getRadius() * 2) / 2,
								 y * BLOCK_SIZE + (BLOCK_SIZE - disc.getRadius() * 2) / 2);
				if (DISCS[y][x] == 'X') {
					disc.setFillColor(sf::Color::White);
				} else if (DISCS[y][x] == 'O') {
					disc.setFillColor(sf::Color::Black);
				}
				window.draw(disc);
			}
		}
	}
}

void initBoard(sf::RectangleShape board[][BOARD_SIZE]) {
	for (int y = 0; y < BOARD_SIZE; y++) {
		for (int x = 0; x < BOARD_SIZE; x++) {
			board[y][x].setSize(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE));

			board[y][x].setPosition(
				x * BLOCK_SIZE,
				y * BLOCK_SIZE
			);

			board[y][x].setFillColor(
				(x + y) % 2 == 0 ? sf::Color(43, 181, 55) : sf::Color(39, 150, 33)
			);

			board[y][x].setOutlineThickness(1.0f);
			board[y][x].setOutlineColor(sf::Color::Black);
		}
	}
}

void setDiscs(std::ifstream &file) {
	for (int y = 0; y < BOARD_SIZE; ++y) {
		for (int x = 0; x < BOARD_SIZE; ++x) {
			file.get(DISCS[y][x]);
		}
		file.ignore();
	}
}

void makeMove(int posX, int posY, char player) {
	DISCS[posY][posX] = player;

	int directions[8][2] = {
		{-1, 0}, {1, 0}, {0, -1}, {0, 1},
		{-1, -1}, {-1, 1}, {1, -1}, {1, 1}
	};

	for (int i = 0; i < 8; i++) {
		int dirX = directions[i][0];
		int dirY = directions[i][1];

		if (validDir(dirX, dirY, posX, posY, player)) {
			int curX = posX + dirX;
			int curY = posY + dirY;

			while (DISCS[curY][curX] != player) {
				DISCS[curY][curX] = player;
				curX += dirX;
				curY += dirY;
			}
		}
	}
}

bool validDir(int dirX, int dirY, int posX, int posY, char player) {
	int curX = posX + dirX;
	int curY = posY + dirY;
	char opp = (player == 'X') ? 'O' : 'X';;

	if (curY < 0 || curY >= BOARD_SIZE || curX < 0 || curX >= BOARD_SIZE || DISCS[curY][curX] != opp)
		return false;

	curX += dirX;
	curY += dirY;
	while (curX >= 0 && curX < BOARD_SIZE && curY >= 0 && curY < BOARD_SIZE) {
		if (DISCS[curY][curX] == player) return true;
		else if (DISCS[curY][curX] == '-') return false;

		curX += dirX;
		curY += dirY;
	}
	return false;
}

void countDiscs(int &black, int &white) {
	black = 0, white = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (DISCS[i][j] == 'X') white++;
			else if (DISCS[i][j] == 'O') black++;
		}
	}
}

void saveFile(std::ofstream &file) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			file << DISCS[i][j];
		}
		file << '\n';
	}
	file << GAME_MODE << " " << TURN;
}

 void pauseDisplay(sf::RenderWindow &window) {
	sf::Font font;
	font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");

	const char* menuOpts[] = {"Resume", "Save", "Exit"};

	sf::Text titleText("PAUSED", font, 50);
	titleText.setFillColor(sf::Color::White);
	titleText.setPosition(300, 100);
	window.draw(titleText);

	for (int i = 0; i < PAUSE_OPTIONS; ++i) {
		sf::Text optText(menuOpts[i], font, 30);

		if (i == pauseSel) {
			optText.setFillColor(sf::Color::Red);
		} else {
			optText.setFillColor(sf::Color::White);
		}

		optText.setPosition(350, 250 + i * 50);
		window.draw(optText);
	}
}

void pauseInput(sf::RenderWindow &window) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		pauseSel = (pauseSel - 1 + PAUSE_OPTIONS) % PAUSE_OPTIONS;
		sf::sleep(sf::milliseconds(150));
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		pauseSel = (pauseSel + 1) % PAUSE_OPTIONS;
		sf::sleep(sf::milliseconds(150));
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
		switch (pauseSel) {
			case 0: {
				if(GAME_MODE == ai) CUR_STATE = game_ai;
				else if (GAME_MODE == p2) CUR_STATE = game_2p;
				break;
			}
			case 1: {
				std::ofstream file("saved_board.txt");
				saveFile(file);
				break;
			}
			case 2:
				window.close();
				break;
		}
	}
}

void callAI(int &posX, int &posY, char player) {
	char opp = (player == 'X') ? 'O' : 'X';
	int bestScore = -1;
	int bestX = -1, bestY = -1;

	for (int y = 0; y < BOARD_SIZE; ++y) {
		for (int x = 0; x < BOARD_SIZE; ++x) {
			if (DISCS[y][x] != '-') continue;

			int currentScore = 0;

			int directions[8][2] = {
				{-1, 0}, {1, 0}, {0, -1}, {0, 1},
				{-1, -1}, {-1, 1}, {1, -1}, {1, 1}
			};

			for (int i = 0; i < 8; ++i) {
				int dirX = directions[i][0];
				int dirY = directions[i][1];

				if (validDir(dirX, dirY, x, y, player)) {
					int curX = x + dirX;
					int curY = y + dirY;

					while (DISCS[curY][curX] == opp) {
						currentScore++;
						curX += dirX;
						curY += dirY;
					}
				}
			}
			if (currentScore > bestScore) {
				bestScore = currentScore;
				bestX = x;
				bestY = y;
			}

		}
	}

	if (bestScore == 0) {
		std::srand(static_cast<unsigned>(std::time(nullptr)));
		do {
			bestX = std::rand() % BOARD_SIZE;
			bestY = std::rand() % BOARD_SIZE;
		} while(DISCS[bestY][bestX] != '-');
	}

	if (bestX != -1 && bestY != -1) {
		posX = bestX;
		posY = bestY;
	}
}




