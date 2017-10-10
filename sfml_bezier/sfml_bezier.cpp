#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>
#include "Curves.h"

void HandleEvents(sf::RenderWindow&);
void HandleInput(sf::RenderWindow&);

bool lmbd = false;
bool lmbp = false;
bool lmbr = false;
sf::Vector2i mpos;
SnakeCurve dcurve(256, 15, 1.0f, 1.0f, sf::Vector2f(50, 50), sf::Vector2f(950,950));
Bezier bezier(6, 100, 25, sf::Vector2f(100, 100), sf::Vector2f(1800, 100));

int main()
{
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "Bezier curves.", sf::Style::Close | sf::Style::Titlebar);

	while (window.isOpen()) {
		HandleEvents(window);
		HandleInput(window);
		
		dcurve.update(lmbp, lmbr, mpos);
		bezier.update(lmbp, lmbr, mpos);
		bezier.DrawLoop(true);

		window.clear();
		{
			dcurve.draw(window);
			bezier.draw(window);
		}
		window.display();
	}
	
    return 0;
}

void HandleEvents(sf::RenderWindow& window) {
	sf::Event ev;
	while (window.pollEvent(ev)) {
		if (ev.type == sf::Event::Closed) window.close();
	}
}

void HandleInput(sf::RenderWindow& window) {
	if (lmbr) lmbr = false;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		mpos = sf::Mouse::getPosition(window);
		if (lmbd) lmbp = false;
		else lmbp = true;
		lmbd = true;
	}
	else {
		if (lmbd) lmbr = true;
		lmbd = false;
	}
}