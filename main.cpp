#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <tuple>

#define WINDOW_WIDTH 1400
#define WINDOW_HEIGHT 600
#define BAR_WIDTH 200
#define BAR_HEIGHT 25
#define BALL_RESET_OFFSET 5

template <typename T>

sf::Vector2<T> componentWiseVectorMultiplication(const sf::Vector2<T>& a, const sf::Vector2<T>& b)
{
    sf::Vector2<T> returnValue;
    returnValue.x = a.x * b.x;
    returnValue.y = a.y * b.y;

    return returnValue;
}

void clampHorizontal(sf::RectangleShape& a)
{
    sf::Vector2f pos = a.getPosition(); 
    if (pos.x <= 0)
    {
        pos.x = 0;
    }
    else if (pos.x + a.getSize().x >= WINDOW_WIDTH)
    {

        pos.x = WINDOW_WIDTH - a.getSize().x;
    }
    a.setPosition(pos);
}

bool intersects(sf::CircleShape& circle, sf::RectangleShape& rectangle)
{
    // adapted from: https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection
    sf::Vector2f circleCentre = sf::Vector2f(circle.getPosition().x + circle.getRadius(), circle.getPosition().y + circle.getRadius());
    float circleDistanceX = abs(circleCentre.x - (rectangle.getPosition().x + rectangle.getSize().x/2)); 
    float circleDistanceY = abs(circleCentre.y - (rectangle.getPosition().y + rectangle.getSize().y/2)); 

    if (circleDistanceX > (rectangle.getSize().x/2 + circle.getRadius())) { return false;}
    if (circleDistanceY > (rectangle.getSize().y/2 + circle.getRadius())) { return false;}


    if (circleDistanceX <= (rectangle.getSize().x/2)) { return true;}
    if (circleDistanceY <= (rectangle.getSize().y/2)) { return true;}


    float cornerDistance_sq = pow((circleDistanceX - rectangle.getSize().x/2), 2) +
                              pow((circleDistanceY - rectangle.getSize().y/2), 2); 
    
    return cornerDistance_sq <= pow(circle.getRadius(), 2);
}


// TODO: Abstract logic into classes
int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Brick Breaker");
    // enable V-Sync
    //window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(120);

    // bat
    sf::RectangleShape bat = sf::RectangleShape(sf::Vector2f(300, 25));
    bat.setPosition(WINDOW_WIDTH/2 - bat.getSize().x/2, WINDOW_HEIGHT - bat.getSize().y - 10);
    bat.setFillColor(sf::Color::Red);
    float batMoveValue = 10.0f;

    // bars
    std::vector<sf::Color> barColours = {sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow, sf::Color::Magenta};
    std::vector<std::tuple<sf::RectangleShape, bool>> bars;
    for (int y = 0;  y < (5*BAR_HEIGHT); y += (BAR_HEIGHT+5))
    {
        for(int x = 0; x < WINDOW_WIDTH; x += (BAR_WIDTH+5))
        {
            sf::RectangleShape bar = sf::RectangleShape(sf::Vector2f(BAR_WIDTH, BAR_HEIGHT));
            bar.setPosition(x, y);
            int colourIdx = static_cast<int>(y / BAR_HEIGHT); 
            bar.setFillColor(barColours.at(colourIdx));
            bars.push_back(std::make_tuple(bar, true));
        }
    }

    // ball
    float radius = 15.0f;
    sf::CircleShape ball(radius);
    ball.setFillColor(sf::Color::Green);
    ball.setPosition(WINDOW_WIDTH/2-radius, WINDOW_HEIGHT/2-radius);
    sf::Vector2f direction = sf::Vector2f(1.0f, 0.5f);
    sf::Vector2f velocity = sf::Vector2f(5.0f, 3.0f);


    bool pause = false;
    // game loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::P)
            {
                pause = !pause;
            }
       }

       if (!pause)
       {
            // user input (update bat position)
            bat.setPosition(sf::Mouse::getPosition(window).x - bat.getSize().x / 2, bat.getPosition().y);
            clampHorizontal(bat);

            // ball logic
            ball.move(componentWiseVectorMultiplication(direction, velocity));

            // collison detection
            if ((ball.getPosition().x + (2 * radius)) >= WINDOW_WIDTH or (ball.getPosition().x <= 0))
            {
                velocity.x *= -1;
            }
            if ((ball.getPosition().y + (2 * radius)) >= WINDOW_HEIGHT or (ball.getPosition().y <= 0))
            {
                velocity.y *= -1;
            }

            if (intersects(ball, bat))
            {
                velocity.y *= -1;
                // incase of side on collisons set ball height slightly above top surface of bat.
                ball.setPosition(ball.getPosition().x, bat.getPosition().y - bat.getSize().y - BALL_RESET_OFFSET);
            }

            window.clear();
            window.draw(bat);
            for (auto &bar : bars)
            {
                // if bar is active
                if (std::get<1>(bar))
                {
                    if (intersects(ball, std::get<0>(bar)))
                    {
                        std::get<1>(bar) = false;
                        velocity.y *= -1;
                    }
                    window.draw(std::get<0>(bar));
                }
            }
            window.draw(ball);
       }

       window.display();
    }

    return 0;
}
