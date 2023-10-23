#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
using namespace std;

struct ln
{
    string txt;
    vector<sf::Color> color;
};

vector<sf::Color> setLineColor(int len, sf::Color col);
void highlightLine(ln& line);