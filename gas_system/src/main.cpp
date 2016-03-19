#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "ParticleSystem.h"

void balloonScene(ParticleSystem& sys)
{
    size_t count = 30;
    float pi = 3.141592;
    float circleRadius = (sys.getMaxPoint().Length() - sys.getMinPoint().Length()) * 0.25f;
    Vector2f circleCenter = (sys.getMinPoint() + sys.getMaxPoint()) * 0.5f;

    for (size_t i = 0; i < count; i++)
    {
        float ang = 2 * pi * i / (float(count));
        Vector2f pos = Vector2f(ang) * circleRadius + circleCenter;

        sys.addParticle(pos, 20.0f);
    }

    for (size_t i = 0; i < count; i++)
    {
        sys.addLink(sys.getParticle(i), sys.getParticle((i + 1) % count), 0.5f);
        sys.addLink(sys.getParticle(i), sys.getParticle((i + count / 2) % count), 1e-2f);
    }
}

void gasScene(ParticleSystem& sys)
{
    size_t count = 30;
    float pi = 3.141592;
    float circleRadius = (sys.getMaxPoint().Length() - sys.getMinPoint().Length()) * 0.25f;
    Vector2f circleCenter = (sys.getMinPoint() + sys.getMaxPoint()) * 0.5f;

    std::vector<Particle *> tmpParticles;

    for (size_t i = 0; i < count; i++)
    {
        float ang = 2 * pi * i / (float(count));
        Vector2f pos = Vector2f(ang) * circleRadius + circleCenter;

        tmpParticles.push_back(sys.addParticle(pos, 5.0f));
    }

    sys.AddVolumeLink(tmpParticles.data(), int(tmpParticles.size()));

    for (size_t i = 0; i < count; i++) {
        sys.addLink(sys.getParticle(i), sys.getParticle( (i + 1) % count ), 0.5f);
    }
}

void jellyScene(ParticleSystem& sys, size_t xCount, size_t yCount)
{
    Vector2f maxPoint = sys.getMaxPoint();
    Vector2f minPoint = sys.getMinPoint();

    Vector2f jellyStart = (maxPoint - minPoint) * 0.2f;

    for (size_t y = 0; y < yCount; y++) {
        for (size_t x = 0; x < xCount; x++) {
            Vector2f pos = jellyStart + Vector2f(30 * x, 20 * y);

            sys.addParticle(pos, 20.0f);
        }
    }

    for (size_t y = 0; y < yCount; y++) {
        for (size_t x = 0; x < xCount; x++) {
            if (x < xCount - 1) {
                sys.addLink(
                            sys.getParticle(x + y * xCount),
                            sys.getParticle(x + 1 + y * xCount),
                            0.1f);
            }
            if (y < yCount - 1) {
                sys.addLink(
                            sys.getParticle(x + y * xCount),
                            sys.getParticle(x + (y + 1) * xCount),
                            0.1f);
            }

            if (y < yCount - 1) {
                if (x < xCount - 1) {
                    sys.addLink(
                                sys.getParticle(x + y * xCount),
                                sys.getParticle(x + 1 + (y + 1) * xCount),
                                0.1f);
                }

                if (x > 0) {
                    sys.addLink(
                                sys.getParticle(x + y * xCount),
                                sys.getParticle(x - 1 + (y + 1) * xCount),
                                0.1f);
                }
            }
        }
    }
}

//class Scene

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "window");

	sf::Texture tex;
    if (tex.loadFromFile("../project/data/Atlas.png"))
	{
		std::cout << "Load succesfull\n";
	}

	else
	{
		std::cout << "Load failed\n";
	}

	Particle p;
//    ParticleSystem sys (Vector2f(0.0f, 0.0f), Vector2f(window.getSize().x - 50.f, window.getSize().y - 50.f));
    ParticleSystem sys (Vector2f(0.0f, 0.0f), Vector2f(window.getSize().x - 50.f, window.getSize().y - 50.f), 0.008f, 0.01f);
    gasScene(sys);
//    balloonScene(sys);
//    jellyScene(sys, 5, 4);

	while (window.isOpen())
	{
		sf::Event evt;

		while (window.pollEvent(evt))
		{
            if (evt.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				window.close();
			}
		}

		sys.Update();
		window.clear();
		for (size_t i = 0; i < sys.GetParticlesCount(); i++)
		{
			sf::Vertex vertices[4];
			float xSize = tex.getSize().x;
			float ySize = tex.getSize().y;
			vertices[0].texCoords = sf::Vector2f(xSize * 0.75f, ySize * 0.75f);
			vertices[1].texCoords = sf::Vector2f(xSize, ySize * 0.75f);
			vertices[2].texCoords = sf::Vector2f(xSize, ySize);
			vertices[3].texCoords = sf::Vector2f(xSize * 0.75f, ySize);

            Particle p = * sys.getParticle(i);

			vertices[0].position = sf::Vector2f(p.pos.x - p.radius, p.pos.y - p.radius);
			vertices[1].position = sf::Vector2f(p.pos.x + p.radius, p.pos.y - p.radius);
			vertices[2].position = sf::Vector2f(p.pos.x + p.radius, p.pos.y + p.radius);
			vertices[3].position = sf::Vector2f(p.pos.x - p.radius, p.pos.y + p.radius);
			window.draw(vertices, 4, sf::Quads, &tex);
		}

		window.display();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
            sys.Push_Right(Vector2f(0.18f, 0.0f));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
            sys.Push_Left(Vector2f(-0.18f, 0.f));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
            sys.Push_Up(Vector2f(0.0f, 0.5f));
		}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            sys.Push_Up(Vector2f(0.0f, -0.18f));
        }

	}
	return 0;
}



















































/*
	sf::Vertex vertices[4];
	vertices[0].position = sf::Vector2f(100.0f, 100.0f);
	vertices[1].position = sf::Vector2f(400.0f, 100.0f);
	vertices[2].position = sf::Vector2f(400.0f, 400.0f);
	vertices[3].position = sf::Vector2f(100.0f, 400.0f);
	for (int i = 0; i < 4; i++)
	{
		vertices[i].color = sf::Color::White;
	}
	float xSize = tex.getSize().x;
	float ySize = tex.getSize().y;
	vertices[0].texCoords = sf::Vector2f(xSize * 0.75f, ySize * 0.75f);
	vertices[1].texCoords = sf::Vector2f(xSize, ySize * 0.75f);
	vertices[2].texCoords = sf::Vector2f(xSize, ySize);
	vertices[3].texCoords = sf::Vector2f(xSize * 0.75f, ySize);

	while (window.isOpen())
	{
		sf::Event evt;
		while (window.pollEvent(evt))
		{
			if (evt.type == sf::Event::Closed)
			{
				window.close();
			}
		}
		window.clear();
		window.draw(vertices, 4, sf::Quads, &tex);
		window.display();

	}

	/*int a = 0;
	double b = 3.14;
	//	std::cin >> a;
	a++;
	a++;
	//	std::cout << "a = " << a << std::endl;
	return 0;
}
*/
