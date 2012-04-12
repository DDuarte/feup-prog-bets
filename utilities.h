#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string>
#include <sstream>
#include <limits>
#include <typeinfo>
#include <vector>
#include <iostream>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#endif

// Random number between inf and sup (inclusive)
inline int Rand(int inf, int sup)
{
    return rand() % (sup - inf + 1) + inf;
}

// Converts char to upper, does nothing if char is uppercased already
inline char ToUpper(char c)
{
    return ((c >= 'a' && c <= 'z') ? (c - 'a' - 'A') : c);
}

// Remove spaces at end and start of string
void TrimSpaces(std::string& string)
{
    string = string.erase(0, string.find_first_not_of(' '));
    string = string.erase(string.find_last_not_of(' ') + 1, string.size());
}

// Swaps two values
template<typename T>
void Swap(T& val1, T& val2)
{
    T temp = val1;
    val1 = val2;
    val2 = temp;
}

// Bubblesorts any type that supports '>' and '<'
template<typename T>
void Bubblesort(std::vector<T>& v)
{
    int n = v.size();
    do 
    {
        int newn = 0;
        for (size_t i = 1; i < v.size(); ++i)
        {
            if (v[i-1] > v[i])
            {
                T temp = v[i];
                v[i] = v[i-1];
                v[i-1] = temp;
                newn = i;
            }
        }
        n = newn;
    } while (n != 0);
}

// Read a value (any type) from console;
// it will ask for the value again if it is not valid
template <typename T>
T ReadVal(std::string prompt)
{
    bool success = false;
    T val;

    while (!success)
    {
        std::cout << prompt;

        std::string input;
        std::getline(std::cin, input);

        // handle strings
        if (typeid(T) == typeid(std::string)) // special case for strings:
            return *reinterpret_cast<T*>(&input); // we simply return the full string (including spaces)
                                              // retrieved by getline

        // convert string to T
        std::stringstream ss(input);

        if (!(ss >> val))
            std::cout << "Valor invalido. Por favor, tente outra vez." << std::endl;
        else
            success = true;
    }

    return val;
}

// Read a string from console (full line, including spaces)
// it will truncate the string if input is bigger than count
std::string ReadString(std::string prompt, size_t count = -1)
{
    std::cout << prompt;
    if (count != -1)
        std::cout << " (max. " << count << " caract.) ";

    std::string input;
    std::getline(std::cin, input);

    if (count != -1 && input.length() > count)
    {
        std::cout << "Aviso: numero maximo de caracteres ultrapassado." << std::endl;
        return input.substr(0, count);
    }

    return input;
}

// Returns true if value exists in vector
bool Exists(const std::vector<int>& vec, int val)
{
    for (size_t i = 0; i < vec.size(); ++i)
        if (vec[i] == val)
            return true;
    return false;
}

void ClearScreen() // multi-platform, by "Cat Plus Plus"
{
#ifdef _WIN32 // Windows: Windows API
	COORD topLeft  = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
	FillConsoleOutputAttribute(console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE, screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
	SetConsoleCursorPosition(console, topLeft);

#else // UNIX: ANSI escape codes
	std::cout << "\x1B[2J\x1B[H";
#endif
}

void PauseScreen()
{
	std::cout << "Prima qualquer tecla para continuar..." << std::endl;
	std::getchar();
}

#endif // UTILITIES_H_
