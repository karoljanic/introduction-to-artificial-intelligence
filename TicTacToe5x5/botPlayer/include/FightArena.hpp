#ifndef FIGHT_ARENA_H
#define FIGHT_ARENA_H

#include <random>
#include <thread>
#include <mutex>

#include "Bot.hpp"

class FightArena {
    static constexpr unsigned short depth = 5;
    static constexpr unsigned short bestOf = 5;
    static constexpr unsigned short totalRounds = 100;

public:
    struct Individual {
        int weights[9];
        unsigned short wins;
        unsigned short losses;
    };

private:
    unsigned short populationSize;
    unsigned short weightLowerBound;
    unsigned short weightUpperBound;
    std::mt19937 mersenneTwister;

    std::vector<Individual> population;

public:
    FightArena(unsigned short initialPopulationSize, unsigned short minWeight, unsigned short maxWeight);

    void initializeSimulation();
    std::vector<Individual> runSimulation();

private:
    static void fight(Individual* individual1, Individual* individual2);
    void cross(Individual* individual1, Individual* individual2, Individual* newIndividual);
    void cross(Individual* individual1, Individual* individual2, Individual* individual3, Individual* newIndividual);
    void mutate(Individual* individual, Individual* newIndividual);
};

#endif // FIGHT_ARENA_H