
/**
 * Fourier Transform: Drawing with Epicycles
 * By Mckenzie Regalado
 * 5/24/2024
*/


#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

#include <iostream>
#include <vector>
#include <complex>
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <cmath>

#define TWO_PI 6.283185307179586

struct Epicycle
{
    /* data */
    double radius;
    double phase;
    int freq;
};

/** Function declaration */
std::vector<std::complex<double>> DFT(std::vector<std::complex<double>> &X_N);
bool sortByRadius(struct Epicycle &a, struct Epicycle &b);

int main(void)
{
    int W = 500, H = 500;
    sf::RenderWindow window(sf::VideoMode(W, H), "App", sf::Style::Close);
    window.setFramerateLimit(60);

    std::vector<std::complex<double>> X_N{};
    std::vector<std::complex<double>> X_K{};
    std::vector<Epicycle> epicycles{};
    std::vector<sf::Vector2f> path{};
    std::vector<sf::CircleShape> circles{};

    double time = 0.0f;
    double dt = 0.0f;

    double max_t = 0.16f;
    double a = 0.0f;

    double animation_speeding_factor = 5.0f;

    double px = 0, py = 0;

    bool draw = false;

    bool visualize = false;

    sf::Font font;
    sf::Text text1;
    sf::Text text2;

    if (font.loadFromFile("font/OpenSans-Regular.ttf"))
    {
        text1.setFont(font);
        text1.setString("Click and hold the left-button of your mouse to draw on the canvas.");
        text1.setCharacterSize(12);
        text1.setFillColor(sf::Color::White);
        text1.setPosition(sf::Vector2f(5.0f, 12.0f));

        text2.setFont(font);
        text2.setString("Press the letter 'v' on your keyboard to visualize the epicycle and 'r' to clear the canvas.");
        text2.setCharacterSize(11);
        text2.setFillColor(sf::Color::White);
        text2.setPosition(sf::Vector2f(5.0f, W - 20));
    }

    // Colors
    sf::Color circle_color(46, 196, 182, 255);
    sf::Color line_color(231, 29, 54, 255);
    sf::Color draw_color(255, 159, 28, 255);
    sf::Color path_color(253, 255, 252);

    sf::Clock deltaClock;

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // Get FPS
        sf::Time deltaTime = deltaClock.restart();
        double currentTime = deltaTime.asSeconds();
        double fps = 1.0f / currentTime;
        std::cout << fps << '\n';

        // clear window
        window.clear(sf::Color(1, 22, 39, 255));

        // Draw Text
        window.draw(text1);
        window.draw(text2);

        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                if (!visualize)
                {
                    sf::Vector2i m_pos = sf::Mouse::getPosition(window);
                    X_N.push_back(std::complex((double)m_pos.x, (double)m_pos.y));
                    draw = true;
                }
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.scancode == sf::Keyboard::Scan::V)
                {
                    if (!visualize && X_N.size() > 0)
                    {
                        // DO FOURIER TRANSFORM
                        X_K = DFT(X_N);

                        int N = (int)X_K.size();

                        dt = TWO_PI / N;

                        for (int k = 0; k < N; k++)
                        {
                            /* code */
                            struct Epicycle ep;
                            ep.radius = std::abs(X_K[k]) / N; // std::abs(z) - returns the absolute value (also known as norm, modulus, or magnitude) of a complex value z.
                            ep.phase = std::arg(X_K[k]);      // std::arg(z) - calculates the phase angle (in radians) of the complex number z.
                            ep.freq = k;
                            epicycles.push_back(ep);
                        }

                        // Sort epicycles by their radius
                        std::sort(epicycles.begin(), epicycles.end(), sortByRadius);

                        for (int k = 0; k < N; k++)
                        {
                            /* code */
                            struct Epicycle ep = epicycles[k];
                            double radius = ep.radius;

                            // Create circle
                            sf::CircleShape circle(radius);
                            circle.setFillColor(sf::Color::Transparent);
                            circle.setOutlineThickness(1.2);
                            circle.setOutlineColor(circle_color);
                            circles.push_back(circle);
                        }

                        visualize = true;
                        draw = false;
                    }
                }
            }

            // reset
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.scancode == sf::Keyboard::Scan::R)
                {
                    X_N.erase(X_N.begin(), X_N.end());
                    X_K.erase(X_K.begin(), X_K.end());

                    epicycles.erase(epicycles.begin(), epicycles.end());
                    path.erase(path.begin(), path.end());
                    circles.erase(circles.begin(), circles.end());

                    visualize = false;
                    time = 0.0;
                    dt = 0.0;
                    a = 0.0;
                }
            }
        }

        // Draw Line
        if (draw && X_N.size() > 0)
        {
            for (size_t i = 0; i < X_N.size() - 1; i++)
            {
                /* code */
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(X_N[i].real(), X_N[i].imag()), draw_color),
                    sf::Vertex(sf::Vector2f(X_N[i + 1].real(), X_N[i + 1].imag()), draw_color)};
                window.draw(line, 2, sf::Lines);
            }
        }

        if (visualize)
        {
            double x = 0.0, y = 0.0;

            /** Draw Epicycles */
            for (int k = 0; k < (int)X_K.size(); k++)
            {
                /* code */

                struct Epicycle ep = epicycles[k];
                double freq = ep.freq;
                double radius = ep.radius;
                double phase = ep.phase;

                // Update position
                double prev_x = x, prev_y = y;

                x += radius * cosf(freq * time + phase);
                y += radius * sinf(freq * time + phase);

                // Draw circles
                circles[k].setPosition(sf::Vector2f(prev_x - radius, prev_y - radius));
                window.draw(circles[k]);

                // draw lines
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(prev_x, prev_y), line_color),
                    sf::Vertex(sf::Vector2f(x, y), line_color)};
                window.draw(line, 2, sf::Lines);
            }

            /** avoid duplicate insertion. */
            if (x != px && y != py)
            {
                path.insert(path.begin(), sf::Vector2f(x, y));
                px = x, py = y;
            }

            // draw the path
            for (size_t p = 0; p < path.size() - 1; p++)
            {
                /* code */
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(path[p].x, path[p].y), path_color),
                    sf::Vertex(sf::Vector2f(path[p + 1].x, path[p + 1].y), path_color)};
                window.draw(line, 2, sf::Lines);
            }

            a += currentTime * animation_speeding_factor;

            if (a > max_t)
            {
                time += dt; // update time
                a = 0.0f;
            }

            if (time > TWO_PI) // 1 revolution,
            {
                time = 0.0; // reset time
                path.erase(path.begin(), path.end());
            }
        }
        window.display();
    }
    return 0;
}

std::vector<std::complex<double>> DFT(std::vector<std::complex<double>> &X_N)
{
    int N = (int)X_N.size();

    std::vector<std::complex<double>> X_K{};

    std::complex<double> i(0.0, 1.0);

    for (int k = 0; k < N; k++)
    {
        /* code */
        std::complex<double> t(0.0, 0.0);
        for (int n = 0; n < N; n++)
        {
            /* code */
            double temp = (TWO_PI * n * k) / N;
            t += std::exp(-i * temp) * X_N[n];
        }
        X_K.push_back(t);
    }
    return X_K;
}

bool sortByRadius(struct Epicycle &a, struct Epicycle &b)
{
    return a.radius > b.radius;
}
