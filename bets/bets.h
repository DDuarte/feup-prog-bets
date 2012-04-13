// FEUP - MIEIC - Programação - 2011/2012
// Duarte Duarte & João Nadais

#ifndef BETS_H_
#define BETS_H_

// "Má prática": ter definição de funções em header files
// tal foi feito porque as funções e structs aqui definidas
// são partilhadas pelos vários projectos.
// Dever-se-ia ter apenas declarações aqui e produzir
// uma biblioteca (estática ou dinâmica) com as definições.

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

struct Key // estrutura básica para o programa
{
    std::vector<int> Numbers;
    std::vector<int> Stars;

    // Since size is constant (NUMBER_COUNT, STAR_COUNT), we could
    // use arrays but vectors are way easier to use and "maintain"
    // and they are as efficient as arrays
};

typedef std::vector<Key> KeyList; // a vector of keys

// para gerar um vector com valores aleatorios entre min e max de tamanho size
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

// para gerar uma chave automaticamente usando InitVectorWithRands
Key GenerateRandomKey()
{
    std::vector<int> numbers = InitVectorWithRands(NUMBER_COUNT, NUMBER_MIN, NUMBER_MAX);
    Bubblesort(numbers); // sort

    std::vector<int> stars = InitVectorWithRands(STAR_COUNT, STAR_MIN, STAR_MAX);
    if (stars[0] > stars[1]) // sort (only 2 values, redundant to use bubblesort)
        Swap(stars[0], stars[1]);

    Key key = { numbers, stars };

    return key;
}

// usando uma stream generica de output inserida pode servir quer para insercao em ficheiros quer para a stream de output da consola
void WriteKey(Key key, std::ostream& stream)
{
    for (size_t i = 0; i < key.Numbers.size(); ++i)
        stream << std::setfill(' ') << std::setw(2) << key.Numbers[i] << " ";

    stream << "- ";

    for (size_t i = 0; i < key.Stars.size(); ++i)
    {
        stream << std::setfill(' ') << std::setw(2) << key.Stars[i];
        if (i != key.Stars.size() - 1) // do not print a space in last position
            stream << " ";
    }

    stream << std::endl;
}

#endif // BETS_H_