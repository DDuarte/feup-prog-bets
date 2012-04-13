// FEUP - MIEIC - Programação - 2011/2012
// Duarte Duarte & João Nadais


#include "../players/players.h"
#include "../utilities.h"
#include "bets.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <utility>
#include <iomanip>
#include <direct.h>

// criacao de vector atraves da insercao de valores pelo teclado
std::vector<int> InitVectorWithUserInput(int size, int min, int max, char* name)
{
    std::vector<int> vec;

    for (int i = 1; i <= size; ++i)
    {
        std::stringstream ss;
        ss << "[" << name << " " << i << "/" << size << "]: ";

        int num = 0;
        do 
        {
            num = ReadVal<int>(ss.str()); // ss é convertida em string para ser mostrada
        } while (/*num == 0 ||*/ num < min || num > max || Exists(vec, num));
        // condição de terminação:
        // número tem de estar dentro dos nossos limites,
        // nem poderá já existir no vector

        vec.push_back(num);
    }

    return vec;
}

Key ReadKey()
{
    std::vector<int> numbers = InitVectorWithUserInput(NUMBER_COUNT, NUMBER_MIN, NUMBER_MAX, "Numero");
    Bubblesort(numbers); // sort

    std::vector<int> stars = InitVectorWithUserInput(STAR_COUNT, STAR_MIN, STAR_MAX, "Estrela");
    if (stars[0] > stars[1]) // sort (only 2 values, redundant to use bubblesort)
        Swap(stars[0], stars[1]);

    Key key = { numbers, stars };

    return key;
}

// Gerar um numero n de chaves aleatorias 
KeyList GenerateNKeys(int n)
{
    srand((unsigned int)time(NULL)); // seeding random number generator

    KeyList keys;
    for (int i = 0; i < n; ++i)
        keys.push_back(GenerateRandomKey());

    return keys;
}

// Função para gerar um numero n de chaves definido por n
KeyList ReadNKeys(int n)
{
    KeyList keys;
    for (int i = 1; i <= n; ++i)
    {
        std::cout << "Aposta " << i << "/" << n << ":" << std::endl;
        keys.push_back(ReadKey());
    }

    return keys;
}

// actualiza o ficheiro bets com as apostas feitas (contidas em chaves) correspondentes a cada jogador (identificado pelo id)
bool AppendToBetsFile(const KeyList& keys, int playerId)
{
    std::ofstream file(BETS_FILE_NAME, std::ios::out | std::ios::app);
    if (!file.is_open())
        return false;

    file << std::setfill(ID_FILL_CHAR) << std::setw(ID_WIDTH) << playerId << std::endl;

    for (size_t i = 0; i < keys.size(); ++i)
        WriteKey(keys[i], file);

    file << std::endl;

    file.close();

    return true;
}

// actualiza o numero de apostas do ficheiro prize_key.txt
bool ModifyPrizeKeyFile(double bet)
{
    // Le a primeira linha
    std::ifstream in(PRIZE_KEY_FILE_NAME);
    
    double totalBets = 0;

    if (in.is_open())
    {
        in.ignore(INT_MAX, ':'); // lê e ignora até ao caracter ':'
        in >> totalBets;
        in.close();
    }

    // Aumenta o numero total de apostas
    totalBets += bet;

    // (Re)Escreve a primeira linha
    std::ofstream out(PRIZE_KEY_FILE_NAME);
    out << "Montante total de apostas: " << totalBets << std::endl;
    out << in.rdbuf(); // Escreve o resto do ficheiro, se existir
    out.close();

    return true;
}

int main()
{
    PlayerList players;
    if (!Read(players))
    {
        std::cerr << "Ficheiro de jogadores nao encontrado, a sair..." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (players.first.size() == 0)
    {
        std::cerr << "Nenhum jogador registado, a sair..." << std::endl;
        exit(EXIT_FAILURE);
    }

    // nota: não é possível cancelar uma aposta a meio (TODO?)

    while (true) // acaba quando o jogador introduzir o numero 0 no id do jogador
    {
        std::cout << "*** Euromilhoes: Registo de Apostas ***" << std::endl << std::endl;

        std::cout << "******************" << std::endl;
        std::cout << "Introduza 0 no numero de jogador para sair." << std::endl;

        int id = ReadVal<int>("Numero de jogador: ");
        if (id <= 0)
            break;

        Player* player = SearchPlayerById(players, id);
        if (player == NULL) // nao encontrou o jogador
        {
            std::cerr << "Jogador " << id << " nao encontrado." << std::endl;
            PauseScreen();
            ClearScreen();
            continue;
        }
        // encontrou e devolveu o apontador

        std::cout << "Bem-vindo " << player->Name << "!" << std::endl;

        //retira-se o saldo e o nome do jogador
        const double balance = player->Balance;
        if (balance < BET_COST) // verifica se tem saldo para pelo menos uma aposta
        {
            std::cout << "Nao podera fazer qualquer apostas com " << std::fixed << std::setprecision(2) << balance << " euros." << std::endl;
            std::cout << "Por favor, carregue o seu cartao com pelo menos 1 euro." << std::endl;
            PauseScreen();
            ClearScreen();
            continue;
        }
        else
            std::cout << "Saldo actual: " << std::fixed << std::setprecision(2) << balance << " euros." << std::endl;

        unsigned int manualBetsCount = 0;
        unsigned int autoBetsCount = 0;
        do 
        {
            manualBetsCount = ReadVal<unsigned int>("Introduza o numero de apostas manuais: ");
            autoBetsCount = ReadVal<unsigned int>("Introduza o numero de apostas automaticas: ");
        } while (manualBetsCount <= 0 && autoBetsCount <= 0);

        double cost = BET_COST * (manualBetsCount + autoBetsCount);
        std::cout << "Custo total: " << cost << " euros." << std::endl;

        if (cost > balance)
        {
            std::cout << "O saldo nao e suficiente para apostar." << std::endl;
            std::cout << "Por favor, carregue o seu cartao." << std::endl;
            PauseScreen();
            ClearScreen();
            continue;
        }

        KeyList manualBets = ReadNKeys(manualBetsCount);
        KeyList autoBets = GenerateNKeys(autoBetsCount);
        
        // gera-se a seguir um vector conjunto de todas as chaves para ser mostrado
        // juntou-se manualBets e autoBets a um única vector: bets
        KeyList bets;
        if (manualBetsCount > 0)
        {
            bets.insert(bets.end(), manualBets.begin(), manualBets.end());
            manualBets.clear();
        }
        if (autoBetsCount > 0)
        {
            bets.insert(bets.end(), autoBets.begin(), autoBets.end());
            autoBets.clear();
        }

        // mostra todas as chaves
        std::cout << std::endl;
        for (size_t i = 0; i < bets.size(); ++i)
            WriteKey(bets[i], std::cout);

        std::cout << std::endl;

        if (!AppendToBetsFile(bets, player->Id))
        {
            std::cout << "Ocorreu um erro ao guardar o ficheiro bets.txt." << std::endl;
            PauseScreen();
            ClearScreen();
            continue;
        }

        // actualiza saldo do jogador depois de jogar as apostas
        player->Balance -= cost;
        Save(players);

        // altera prize_key.txt
        if (!ModifyPrizeKeyFile(cost))
        {
            // ocorreu um erro
            std::cout << "Erro ao escrever " << BETS_FILE_NAME << ". A terminar." << std::endl;
            return EXIT_FAILURE;
        }

        PauseScreen();
        ClearScreen();
    }
        
    PauseScreen();
    return EXIT_SUCCESS; // se tudo for bem escrito o programa acaba
}
