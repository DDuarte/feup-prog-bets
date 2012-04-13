#include "../utilities.h"
#include "../players/players.h"
#include "../bets/bets.h"

#include <iostream>
#include <fstream>

#define WINNERS_FILE_NAME "winners.txt"
#define PRIZES_MAX 13

typedef std::pair<Player*, KeyList> PlayerBets;
typedef std::vector<PlayerBets> PlayerBetsList;

struct PrizedPlayer
{
    PrizedPlayer()
    {
        Reward = 0;
    }

    Player* Player; // the player
    std::vector< std::pair<int, double> > Prizes; // prize and number of times it won
    KeyList Keys; // prized keys
    int TotalBetsCount; // number of bets (prized + non prized)
    double Reward; // total money rewarded

    void IncreaseBalance(double amount, int prize)
    {
        Player->Balance += amount;
        Reward += amount;
        Prizes[prize].second = amount / Prizes[prize].first;
    }
};

typedef std::vector<PrizedPlayer> PrizedPlayers;

struct Match
{
    Match(int numberCount, int starCount)
    {
        NumberCount = numberCount;
        StarCount = starCount;
    }

    Match()
    {
        NumberCount = 0;
        StarCount = 0;
    }

    int NumberCount;
    int StarCount;
};

struct PrizeKeyRow
{
    int PrizeId;
    std::string Hits;
    int WinnerCount;
    double Value;
};

typedef std::vector<PrizeKeyRow> PrizeKeyRows;

Key GetPrizeKeyFromFile(int& totalBets)
{
    std::ifstream file(PRIZE_KEY_FILE_NAME, std::ios::in);
    if (!file.is_open())
    {
        std::cout << "Ficheiro " << PRIZE_KEY_FILE_NAME << " nao encontrado." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string m, t, d, a, c, dd, cc, se; // each word in 1st and 2nd lines

    file >> m >> t >> d >> a >> totalBets; // line 1
    file >> c >> dd >> cc; // line 2 till first number

    std::vector<int> numbers; // line 2, numbers
    for (int i = 0; i < NUMBER_COUNT; ++i)
    {
        int n;
        file >> n;
        numbers.push_back(n);
    }

    file >> se; // line 2, number/star separator;

    std::vector<int> stars; // line 2, stars
    for (int i = 0; i < STAR_COUNT; ++i)
    {
        int s;
        file >> s;
        stars.push_back(s);
    }

    if (numbers.size() == 0 || stars.size() == 0 || file.bad())
    {
        std::cout << "Erro ao processar " << PRIZE_KEY_FILE_NAME << "." << std::endl;
        exit(EXIT_FAILURE);
    }

    file.close();

    return std::make_pair(numbers, stars);
}

int GetPrizeForHit(int numbers, int stars)
{
    const int NO_PRIZE = -1;

    switch (stars)
    {
        case 0:
            switch (numbers)
            {
                case 2: return 12;
                case 3: return 9;
                case 4: return 5;
                case 5: return 2;
                default: return NO_PRIZE;
            }
        case 1:
            switch (numbers)
            {
                case 2: return 11;
                case 3: return 8;
                case 4: return 4;
                case 5: return 1;
                default: return NO_PRIZE;
            }
        case 2:
            switch (numbers)
            {
                case 1: return 10;
                case 2: return 7;
                case 3: return 6;
                case 4: return 3;
                case 5: return 0;
                default: return NO_PRIZE;
            }
        default:
            return NO_PRIZE;
    }
}

std::string GetHitForPrize(int prize)
{
    switch (prize)
    {
		case 1:  return "5+2";
		case 2:  return "5+1";
		case 3:  return "5+0";
		case 4:  return "4+2";
		case 5:  return "4+1";
		case 6:  return "4+0";
		case 7:  return "3+2";
		case 8:  return "2+2";
		case 9:  return "3+1";
		case 10: return "3+0";
		case 11: return "1+2";
		case 12: return "2+1";
		case 13: return "2+0";
		default: return "ERR";
    }
}

Key GetKey(const std::string& key)
{
    std::stringstream ss;

    ss << key;

    std::vector<int> numbers(NUMBER_COUNT);
    for (size_t i = 0; i < numbers.size(); ++i) // line 2, numbers
    {
        int n;
        ss >> n;
        numbers[i] = n;
    }

    std::string se;
    ss >> se; // number/star separator;

    std::vector<int> stars(STAR_COUNT);
    for (size_t i = 0; i < stars.size(); ++i) // line 2, stars
    {
        int s;
        ss >> s;
        stars[i] = s;;
    }

    if (numbers.size() == 0 || stars.size() == 0 || ss.bad())
    {
        std::cout << "Erro ao processar " << BETS_FILE_NAME << "." << std::endl;
        exit(EXIT_FAILURE);
    }

    return std::make_pair(numbers, stars);
}

PlayerBetsList GetBets(const PlayerList& players)
{
    std::ifstream file(BETS_FILE_NAME);
    if (!file.is_open())
    {
        std::cout << "Erro ao processar " << BETS_FILE_NAME << "." << std::endl;
        exit(EXIT_FAILURE);
    }

    PlayerBetsList betsList;

    while (!file.eof())
    {
        std::string idStr;
        std::getline(file, idStr);

        int id = atoi(idStr.c_str());

        Player* player = SearchPlayerById(const_cast<PlayerList&>(players), id);
        if (player == NULL)
        {
            if (!file.eof())
                std::cout << "Jogador " << id << " nao encontrado." << std::endl;
            break;
        }

        KeyList keys;
        std::string keyStr;
        do 
        {
            std::getline(file, keyStr);
            if (keyStr != "")
                keys.push_back(GetKey(keyStr));
        } while (keyStr != "");

        //PlayerBets bets = std::make_pair(player, keys);
        betsList.push_back(std::make_pair(player, keys));
    }

    // Merge duplicated ids into the same user
    for (size_t i = 0; i < betsList.size(); ++i) // BUG: not working fine for all the cases
    {
        for (size_t j = i + 1; j < betsList.size(); ++j)
        {
            if (betsList[i].first == betsList[j].first)
            {
                betsList[i].second.insert(betsList[i].second.end(), betsList[j].second.begin(), betsList[j].second.end());
                betsList.erase(betsList.begin() + j);
                --j;
            }
        }
    }

    file.close();
    return betsList;
}

Match CalculateMatchingKeys(const Key& a, const Key& b)
{
    int numberCount = 0;
    int starCount = 0;

    for (int i = 0; i < NUMBER_COUNT; ++i)
    {
        for (int j = 0; j < NUMBER_COUNT; ++j)
        {
            if (a.first[i] == b.first[j])
                numberCount++;
        }
    }

    for (int i = 0; i < STAR_COUNT; ++i)
    {
        for (int j = 0; j < STAR_COUNT; ++j)
        {
            if (a.second[i] == b.second[j])
                starCount++;
        }
    }

    Match match(numberCount, starCount);
    return match;
}

int GetPrizeId(const Key& prizeKey, const Key& betKey)
{
    Match match = CalculateMatchingKeys(prizeKey, betKey);
    return GetPrizeForHit(match.NumberCount, match.StarCount);
}

// Returns the index of the player. If it does not exist, returns -1.
int Find(const PrizedPlayers& prizedPlayers, const Player* player)
{
    for (size_t i = 0; i < prizedPlayers.size(); ++i)
    {
        if (prizedPlayers[i].Player == player)
            return i;
    }

    return -1;
}

PrizedPlayers CalculatePrizedPlayers(const PlayerBetsList& bets, const Key& prizeKey)
{
    PrizedPlayers prizes;

    for (size_t i = 0; i < bets.size(); ++i)
    {
        PrizedPlayer prizedPlayer;
        prizedPlayer.Player = bets[i].first;

        prizedPlayer.Prizes = std::vector< std::pair<int, double> >(PRIZES_MAX);

        int index = Find(prizes, prizedPlayer.Player);
        if (index == -1) // new player
        {
            prizes.push_back(prizedPlayer);
            index = prizes.size() - 1;
        }
        // index now contains the index of the player that will be rewarded

        prizes[index].TotalBetsCount = bets[i].second.size();

        for (int j = 0; j < prizes[index].TotalBetsCount; ++j)
        {
            int prize = GetPrizeId(bets[i].second[j], prizeKey);
            if (prize != -1)
            {
                prizes[index].Prizes[prize].first = prizes[index].Prizes[prize].first + 1;
                prizes[index].Keys.push_back(bets[i].second[j]);
            }
        }
    }

    for (size_t i = 0; i < prizes.size(); ++i)
    {
        bool hasPrizes = false;
        for (size_t j = 0; j < prizes[i].Prizes.size(); ++j)
        {
            if (prizes[i].Prizes[j].first != 0)
                hasPrizes = true;
        }

        if (!hasPrizes)
            prizes.erase(prizes.begin() + i);
    }
        
    return prizes;
}

PrizeKeyRows CalculatePrizes(const PlayerBetsList& bets, int totalBets, const Key& prizeKey, PrizedPlayers& prizes)
{
    prizes = CalculatePrizedPlayers(bets, prizeKey);

    int existingPrizes[PRIZES_MAX] = {0};
    for (size_t i = 0; i < prizes.size(); ++i)
        for (size_t j = 0; j < prizes[i].Prizes.size(); ++j)
            existingPrizes[j] += prizes[i].Prizes[j].first;

    int countPrizedBets = 0;
    int countPrizedPrizes = 0; // no pun intended
    for (int i = 0; i < PRIZES_MAX; ++i)
    {
        if (existingPrizes[i] != 0)
        {
            ++countPrizedPrizes;
            countPrizedBets += existingPrizes[i];
        }
    }

    const double awardPerPrize = (double)totalBets / countPrizedPrizes;

    PrizeKeyRows prizeKeyRows;

    for (size_t i = 0; i < PRIZES_MAX; ++i)
    {
        PrizeKeyRow row;

        row.PrizeId = i+1; // The prize id starting at 1
        row.WinnerCount = existingPrizes[i]; // number of winners
        row.Value = row.WinnerCount != 0 ? awardPerPrize / existingPrizes[i] : 0; // money to award to a person that won this prize (i)
        row.Hits = GetHitForPrize(row.PrizeId);
        prizeKeyRows.push_back(row);

        if (existingPrizes[i] == 0)
            continue;

        for (size_t j = 0; j < prizes.size(); ++j)
            prizes[j].IncreaseBalance(prizes[j].Prizes[i].first * row.Value, i);
    }

    return prizeKeyRows;
}

void WritePrizeKey(const PrizeKeyRows& rows)
{
    std::ofstream file(PRIZE_KEY_FILE_NAME, std::ios::app | std::ios::out);
    if (!file.is_open())
    {
        std::cout << "Ficheiro " << PRIZE_KEY_FILE_NAME << " nao encontrado." << std::endl;
        exit(EXIT_FAILURE);
    }

    file << "Premio Acertos Num.Venc.      Valor     " << std::endl;
    file << "------ ------- --------- ---------------" << std::endl;

    for (size_t i = 0; i < rows.size(); ++i)
    {
        file << "  " << std::setw(2) << rows[i].PrizeId << "   ";
        file << "  " << rows[i].Hits << "   ";
        file << std::setw(9) << rows[i].WinnerCount << " ";
        file << std::setw(15) << std::setprecision(2) <<
            std::setiosflags(std::ios::fixed) <<
            rows[i].Value << std::endl;
    }

    file.close();
}

std::string WriteKeyForWinners(const Key& key, const Key& prizeKey, const std::vector< std::pair<int, double> >* prizes)
{
    std::stringstream ss;

    int numbers = 0;
    int stars = 0;
    bool prize = false;

    // Write number part of keys
    for (int i = 0; i < NUMBER_COUNT; ++i)
    {
        ss << std::setw(2) << key.first[i];
        for (int j = 0; j < NUMBER_COUNT; ++j)
        {
            if (key.first[i] == prizeKey.first[j])
            {
                ss << "*";
                numbers++;
                prize = true;
            }
        }

        if (prize)
            prize = false;
        else
            ss << " ";

        ss << " ";
    }

    ss << " - ";

    // Write star part of keys
    for (int i = 0; i < STAR_COUNT; ++i)
    {
        ss << std::setw(2) << key.second[i];
        for (int j = 0; j < STAR_COUNT; ++j)
        {
            if (key.second[i] == prizeKey.second[j])
            {
                ss << "*";
                stars++;
                prize = true;
            }
        }

        if (prize)
            prize = false;
        else
            ss << " ";
        ss << " ";
    }

    ss << " ";

    int prizeId = GetPrizeForHit(numbers, stars);

    // Write prize in "hit" form
    ss << "(" << GetHitForPrize(prizeId) << ")=";

    // Write awarded money for this key
    ss << std::setprecision(2) << std::fixed << prizes->at(prizeId).second;

    return ss.str();
}

void WriteWinners(const PrizedPlayers& prizes, const Key& prizeKey)
{
    std::ofstream file(WINNERS_FILE_NAME);
    if (!file.is_open())
    {
        std::cout << "Ficheiro " << WINNERS_FILE_NAME << " nao aberto." << std::endl;
        exit(EXIT_FAILURE);
    }

    for (std::vector<PrizedPlayer>::const_iterator plr = prizes.begin(); plr != prizes.end(); ++plr)
    {
        // write id
        file << std::setfill(ID_FILL_CHAR) << std::setw(ID_WIDTH-1) << plr->Player->Id;
        file << std::setfill(' ') << " - ";

        // write name
        file << std::left << std::setw(NAME_WIDTH) << plr->Player->Name << std::right;
        file << " - ";

        // write total bets / prized bets
        file << std::setw(5) << plr->TotalBetsCount << "/" << plr->Keys.size();
        file << " - ";

        // write total reward
        file << std::setprecision(2) << std::fixed << plr->Reward;
        file << std::endl;

        for (size_t i = 0; i < plr->Keys.size(); ++i)
            file << WriteKeyForWinners(plr->Keys[i], prizeKey, &plr->Prizes) << std::endl;
    }
}

int main()
{
    int totalBets;

     // Gets the key and the amount of money to award (from bets)
    std::cout << "1. Obtendo chave vencendora..." << std::endl;
    Key prizeKey = GetPrizeKeyFromFile(totalBets);

    // Get a list of players from players.txt
    std::cout << "2. A ler jogadores..." << std::endl;
    PlayerList players;
    if (!Read(players))
    {
        std::cout << "Erro ao processar " << PLAYERS_FILE_NAME << "." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Get bets from bets.txt
    std::cout << "3. A ler apostas..." << std::endl;
    PlayerBetsList bets = GetBets(players);

    // Calculate prizes for each prize/players based on the prize key
    std::cout << "4. A calcular premios..." << std::endl;
    PrizedPlayers prizedPlayers;
    PrizeKeyRows prizeKeyRows = CalculatePrizes(bets, totalBets, prizeKey, prizedPlayers);

    // Append to prize_key.txt the amount of money distributed/count/etc for each prize
    std::cout << "5. A escrever em prize_key..." << std::endl;
    WritePrizeKey(prizeKeyRows);

    // Create and write winners.txt
    std::cout << "6. A criar ficheiro de winners..." << std::endl;
    WriteWinners(prizedPlayers, prizeKey);

    // Re-write players.txt with updated balances
    std::cout << "7. A actualizar players..." << std::endl;
    Save(players);

	// Good bye.
	std::cout << "Fim." << std::endl;

	PauseScreen();
    return EXIT_SUCCESS;
}