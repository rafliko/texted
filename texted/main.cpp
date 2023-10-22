#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "config.h"
using namespace std;

string programPath = "";
string filePath = "";
string status = "";

vector<string> lines;

int spacing = 1;
int charHeight = fontSize;
int charWidth = fontSize / 2 + spacing;

sf::Vector2i offset(7, 0);

sf::Vector2i cursor(0, 0);

sf::Vector2i selectFrom(0, 0);
sf::Vector2i selectTo(0, 0);
bool selecting = false;

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");
    if (argc > 1)
    {
        programPath = argv[0];
        programPath = programPath.substr(0, programPath.find_last_of('\\') + 1);
        filePath = argv[1];
        argc--;
    }
    else return 1;

    ifstream file(filePath);
    if (!file.fail())
    {
        string line;
        while (getline(file, line))
        {
            lines.push_back(line);
        }
        file.close();
    }
    else return 1;

    sf::Font font;
    font.loadFromFile(programPath+"Fonts/Hack/Hack-Regular.ttf");

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
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::Resized)
            {
                // update the view to the new size of the window
                sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.scancode == sf::Keyboard::Scan::LShift)
                {
                    selectFrom = cursor;
                    selectTo = cursor;
                    selecting = true;
                }

                if (event.key.scancode == sf::Keyboard::Scan::Left) cursor.x--;
                if (event.key.scancode == sf::Keyboard::Scan::Right) cursor.x++;
                if (event.key.scancode == sf::Keyboard::Scan::Up) cursor.y--;
                if (event.key.scancode == sf::Keyboard::Scan::Down) cursor.y++;

                if (event.key.scancode >= 86 && event.key.scancode <= 89)
                {
                    if (selecting) selectTo = cursor;
                    else
                    {
                        selectFrom = sf::Vector2i(0, 0);
                        selectTo = sf::Vector2i(0,0);
                        selecting = false;
                    }
                }

                if (event.key.scancode == sf::Keyboard::Scan::Home) { cursor.x = 0; if (selecting) selectTo = cursor; }
                if (event.key.scancode == sf::Keyboard::Scan::End) { cursor.x = lines[cursor.y].length(); if (selecting) selectTo = cursor; }

                if (event.key.scancode == sf::Keyboard::Scan::PageUp) offset.y += window.getSize().y / charHeight - 2;
                if (event.key.scancode == sf::Keyboard::Scan::PageDown) offset.y -= window.getSize().y / charHeight - 2;

                if (event.key.scancode == sf::Keyboard::Scan::Delete && cursor.x < lines[cursor.y].length())
                {
                    lines[cursor.y].erase(cursor.x, 1);
                }
                else if (event.key.scancode == sf::Keyboard::Scan::Delete && cursor.x == lines[cursor.y].length() && cursor.y<lines.size()-1)
                {
                    lines[cursor.y] += lines[cursor.y + 1];
                    lines.erase(lines.begin() + cursor.y + 1);
                }

                if (event.key.scancode == sf::Keyboard::Scan::Backspace && cursor.x > 0)
                {
                    lines[cursor.y].erase(cursor.x - 1, 1);
                    cursor.x--;
                }
                else if (event.key.scancode == sf::Keyboard::Scan::Backspace && cursor.x == 0 && cursor.y > 0)
                {
                    cursor.y--;
                    cursor.x = lines[cursor.y].length();
                    lines[cursor.y] += lines[cursor.y+1];
                    lines.erase(lines.begin() + cursor.y+1);
                }

                if (event.key.scancode == sf::Keyboard::Scan::Enter)
                {
                    lines.insert(lines.begin()+cursor.y+1,lines[cursor.y].substr(cursor.x));
                    lines[cursor.y] = lines[cursor.y].substr(0, cursor.x);
                    cursor.x = 0;
                    cursor.y++;
                }
            }

            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.scancode == sf::Keyboard::Scan::LShift) selecting = false;
            }

            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                {
                    if ((int)event.mouseWheelScroll.delta == 1) offset.y+=scrollSpeed;
                    if ((int)event.mouseWheelScroll.delta == -1) offset.y-=scrollSpeed;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    cursor.x = event.mouseButton.x / charWidth - offset.x;
                    cursor.y = event.mouseButton.y / charHeight - offset.y;
                    selectFrom = cursor;
                    selectTo = cursor;
                    selecting = true;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    selecting = false;
                }
            }

            if (event.type == sf::Event::MouseMoved)
            {
                if (selecting)
                {
                    cursor.x = event.mouseMove.x / charWidth - offset.x;
                    cursor.y = event.mouseMove.y / charHeight - offset.y;
                    selectTo = cursor;
                }
            }

            if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode >= 32 && event.text.unicode <= 126)
                {
                    lines[cursor.y].insert(cursor.x, 1, event.text.unicode);
                    cursor.x++;
                }
            }
        }
        ///////////////////////////////////////////////////////////////////////

        // update
        ///////////////////////////////////////////////////////////////////////
        if (offset.y > 0) offset.y = 0;
        if (offset.y < -((int)lines.size()-1)) offset.y = -((int)lines.size()-1);

        if (cursor.x < 0) cursor.x = 0;
        if (cursor.x > lines[cursor.y].length()) cursor.x = lines[cursor.y].length();
        if (cursor.y < 0) cursor.y = 0;
        if (cursor.y > lines.size()-1) cursor.y = lines.size()-1;

        if (selectFrom.x < 0) selectFrom.x = 0;
        if (selectFrom.x > lines[selectFrom.y].length()) selectFrom.x = lines[selectFrom.y].length();
        if (selectFrom.y < 0) selectFrom.y = 0;
        if (selectFrom.y > lines.size() - 1) selectFrom.y = lines.size() - 1;

        if (selectTo.x < 0) selectTo.x = 0;
        if (selectTo.x > lines[selectTo.y].length()) selectTo.x = lines[selectTo.y].length();
        if (selectTo.y < 0) selectTo.y = 0;
        if (selectTo.y > lines.size() - 1) selectTo.y = lines.size() - 1;

        status = filePath + "\tL:" + to_string(cursor.y + 1) + "\tC:" + to_string(cursor.x);
        ///////////////////////////////////////////////////////////////////////

        // drawing
        ///////////////////////////////////////////////////////////////////////

        window.clear(bgColor);

        // line number box
        rect.setSize(sf::Vector2f(charWidth * offset.x, window.getSize().y));
        rect.setPosition(0, 0);
        rect.setFillColor(lineNumBgColor);
        window.draw(rect);

        // selection box
        sf::Vector2i from, to;
        if(selectFrom.y<selectTo.y)
        {
            from = selectFrom;
            to = selectTo;
        }
        else
        {
            from = selectTo;
            to = selectFrom;
        }

        if (from.y == to.y)
        {
            rect.setSize(sf::Vector2f(charWidth * (to.x - from.x), charHeight));
            rect.setPosition(charWidth * from.x + charWidth * offset.x, charHeight * from.y + charHeight * offset.y);
            rect.setFillColor(sf::Color::Blue);
            window.draw(rect);
        }
        else
        {
            for (int i = from.y; i <= to.y; i++)
            {
                if (i == from.y)
                {
                    rect.setSize(sf::Vector2f(charWidth * lines[i].length() - charWidth * from.x, charHeight));
                    rect.setPosition(charWidth * offset.x + charWidth * from.x, charHeight* i + charHeight * offset.y);
                }
                else if (i == to.y)
                {
                    rect.setSize(sf::Vector2f(charWidth * to.x, charHeight));
                    rect.setPosition(charWidth * offset.x, charHeight * i + charHeight * offset.y);
                }
                else
                {
                    rect.setSize(sf::Vector2f(charWidth* lines[i].length(), charHeight));
                    rect.setPosition(charWidth* offset.x, charHeight* i + charHeight * offset.y);
                }
                rect.setFillColor(sf::Color::Blue);
                window.draw(rect);
            }
        }

        // line numbers and lines
        for (int i = -offset.y; i < window.getSize().y / charHeight - offset.y; i++) // only draw visible lines
        {
            if (i == lines.size()) break;
            txt.setFillColor(lineNumColor);
            txt.setPosition(0, charHeight * i + charHeight * offset.y);
            txt.setString(to_string(i + 1));
            window.draw(txt);
            txt.setFillColor(fgColor);
            for (int j = 0; j < lines[i].length(); j++)
            {
                txt.setPosition(charWidth * offset.x + charWidth * j, charHeight * i + charHeight * offset.y);
                txt.setString(lines[i][j]);
                window.draw(txt);
            }
        }

        // cursor
        rect.setSize(sf::Vector2f(1, charHeight));
        rect.setPosition(charWidth * cursor.x + charWidth * offset.x,charHeight * cursor.y + charHeight * offset.y);
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

    return 0;
}