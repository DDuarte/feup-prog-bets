#ifndef BETS_H_
#define BETS_H_

#include "../utilities.h"
/*
- onde estao definidas as seguintes funcoes: 
- readval(recebe como argumento uma string para mostrar e verifica se o valor lido do teclado e do tipo inserido) 
- exists(verifica a existencia de um valor no vector)
- bubblesort(ordena o vector,definido com template para poder ordenar quer sejam ints, doubles, floats ou outro tipo de valores)
- trimspaces(elimina os espaços a mais das strings)
*/

#include <utility>
#include <vector>
#include <iomanip>
#include <iostream>

#define BETS_FILE_NAME "bets.txt"
#define PRIZE_KEY_FILE_NAME "prize_key.txt"

#define NUMBER_MIN 1
#define NUMBER_MAX 50
#define NUMBER_COUNT 5

#define STAR_MIN 1
#define STAR_MAX 11
#define STAR_COUNT 2

#define BET_COST 1.0f

typedef std::pair< std::vector<int>, std::vector<int> > Key; // 2 vectors, numbers and stars
typedef std::vector<Key> KeyList; // a vector of keys

std::vector<int> InitVectorWithRands(int size, int min, int max)
{
    std::vector<int> vec;

    for (int i = 0; i < size; ++i)
    {
        int randN;
        do { randN = Rand(min, max); } // generates a new number until it is a 
        while (Exists(vec, randN));    // number different from all the others

        vec.push_back(randN);
    }

    return vec;
}

Key GenerateRandomKey()
{
    std::vector<int> numbers = InitVectorWithRands(NUMBER_COUNT, NUMBER_MIN, NUMBER_MAX);
    Bubblesort(numbers); // sort

    std::vector<int> stars = InitVectorWithRands(STAR_COUNT, STAR_MIN, STAR_MAX);
    if (stars[0] > stars[1]) // sort (only 2 values, redundant to use bubblesort)
        Swap(stars[0], stars[1]);

    return std::make_pair(numbers, stars);
}

void WriteKey(Key key, std::ostream& stream)
{
    for (size_t i = 0; i < key.first.size(); ++i)
        stream << std::setfill(' ') << std::setw(2) << key.first[i] << " ";

    stream << "- ";

    for (size_t i = 0; i < key.second.size(); ++i)
    {
        stream << std::setfill(' ') << std::setw(2) << key.second[i];
        if (i != key.second.size() - 1) // do not print a space in last position
            stream << " ";
    }

    stream << std::endl;
}

#endif // BETS_H_