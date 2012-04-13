#include "../bets/bets.h"

#include <fstream>
#include <iostream>
#include <time.h>
#include <iomanip>

int main()
{
    std::cout << "*** Euromilhoes: Geracao de chave ***" << std::endl;
    std::cout << "Atencao: este programa devera ser apenas executado uma unica vez." << std::endl << std::endl;

    std::fstream file(PRIZE_KEY_FILE_NAME, std::ios::app);

    if (!file.is_open())
    {
        std::cout << "Ficheiro " << PRIZE_KEY_FILE_NAME << " nao encontrado." << std::endl;
        exit(EXIT_FAILURE);
    }

    srand((unsigned int)time(NULL)); // seeding random number generator
    Key key = GenerateRandomKey();
    
    file << "Chave do concurso: ";
    WriteKey(key, file);

    std::cout << "Chave gerada: ";
    WriteKey(key, std::cout);

    PauseScreen();

    return EXIT_SUCCESS;
}