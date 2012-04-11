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
            num = ReadVal<int>(ss.str());
        } while (num == 0 || num < min || num > max || Exists(vec, num));

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

    return std::make_pair(numbers, stars);
}

KeyList GenerateNKeys(int n)
{
    srand((unsigned int)time(NULL)); // seeding random number generator

    KeyList keys;
    for (int i = 0; i < n; ++i)
        keys.push_back(GenerateRandomKey());

    return keys;
}

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

bool ModifyPrizeKeyFile(double bet)
{
    // Read first line
    std::ifstream in(PRIZE_KEY_FILE_NAME);
    
    std::string m, t, d, a;
    double totalBets;

    if (in.is_open())
    {
        in >> m >> t >> d >> a >> totalBets;
        in.close();
    }
    else // if file does not exist, write to a new one
    {
        m = "Montante";
        t = "total";
        d = "de";
        a = "apostas:";
        totalBets = 0;
    }

    // Increase total bet amount
    totalBets += bet;

    // (Re)Write first line
    std::ofstream out(PRIZE_KEY_FILE_NAME);
    out << m << " " << t << " " << d << " " << a << " " << totalBets << std::endl;
    out << in.rdbuf();
    out.close();

    return true;
}

int main()
{
    std::cout << "*** Bets ***" << std::endl << std::endl;

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

    while (true)
    {
        std::cout << "******************" << std::endl;
        int id = ReadVal<int>("Numero de jogador (introduza \"-1\" para sair): ");

        if (id == -1)
            exit(EXIT_SUCCESS);

        Player* player = SearchPlayerById(players, id);
        if (player == NULL)
        {
            std::cerr << "Jogador " << id << " nao encontrado." << std::endl;
            continue;
        }

        std::cout << "Bem-vindo " << player->Name << "!" << std::endl;
        const double balance = player->Balance;
        if (balance == 0)
        {
            std::cout << "Nao podera fazer qualquer apostas com 0 euros." << std::endl;
            std::cout << "Por favor, carregue o seu cartao com pelo menos 1 euro." << std::endl;
        }
        else
            std::cout << "Saldo actual: " << balance << " euros." << std::endl;

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
            continue;
        }

        KeyList manualBets = ReadNKeys(manualBetsCount);
        KeyList autoBets = GenerateNKeys(autoBetsCount);
        
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

        // print keys
        std::cout << std::endl;
        for (size_t i = 0; i < bets.size(); ++i)
            WriteKey(bets[i], std::cout);

        std::cout << std::endl;

        if (!AppendToBetsFile(bets, player->Id))
        {
            std::cout << "Ocorreu um erro ao guardar o ficheiro bets.txt." << std::endl;
            continue;
        }

        // decrement balance and save players.txt file
        player->Balance -= cost;
        Save(players);

        // change prize_key.txt
        ModifyPrizeKeyFile(cost);
    }
        
    system("PAUSE");
}
