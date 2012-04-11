#include "../utilities.h"
#include "../players/players.h"
#include "../bets/bets.h"

#include <iostream>
#include <fstream>

#define WINNERS_FILE_NAME "winners.txt"

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

Key GetKey(std::string& key)
{
    std::stringstream ss;

    ss << key;

    std::vector<int> numbers;
    for (int i = 0; i < NUMBER_COUNT; ++i)
    {
        int n;
        ss >> n;
        numbers.push_back(n);
    }

    std::string se;
    ss >> se; // number/star separator;

    std::vector<int> stars; // line 2, stars
    for (int i = 0; i < STAR_COUNT; ++i)
    {
        int s;
        ss >> s;
        stars.push_back(s);
    }

    if (numbers.size() == 0 || stars.size() == 0 || ss.bad())
    {
        std::cout << "Erro ao processar " << BETS_FILE_NAME << "." << std::endl;
        exit(EXIT_FAILURE);
    }

    return std::make_pair(numbers, stars);
}

typedef std::pair<Player*, KeyList> PlayerBets;
typedef std::vector<PlayerBets> PlayerBetsList;

#define PRIZES_MAX 13

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

        PlayerBets bets = std::make_pair(player, keys);
        betsList.push_back(bets);
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

    return betsList;
}

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

    // 1º- 5 números + 2 estrelas
    // 2º- 5 números + 1 estrela
    // 3º- 5 números + 0 estrelas
    // 4º- 4 números + 2 estrelas
    // 5º- 4 números + 1 estrela
    // 6º- 4 números + 0 estrelas
    // 7º- 3 números + 2 estrelas
    // 8º- 2 números + 2 estrelas
    // 9º- 3 números + 1 estrela
    // 10º- 3 números + 0 estrelas
    // 11º- 1 número + 2 estrelas
    // 12º- 2 números + 1 estrela
    // 13º- 2 números + 0 estrelas

    const int NO_PRIZE = -1;

    switch (match.StarCount)
    {
        case 0:
            switch (match.NumberCount)
            {
                case 2: return 12;
                case 3: return 9;
                case 4: return 5;
                case 5: return 2;
                default: return NO_PRIZE;
            }
        	break;
        case 1:
            switch (match.NumberCount)
            {
                case 2: return 11;
                case 3: return 8;
                case 4: return 4;
                case 5: return 1;
                default: return NO_PRIZE;
            }
            break;
        case 2:
            switch (match.NumberCount)
            {
                case 1: return 10;
                case 2: return 7;
                case 3: return 6;
                case 4: return 3;
                case 5: return 0;
                default: return NO_PRIZE;
            }
            break;
        default:
            return NO_PRIZE;
    }
}

struct PrizedPlayer
{
    Player* Player;
    std::vector<int> Prizes; // prize and number of times it won
	KeyList Keys;

	void IncreaseBalance(double amount) { Player->Balance += amount; }
};

typedef std::vector<PrizedPlayer> PrizedPlayers;

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

        prizedPlayer.Prizes = std::vector<int>(PRIZES_MAX);

        int index = Find(prizes, prizedPlayer.Player);
        if (index == -1) // new player
        {
            prizes.push_back(prizedPlayer);
            index = prizes.size() - 1;
        }
        // index now contains the index of the player that will be rewarded

        for (size_t j = 0; j < bets[i].second.size(); ++j)
        {
            int prize = GetPrizeId(bets[i].second[j], prizeKey);
            if (prize != -1)
			{
                prizes[index].Prizes[prize] = prizes[index].Prizes[prize] + 1;
				prizes[index].Keys.push_back(bets[i].second[j]);
			}
        }
    }

    for (size_t i = 0; i < prizes.size(); ++i)
    {
        bool hasPrizes = false;
        for (size_t j = 0; j < prizes[i].Prizes.size(); ++j)
        {
            if (prizes[i].Prizes[j] != 0)
                hasPrizes = true;
        }

        if (!hasPrizes)
            prizes.erase(prizes.begin() + i);
    }
        
    return prizes;
}

std::string GetHitForPrize(int prize)
{
	switch (prize)
	{
	case 1:  return "5+2"; break;
	case 2:  return "5+1"; break;
	case 3:  return "5+0"; break;
	case 4:  return "4+2"; break;
	case 5:  return "4+1"; break;
	case 6:  return "4+0"; break;
	case 7:  return "3+2"; break;
	case 8:  return "2+2"; break;
	case 9:  return "3+1"; break;
	case 10: return "3+0"; break;
	case 11: return "1+2"; break;
	case 12: return "2+1"; break;
	case 13: return "2+0"; break;
	default: return "ERR";
	}
}

PrizeKeyRows CalculatePrizes(const PlayerBetsList& bets, int totalBets, const Key& prizeKey, PrizedPlayers& prizes)
{
    prizes = CalculatePrizedPlayers(bets, prizeKey);

    int existingPrizes[PRIZES_MAX] = {0};
    for (size_t i = 0; i < prizes.size(); ++i)
        for (size_t j = 0; j < prizes[i].Prizes.size(); ++j)
            existingPrizes[j] += prizes[i].Prizes[j];

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
            prizes[j].IncreaseBalance(prizes[j].Prizes[i] * row.Value);
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
		file << "  " << std::setw(7) << rows[i].Hits << "   ";
		file << std::setw(9) << rows[i].WinnerCount << " ";
		file << std::setw(15) << std::setprecision(2) <<
			std::setiosflags(std::ios::fixed) <<
			rows[i].Value << std::endl;
	}

	file.close();
}

void WriteWinners(const PrizedPlayers& prizes)
{
	std::ofstream file(WINNERS_FILE_NAME);
	if (!file.is_open())
	{
		std::cout << "Ficheiro " << WINNERS_FILE_NAME << " nao aberto." << std::endl;
		exit(EXIT_FAILURE);
	}
}

int main()
{
    int totalBets;

	 // Gets the key and the amount of money to award (from bets)
    Key prizeKey = GetPrizeKeyFromFile(totalBets);

	// Get a list of players from players.txt
    PlayerList players;
    if (!Read(players))
    {
        std::cout << "Erro ao processar " << PLAYERS_FILE_NAME << "." << std::endl;
        exit(EXIT_FAILURE);
    }

	// Get bets from bets.txt
    PlayerBetsList bets = GetBets(players);

	// Calculate prizes for each prize/players based on the prize key
	PrizedPlayers prizedPlayers;
    PrizeKeyRows prizeKeyRows = CalculatePrizes(bets, totalBets, prizeKey, prizedPlayers);

	// Append to prize_key.txt the amount of money distributed/count/etc for each prize
	WritePrizeKey(prizeKeyRows);

	// Create winners.txt
	WriteWinners(prizedPlayers);


    // ...

	// Re-write players.txt with updated balances
	Save(players);

    system("PAUSE");
    return EXIT_SUCCESS;
}