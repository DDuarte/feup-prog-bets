#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string>
#include <sstream>
#include <limits>
#include <typeinfo>
#include <vector>
#include <iostream>

// random number between inf and sup (inclusive)
inline int Rand(int inf, int sup)
{
    return rand() % (sup - inf + 1) + inf;
}

// converts char to upper, does nothing if char is uppercased already
inline char ToUpper(char c)
{
    return ((c >= 'a' && c <= 'z') ? (c - 'a' - 'A') : c);
}

void TrimSpaces(std::string& string)
{
    string = string.erase(0, string.find_first_not_of(' '));
    string = string.erase(string.find_last_not_of(' ') + 1, string.size());
}

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

// returns true if value exists in vector
bool Exists(const std::vector<int>& vec, int val)
{
    for (size_t i = 0; i < vec.size(); ++i)
        if (vec[i] == val)
            return true;
    return false;
}

#endif // UTILITIES_H_
