#include "main.h"
#include "config.h"
using namespace std;

vector<sf::Color> setLineColor(int len, sf::Color col)
{
    vector<sf::Color> v;
    for (int i = 0; i < len; i++)
    {
        v.push_back(col);
    }
    return v;
}

void highlightLine(ln& line)
{
    int pos;
    int offset;
    int kwLength = sizeof(keywords) / sizeof(string);
    int dtLength = sizeof(datatypes) / sizeof(string);

    line.color = setLineColor(line.color.size(), fgColor);

    // keywords
    for (int i = 0; i < kwLength; i++)
    {
        offset = 0;
        while (true)
        {
            pos = line.txt.find(keywords[i], offset);

            if (pos == -1) break;
            else offset = pos + 1;

            if (line.txt[pos - 1] >= '!' && line.txt[pos - 1] <= '~') continue;
            if (pos + datatypes[i].length() < line.txt.length() &&
                line.txt[pos + keywords[i].length()] != ' ' &&
                line.txt[pos + keywords[i].length()] != '(' &&
                line.txt[pos + keywords[i].length()] != ';') continue;

            for (int j = pos; j < pos + keywords[i].length(); j++)
            {
                line.color[j] = kwColor;
            }

        }
    }

    // datatypes
    for (int i = 0; i < dtLength; i++)
    {
        offset = 0;
        while (true)
        {
            pos = line.txt.find(datatypes[i], offset);

            if (pos == -1) break;
            else offset = pos + 1;

            if (line.txt[pos - 1] >= '!' && line.txt[pos - 1] <= '~') continue;
            if (pos + datatypes[i].length() < line.txt.length() &&
                line.txt[pos + datatypes[i].length()] != ' ' &&
                line.txt[pos + datatypes[i].length()] != '(' &&
                line.txt[pos + datatypes[i].length()] != ';') continue;

            for (int j = pos; j < pos + datatypes[i].length(); j++)
            {
                line.color[j] = dtColor;
            }
        }
    }

    // numbers and comments
    for (int i = 0; i < line.txt.length(); i++)
    {
        if (line.txt[i] >= '0' && line.txt[i] <= '9') line.color[i] = numColor;
        if (line.txt[i] == '\"')
        {
            pos = line.txt.find('\"', i+1);

            if (pos != -1)
            {
                for (int j = i; j <= pos; j++)
                {
                    line.color[j] = strColor;
                }
            }
        }
        if (line.txt.substr(i, 2) == "//")
        {
            for (int j = i; j < line.txt.length(); j++)
            {
                line.color[j] = commentColor;
            }
            break;
        }
    }
}