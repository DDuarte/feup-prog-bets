// FEUP - MIEIC - Programação - 2011/2012
// Duarte Duarte & João Nadais

#include "players.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <limits>

enum Task
{
    RegisterPlayer = 'R',
    CheckBalance = 'C',
    IncreaseBalance = 'A',
    RemovePlayer = 'E',
    ListPlayers = 'L',
    Exit = 'S'
};

int CreatePlayer(PlayerList& players, const std::string& name, double balance)
{
    Player player;
    players.second += 1; // increase max id

    player.Id = players.second;
    player.Name = name;
    player.Balance = balance;

    players.first.push_back(player);

    return player.Id;
}

bool DeletePlayer(PlayerList& players, int id)
{
    for (size_t i = 0; i < players.first.size(); ++i)
    {
        if (players.first[i].Id == id)
        {
            players.first.erase(players.first.begin() + i);
            return true; // deleted
        }
    }

    return false; // not deleted
}

bool IncreasePlayerBalance(Player* player, double amount)
{
    if (amount <= 0)
        return false; // Invalid value

    player->Balance += amount;
    return true;
}

void ExitAndSave(const PlayerList& players)
{
    Save(players);
    exit(EXIT_SUCCESS);
}

int main()
{
    PlayerList players;
    if (!Read(players))
        Save(players); // creates an empty file

    while (true)
    {
        std::cout << "*** Euromilhoes - Gestao de Jogadores ***" << std::endl << std::endl;

        std::cout << "Introduza a letra respectiva:" << std::endl;
        std::cout << "R - Registar um jogador." << std::endl;
        std::cout << "C - Consultar o saldo de um jogador." << std::endl;
        std::cout << "A - Carregar saldo de um jogador." << std::endl;
        std::cout << "E - Eliminar um jogador." << std::endl;
        std::cout << "L - Listar jogadores." << std::endl;
        std::cout << "S - Sair." << std::endl;

        char task = ReadVal<char>("");

        std::cout << std::endl;

        switch(ToUpper(task))
        {
            case RegisterPlayer:
            {
                std::cout << "* Registar jogador" << std::endl;

                std::string name = ReadString("Nome: ", NAME_WIDTH - 1);
                double amount = ReadVal<double>("Saldo inicial: ");

                int id = CreatePlayer(players, name, amount);
                std::cout << "Sucesso, atribuido numero " << id << "." << std::endl;
                std::cout << std::endl;

                break;
            }
            case CheckBalance:
            {
                std::cout << "* Consultar saldo" << std::endl;

                int id = ReadVal<int>("Numero do jogador: ");

                if (Player* player = SearchPlayerById(players, id))
                    std::cout << "Saldo de " << player->Name << ": " << std::fixed << std::setprecision(2) << player->Balance << "E." << std::endl;
                else
                    std::cout << "Jogador " << id << " nao encontrado." << std::endl;
                std::cout << std::endl;

                break;
            }
            case IncreaseBalance:
            {
                std::cout << "* Aumentar saldo" << std::endl;

                int id = ReadVal<int>("Numero do jogador: ");

                if (Player* player = SearchPlayerById(players, id))
                {
                    double amount = ReadVal<double>("Quantidade: ");

                    double oldAmount = player->Balance;

                    if (IncreasePlayerBalance(player, amount))
                    {
                        std::cout << player->Name << ": anterior: " << oldAmount << "E, aumento: ";
                        std::cout << amount << "E, actual: " << player->Balance << "E." << std::endl;
                    }
                    else
                        std::cout << "Valor invalido." << std::endl;
                }
                else
                    std::cout << "Jogador " << id << " nao encontrado." << std::endl;
                std::cout << std::endl;

                break;
            }
            case RemovePlayer:
            {
                std::cout << "* Remove jogador" << std::endl;

                int id = ReadVal<int>("Numero do jogador: ");
                if (Player* player = SearchPlayerById(players, id))
                {
                    std::string name = player->Name;
                    DeletePlayer(players, id);
                    std::cout << "Jogador " << name << " eliminado." << std::endl;
                }
                else
                    std::cout << "Jogador " << id << " nao encontrado." << std::endl;
                std::cout << std::endl;

                break;
            }
            case ListPlayers:
            {
                std::cout << "* Lista de jogadores" << std::endl;

                if (players.first.size() == 0)
                    std::cout << "Lista de jogadores vazia." << std::endl;
                else
                    for (size_t i = 0; i < players.first.size(); ++i)
                    {
                        std::cout << "|";

                        char prevFill = std::cout.fill(ID_FILL_CHAR);
                        std::streamsize prevWidth = std::cout.width(ID_WIDTH);
                        std::cout << std::right << players.first[i].Id;
                        std::cout.fill(prevFill);
                        std::cout.width(prevWidth);

                        prevWidth = std::cout.width(NAME_WIDTH);
                        std::cout << std::left << "| " << players.first[i].Name << " |" << std::endl;
                        std::cout.width(prevWidth);
                    }

                std::cout << std::endl;

                break;
            }
            case Exit:
            {
                char answer = ReadVal<char>("Tem a certeza que deseja sair (S/N)? ");

                if (ToUpper(answer) == 'S')
                    ExitAndSave(players);
                else if (ToUpper(answer) == 'N')
                    break;
                else
                    std::cout << std::endl; // Fall to default
            }
            default:
            {
                std::cout << "Opcao invalida. Por favor, tente outra vez." << std::endl;
                break;
            }
        }

        PauseScreen();
        ClearScreen();
    }

    PauseScreen();
}
