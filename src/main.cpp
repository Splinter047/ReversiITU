#include <SFML/Graphics.hpp>

int main()
{
	auto window = sf::RenderWindow({800u, 800u}, "Reversi");
	window.setFramerateLimit(60);

	while (window.isOpen())
	{
		for (auto event = sf::Event(); window.pollEvent(event);)
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear();
		window.display();
	}
}