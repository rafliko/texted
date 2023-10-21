#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <SFML/Graphics.hpp>
#include "config.h"
using namespace std;

string filePath = "";
string status = "";

vector<string> lines;

int spacing = 1;
int charHeight = fontSize;
int charWidth = fontSize / 2 + spacing;

int offsetX = 7;
int offsetY = 0;

int cursorX = 0;
int cursorY = 0;

int main()
{
    setlocale(LC_ALL, "");
    while (true)
    {
        cout << "Path to file: ";
        cin >> filePath;

        ifstream file(filePath);
        if (!file.fail())
        {
            string line;
            while (getline(file, line))
            {
                lines.push_back(line);
            }
            file.close();
            status = filePath + "\tL:"+to_string(cursorY+1)+"\tC:"+to_string(cursorX);
        }
        else
        {
            continue;
        }

        sf::Font font;
        font.loadFromFile("Fonts/Hack/Hack-Regular.ttf");

        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(fontSize);

        sf::RectangleShape rect;

        sf::Cursor ibeam;
        ibeam.loadFromSystem(sf::Cursor::Text);

        sf::RenderWindow window(sf::VideoMode(1200, 800), "texted");
        window.setFramerateLimit(60);
        window.setMouseCursor(ibeam);

        while (window.isOpen())
        {
            // events
            ///////////////////////////////////////////////////////////////////////
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    lines.clear();
                    window.close();
                }

                if (event.type == sf::Event::Resized)
                {
                    // update the view to the new size of the window
                    sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                }

                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.scancode == sf::Keyboard::Scan::Left) cursorX--;
                    if (event.key.scancode == sf::Keyboard::Scan::Right) cursorX++;
                    if (event.key.scancode == sf::Keyboard::Scan::Up) cursorY--;
                    if (event.key.scancode == sf::Keyboard::Scan::Down) cursorY++;

                    if (event.key.scancode == sf::Keyboard::Scan::Home) cursorX = 0;
                    if (event.key.scancode == sf::Keyboard::Scan::End) cursorX = lines[cursorY].length();

                    if (event.key.scancode == sf::Keyboard::Scan::PageUp) offsetY += window.getSize().y / charHeight - 2;
                    if (event.key.scancode == sf::Keyboard::Scan::PageDown) offsetY -= window.getSize().y / charHeight - 2;

                    if (event.key.scancode == sf::Keyboard::Scan::Delete && cursorX < lines[cursorY].length())
                    {
                        lines[cursorY].erase(cursorX, 1);
                    }
                    if (event.key.scancode == sf::Keyboard::Scan::Delete && cursorX == lines[cursorY].length())
                    {
                        lines[cursorY] += lines[cursorY + 1];
                        lines.erase(lines.begin() + cursorY + 1);
                    }
                    if (event.key.scancode == sf::Keyboard::Scan::Backspace && cursorX > 0)
                    {
                        lines[cursorY].erase(cursorX - 1, 1);
                        cursorX--;
                    }
                    if (event.key.scancode == sf::Keyboard::Scan::Backspace && cursorX == 0)
                    {
                        cursorY--;
                        cursorX = lines[cursorY].length();
                        lines[cursorY] += lines[cursorY+1];
                        lines.erase(lines.begin() + cursorY+1);
                    }
                    if (event.key.scancode == sf::Keyboard::Scan::Enter)
                    {
                        lines.insert(lines.begin()+cursorY+1,lines[cursorY].substr(cursorX));
                        lines[cursorY] = lines[cursorY].substr(0, cursorX);
                        cursorX = 0;
                        cursorY++;
                    }
                }

                if (event.type == sf::Event::MouseWheelScrolled)
                {
                    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                    {
                        if ((int)event.mouseWheelScroll.delta == 1) offsetY+=4;
                        if ((int)event.mouseWheelScroll.delta == -1) offsetY-=4;
                    }
                }

                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        cursorX = event.mouseButton.x / charWidth - offsetX;
                        cursorY = event.mouseButton.y / charHeight - offsetY;
                    }
                }

                if (event.type == sf::Event::TextEntered)
                {
                    if (event.text.unicode >= 32 && event.text.unicode <= 126)
                    {
                        lines[cursorY].insert(cursorX, 1, event.text.unicode);
                        cursorX++;
                    }
                }
            }
            ///////////////////////////////////////////////////////////////////////

            // update
            ///////////////////////////////////////////////////////////////////////
            if (offsetY > 0) offsetY = 0;
            if (offsetY < -((int)lines.size()-1)) offsetY = -((int)lines.size()-1);
            if (cursorX < 0) cursorX = 0;
            if (cursorX > lines[cursorY].length()) cursorX = lines[cursorY].length();
            if (cursorY < 0) cursorY = 0;
            if (cursorY > lines.size()-1) cursorY = lines.size()-1;
            status = filePath + "\tL:" + to_string(cursorY + 1) + "\tC:" + to_string(cursorX);
            ///////////////////////////////////////////////////////////////////////

            // drawing
            ///////////////////////////////////////////////////////////////////////

            window.clear(bgColor);

            // line number box
            rect.setSize(sf::Vector2f(charWidth * offsetX, window.getSize().y));
            rect.setPosition(0, 0);
            rect.setFillColor(lineNumBgColor);
            window.draw(rect);

            // line numbers and lines
            for (int i = -offsetY; i < window.getSize().y / charHeight - offsetY; i++)
            {
                if (i == lines.size()) break;
                txt.setFillColor(lineNumColor);
                txt.setPosition(0, charHeight * i + charHeight * offsetY);
                txt.setString(to_string(i + 1));
                window.draw(txt);
                txt.setFillColor(fgColor);
                for (int j = 0; j < lines[i].length(); j++)
                {
                    txt.setPosition(charWidth * offsetX + charWidth * j, charHeight * i + charHeight * offsetY);
                    txt.setString(lines[i][j]);
                    window.draw(txt);
                }
            }

            // cursor
            rect.setSize(sf::Vector2f(1, charHeight));
            rect.setPosition(charWidth * cursorX + charWidth * offsetX,charHeight * cursorY + charHeight * offsetY);
            rect.setFillColor(fgColor);
            window.draw(rect);

            // status box
            rect.setSize(sf::Vector2f(window.getSize().x, charHeight));
            rect.setPosition(0, window.getSize().y - charHeight);
            rect.setFillColor(sf::Color::Black);
            window.draw(rect);

            // status message
            txt.setFillColor(fgColor);
            txt.setPosition(0, window.getSize().y - charHeight);
            txt.setString(status);
            window.draw(txt);
            window.display();

            ///////////////////////////////////////////////////////////////////////
        }
    }

    return 0;
}