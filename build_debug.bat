@echo off
g++ -std=c++20 -Wall -Wextra main.cpp -o bin\Debug\epicycle -I SFML\include -L SFML\lib -lsfml-main-d -lsfml-graphics-d -lsfml-system-d -lsfml-window-d