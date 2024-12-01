#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>

const int WINDOW_SIZE = 800;
const int SIZE = 8;
const float BLOCK_SIZE = WINDOW_SIZE / SIZE;
int TURN = 0;

void setBoard(sf::RectangleShape [][SIZE]);

void gameLoop(sf::RenderWindow &);

void drawBoard(sf::RenderWindow &, sf::RectangleShape [][SIZE], char [][SIZE]);

void initDiscs(char [][SIZE], std::ifstream &);

void makeMove(char [][SIZE], int, int, char);

bool validDir(char [][SIZE], int, int, int, int, char);

void countChars(char [][SIZE], int &, int &);

int main() {
	sf::RenderWindow window(
		sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE),
		"Reversi",
		sf::Style::Titlebar | sf::Style::Close
	);
	window.setFramerateLimit(60);

	gameLoop(window);
}

void gameLoop(sf::RenderWindow &window) {
	sf::RectangleShape board[SIZE][SIZE];
	setBoard(board);

	std::ifstream initFile("board_init.txt");
	char discs[SIZE][SIZE]{'-'};
	initDiscs(discs, initFile);
	int blackCounts = 0;
	int whiteCounts = 0;

	sf::Font font;
	font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");
	sf::Text gameOver = sf::Text("Game Over", font);
	gameOver.setCharacterSize(80);
	gameOver.setColor(sf::Color::White);
	gameOver.setPosition(WINDOW_SIZE / 4, WINDOW_SIZE / 2);


	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseButtonPressed) {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);

				int gridX = mousePos.x / BLOCK_SIZE;
				int gridY = mousePos.y / BLOCK_SIZE;

				if (discs[gridY][gridX] == '-') {
					char curPlayer;
					if (TURN % 2 == 0) curPlayer = 'X';
					else curPlayer = 'O';
					makeMove(discs, gridX, gridY, curPlayer);
					countChars(discs, blackCounts, whiteCounts);
					TURN++;
				}
			}
		}

		drawBoard(window, board, discs);

		if ((blackCounts + whiteCounts) == 64) window.draw(gameOver);

		window.display();
	}
}

void drawBoard(sf::RenderWindow &window, sf::RectangleShape board[][SIZE], char discs[][SIZE]) {
	window.clear();
	for (int y = 0; y < SIZE; ++y) {
		for (int x = 0; x < SIZE; ++x) {
			window.draw(board[y][x]);
		}
	}

	for (int y = 0; y < SIZE; ++y) {
		for (int x = 0; x < SIZE; ++x) {
			if (discs[y][x] != '-') {
				sf::CircleShape disc(BLOCK_SIZE / 2 - 5);
				disc.setPointCount(60);

				disc.setPosition(x * BLOCK_SIZE + (BLOCK_SIZE - disc.getRadius() * 2) / 2,
				                 y * BLOCK_SIZE + (BLOCK_SIZE - disc.getRadius() * 2) / 2);
				if (discs[y][x] == 'X') {
					disc.setFillColor(sf::Color::White);
				} else if (discs[y][x] == 'O') {
					disc.setFillColor(sf::Color::Black);
				}
				window.draw(disc);
			}
		}
	}
}

void initDiscs(char discs[][SIZE], std::ifstream &input) {
	for (int y = 0; y < SIZE; ++y) {
		for (int x = 0; x < SIZE; ++x) {
			input.get(discs[y][x]);
		}
		input.ignore();
	}
}

void setBoard(sf::RectangleShape board[][8]) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
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

void makeMove(char discs[][SIZE], int posX, int posY, char player) {
	discs[posY][posX] = player;

	int directions[SIZE][2] = {
		{-1, 0}, {1, 0}, {0, -1}, {0, 1},
		{-1, -1}, {-1, 1}, {1, -1}, {1, 1}
	};

	for (int i = 0; i < SIZE; i++) {
		int dirX = directions[i][0];
		int dirY = directions[i][1];

		if (validDir(discs, dirX, dirY, posX, posY, player)) {
			int curX = posX + dirX;
			int curY = posY + dirY;

			while (discs[curY][curX] != player) {
				discs[curY][curX] = player;
				curX += dirX;
				curY += dirY;
			}
		}
	}
}

bool validDir(char discs[][SIZE], int dirX, int dirY, int posX, int posY, char player) {
	int curX = posX + dirX;
	int curY = posY + dirY;
	char opp;

	if (player == 'X') opp = 'O';
	else opp = 'X';

	if (curY < 0 || curY >= SIZE || curX < 0 || curX >= SIZE || discs[curY][curX] != opp)
		return false;

	curX += dirX;
	curY += dirY;
	while (curX >= 0 && curX < SIZE && curY >= 0 && curY < SIZE) {
		if (discs[curY][curX] == player) return true;
		else if (discs[curY][curX] == '-') return false;

		curX += dirX;
		curY += dirY;
	}
	return false;
}

void countChars(char discs[][SIZE], int &blacks, int &whites) {
	blacks = 0, whites = 0;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (discs[i][j] == 'X') whites++;
			else if (discs[i][j] == 'O') blacks++;
		}
	}
}
