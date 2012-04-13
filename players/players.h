// FEUP - MIEIC - Programação - 2011/2012
// Duarte Duarte & João Nadais

#ifndef PLAYERS_H_
#define PLAYERS_H_

// "Má prática": ter definição de funções em header files
// tal foi feito porque as funções e structs aqui definidas
// são partilhadas pelos vários projectos.
// Dever-se-ia ter apenas declarações aqui e produzir
// uma biblioteca (estática ou dinâmica) com as definições.

#include "../utilities.h"

#include <vector>
#include <utility>
#include <string>
#include <fstream>

#include <direct.h>

#define PLAYERS_FILE_NAME "players.txt"
#define TEMP_FILE_NAME "temp_players.txt"

#define ID_WIDTH 8
#define ID_FILL_CHAR '0'
#define NAME_WIDTH 21
#define BALANCE_WIDTH 13
#define BALANCE_PRECISION 2

struct Player
{
    int Id;
    std::string Name;
    double Balance;
};

typedef std::pair<std::vector<Player>, int> PlayerList; // second is the max id

Player* SearchPlayerById(PlayerList& players, int id)
{
    if (players.first.size() == 0)
        return NULL; // empty vector

    int bottom = 0;
    int top = players.first.size() - 1;

    while (bottom <= top)
    {
        int middle = (top + bottom) / 2;

        if (players.first[middle].Id == id)
            return &players.first[middle];
        else
            if (players.first[middle].Id < id)
                bottom = middle + 1;
            else
                top = middle - 1;
    }

    return NULL; // not found
}

int RecreatePlayer(PlayerList& players, int id, const std::string& name, double balance)
{
    Player player;

    player.Id = id;
    player.Name = name;
    player.Balance = balance;

    players.first.push_back(player);

    return player.Id;
}

void Save(const PlayerList& players)
{
    std::ofstream file;
    file.open(TEMP_FILE_NAME);

    char prevFill = file.fill(ID_FILL_CHAR);
    std::streamsize prevWidth = file.width(ID_WIDTH-1);
    file << players.second << std::endl; // last id at the top
    file.fill(prevFill);
    file.width(prevWidth);

    for (size_t i = 0; i < players.first.size(); ++i)
    {
        // Id
        prevFill = file.fill(ID_FILL_CHAR);
        prevWidth = file.width(ID_WIDTH-1);
        file << players.first[i].Id << " ";
        file.fill(prevFill);
        file.width(prevWidth);

        // Name
        prevWidth = file.width(NAME_WIDTH-1);
        file << std::left << players.first[i].Name << " ";
        file.width(prevWidth);

        // Balance
        prevWidth = file.width(BALANCE_WIDTH);
        std::streamsize prevPrec = file.precision(BALANCE_PRECISION);
        file.setf(std::ios::fixed, std::ios::floatfield);
        file << std::right << players.first[i].Balance;
        file.unsetf(std::ios::fixed|std::ios::floatfield);
        file.precision(prevPrec);
        file.width(prevWidth);

        file << std::endl;
    }

    file.close();

    remove(PLAYERS_FILE_NAME);
    rename(TEMP_FILE_NAME, PLAYERS_FILE_NAME);
}

bool Read(PlayerList& players)
{
    std::ifstream file;
    file.open(PLAYERS_FILE_NAME);
    if (!file.is_open())
        return false;

    int maxId;
    file >> maxId;

    while (!file.eof())
    {
        int id;
        char nameCh[NAME_WIDTH+1];
        double balance;

        file >> id;
        file.get(nameCh, NAME_WIDTH+1);
        file >> balance;

        if (file.fail())
        {
            if (file.eof())
            {
                file.close();
                players.second = maxId;
                return true;
            }
            return false;
        }

        std::string name(nameCh);
        TrimSpaces(name);

        RecreatePlayer(players, id, name, balance);
    }

    file.close();

    return true;
}

#endif // PLAYERS_H_