#include "FightArena.hpp"

FightArena::FightArena(unsigned short initialPopulationSize, unsigned short minWeight, unsigned short maxWeight):
    populationSize{initialPopulationSize}, 
    weightLowerBound{minWeight}, 
    weightUpperBound{maxWeight},
    mersenneTwister{time(0)} {  }


void FightArena::initializeSimulation() {
    std::uniform_int_distribution<int> dis(weightLowerBound, weightUpperBound);

    for(unsigned short i = 0; i < populationSize; i++) {
        Individual individual {
            { dis(mersenneTwister), dis(mersenneTwister), dis(mersenneTwister), 
              dis(mersenneTwister), dis(mersenneTwister), dis(mersenneTwister),
              dis(mersenneTwister), dis(mersenneTwister), dis(mersenneTwister) }, 0, 0
        };

        population.push_back(individual);
    }
}


std::vector<FightArena::Individual> FightArena::runSimulation() {
    for(unsigned short round = 1; round <= totalRounds; round++) {
        for(Individual& individual: population) {
            individual.losses = 0;
            individual.wins = 0;
        }

        std::cout << "round " << round << " of " << totalRounds << std::endl;
        std::vector<std::thread> simulationThreads;

        for (size_t i = 0; i < population.size(); i++) {
            for (size_t j = i + 1; j < population.size(); j++) {
                for(size_t k = 0; k < bestOf; k++)
                    simulationThreads.push_back(std::thread(fight, &population[i], &population[j]));
            }
        }

        for (auto& thread : simulationThreads) {
            thread.join();
        }

        std::sort(population.begin(), population.end(), [](const Individual& a, const Individual& b) {
            return (a.wins - a.losses) > (b.wins - b.losses);
        });

        if(totalRounds != totalRounds) {
            population.erase(population.begin() + 3, population.end());

            Individual new1, new2, new3, new4, new5, new6, new7;
            cross(&population[0], &population[1], &new1);
            cross(&population[0], &population[2], &new2);
            cross(&population[1], &population[2], &new3);

            cross(&population[0], &population[1], &population[2], &new4);

            mutate(&population[0], &new5);
            mutate(&population[1], &new6);
            mutate(&population[2], &new7);

            population.emplace_back(new1);
            population.emplace_back(new2);
            population.emplace_back(new3);
            population.emplace_back(new4);
            population.emplace_back(new5);
            population.emplace_back(new6);
            population.emplace_back(new7);
        }
    }

    return population;
}


void FightArena::fight(Individual* individual1, Individual* individual2) {
    Bot bot1{depth, HeuristicType::HYBRID, 
        HybridHeuristicsWeights{
            individual1->weights[0], individual1->weights[1], individual1->weights[2],
            individual1->weights[3], individual1->weights[4], individual1->weights[5],
            individual1->weights[6], individual1->weights[7], individual1->weights[8]}, true};

    Bot bot2{depth, HeuristicType::HYBRID,
        HybridHeuristicsWeights{
            individual2->weights[0], individual2->weights[1], individual2->weights[2],
            individual2->weights[3], individual2->weights[4], individual2->weights[5],
            individual2->weights[6], individual2->weights[7], individual2->weights[8]}, false};

    int gameState;
    int move;

    while(true) {
        move = bot1.move(AlgorithmType::ALPHA_BETA_PRUNING);
        bot2.opponentMove(move / 10, move % 10);
        gameState = bot1.gameState();

        if(gameState != -1)
            break;

        move = bot2.move(AlgorithmType::ALPHA_BETA_PRUNING);
        bot1.opponentMove(move / 10, move % 10);
        gameState = bot1.gameState();

        if(gameState != -1)
            break;
    }

    if(gameState == 2) {    // wins 1, loses 2
        individual1->wins++;
        individual2->losses++;
    }
    else if(gameState == 1) {   // wins 2, loses 1
        individual1->losses++;
        individual2->wins++;
    }
}

void FightArena::cross(Individual* individual1, Individual* individual2, Individual* newIndividual) {
    std::uniform_int_distribution<int> dis(0, 8);
    unsigned int crossPoint = dis(mersenneTwister);

    for(unsigned short i = 0; i < crossPoint; i++)
        newIndividual->weights[i] = individual1->weights[i];

    for(unsigned short i = crossPoint; i < 9; i++)
        newIndividual->weights[i] = individual2->weights[i];
}

void FightArena::cross(Individual* individual1, Individual* individual2, Individual* individual3, Individual* newIndividual) {
    std::uniform_int_distribution<int> dis(0, 2);
    int r;
    for(unsigned short i = 0; i < 9; i++) {
        r = dis(mersenneTwister);

        if(r == 0)
            newIndividual->weights[i] = individual1->weights[i];
        else if(r == 1)
            newIndividual->weights[i] = individual2->weights[i];
        else
            newIndividual->weights[i] = individual3->weights[i];
    }
}

void FightArena::mutate(Individual* individual, Individual* newIndividual) {
    std::uniform_int_distribution<int> dis1(0, 8);
    std::uniform_int_distribution<int> dis2(0, 1);

    int r;
    for(unsigned short i = 0; i < 9; i++) {
        r = dis1(mersenneTwister);

        if(r == 0) {
            r = dis2(mersenneTwister);
            if(r == 0)
                newIndividual->weights[i] = std::max(individual->weights[i] + 1, 100);
            else
                newIndividual->weights[i] = std::min(individual->weights[i] - 1, 0);
        }
        else {
            newIndividual->weights[i] = individual->weights[i];
        }
    }
}
