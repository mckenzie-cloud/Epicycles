@echo off
g++ -std=c++20 -s -O2 main.cpp -o bin\Release\epicycle -I SFML\include -L SFML\lib -lsfml-main -lsfml-graphics -lsfml-system -lsfml-window