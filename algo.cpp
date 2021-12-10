/**
 * @file algo.cpp
 * @author Rebeka Cernianska (xcerni13), Peter Rucek (xrucek00)
 * @brief Algo for ...
 * @date 2021-12-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <iostream>
#include <random>
#include <cmath>
#include <stdlib.h>
#include <chrono>


typedef std::chrono::high_resolution_clock myclock;
using std::cout;
using std::endl;

double TruncateNumber(double In, unsigned int Digits)
{
    double f = pow(10, Digits);
    return ((int)(In*f))/f;
}

class NormalDistribution {
    myclock::time_point beginning = myclock::now();
    std::default_random_engine generator; //std::mt19937
    std::normal_distribution<double> distribution;
    double min;
    double max;
public:
    NormalDistribution(double min, double max):
        distribution((min + max) / 2, (max - min) / 4), min(min), max(max)
    {
        myclock::duration d = myclock::now() - beginning;
        unsigned seed = d.count();
        generator.seed(seed);
    }

    double operator ()() {
        while (true) {
            double number = this->distribution(generator);
            if (number >= this->min && number <= this->max)
                return TruncateNumber(number,3);
        }
    }
};

class ExponentialDistribution {
    myclock::time_point beginning = myclock::now();
    std::default_random_engine generator; //std::mt19937
    std::exponential_distribution<> distribution;
    double min;
    double max;
public:
    ExponentialDistribution(double min, double max):
        distribution(1 /((max - min) / (-std::log(0.05)))), min(min), max(max)
    {
        myclock::duration d = myclock::now() - beginning;
        unsigned seed = d.count();
        generator.seed(seed);
    }

    double operator ()() {
        while (true) {
            double number = this->distribution(generator) + min; // shifted
            if (number >= this->min && number <= this->max)
                return TruncateNumber(number,3);
        }
    }
};

enum Size
{
    SMALL = 3,
    MEDIUM = 6,
    BIG = 9,
    EXTRA = 12
};

class Factory
{
public:
    
    int M;  // Number of machines
    int Td; // Time factory is working per day, second per day
    Factory(Size m = Size::SMALL, int td = 60*60*24)
    {
        M = m;
        Td = td;
    }

    long SimulateDay()
    {
        NormalDistribution CTwb(6.0902, 6.4609);        // Cycle Time Wire Bond
        NormalDistribution CTda(2.8072, 2.9460);        // Cycle Time Die Attach
        NormalDistribution CTpc(0.9882, 1.0498);        // Cycle Time Pre-Cap Inspection
        ExponentialDistribution DDda(2141, 4391);       // Downtime Duration Die Attach
        ExponentialDistribution DDwb(1364, 2797);       // Downtime Duration Wire Bond
        ExponentialDistribution DFda(1066*60, 2187*60); // Downtime Frequency Die Attach
        ExponentialDistribution DFwb(1421*60, 2193*60); // Downtime Frequency Wire Bond
        ExponentialDistribution STda(2957, 6063);       // Setup Time Die Attach
        ExponentialDistribution STwb (1324, 2714);      // Setup Time Wire Bond

        int Q[] = {2200,3080,11264}; // Batch size 
        srand(time(NULL));
        int Qcur = Q[0];

        int CToc = 7200; // 7187.572 , Cycle Time Oven Cure

        Qcur = Q[rand() % 3];
        long Ttotal= ( (Td-STda())*((86400-DDda())/86400) + Td +(Td-STwb())*((86400-DDwb())/86400)+Td ) * M ;
        long Tunit = CToc + 387.1239*(CTda()-2.8766)/0.0694 + 1008.0011*(CTwb()-6.2756)/0.1854 +
            157.9690*(CTpc()-1.019)/0.0308 + 9.5167*(DDda()-3266)/1125 + 3.9325*(DDwb()-2080.5)/716.5 +
            2.4477*(DFda()-1626.5)/560.5 - 0.1807*(DFwb()-1807)/386 - 2.6148*(STda()-4510)/1553 +
            24.3713*(STwb()-2019)/695 + 10.1729*Qcur;

        long Om = Ttotal * Qcur / Tunit;

        return Om;
    }
};

long GenerateFactories(Size size, int count)
{
    long daily = 0;
    for (int i = 0 ; i < count ; i++)
    {
        Factory *factory = new Factory(size);
        int Om = factory->SimulateDay();
        daily += Om;
    }
    return daily; // number of semiconductors per day from @param count facotries of  @param size 
}


int main()
{
    // The case study was carried out in a semiconductor assembly line
    // which consisted of three die attach machines, four
    // oven cure machines, nine wire bond machines,
    // and three pre-cap inspection machines

    #define TOTAL_2021 1140000000000
    long long yearly = 0;
    long factoriesCount = 38000;
    while (true)
    {
        yearly = 0;
        for (int day = 0 ; day < 365 ; day++)
        {
            long dialyCount = GenerateFactories(Size::SMALL, factoriesCount);
            yearly += dialyCount;
        }
        cout << "1.14 trilion vs " << yearly << " current factories count:"<< factoriesCount << endl;
        if (yearly > TOTAL_2021) // 1.14 trilion number of semiconductors per year 2021
        {
            break;
        }
        else
        {
            factoriesCount += 500;
        }
        
    }
    cout << "1.14 trilion polovodicov vyrobi "<< factoriesCount << " modelovych fabrik za rok" << endl;
    

    long totalByNYears = TOTAL_2021; 
    factoriesCount = 2100;
    for(int year = 1 ; year < 6 ; year++)
    {
        long rise = (totalByNYears / 100) * 6.25;
        long yearly = 0;
        while (true)
        {
            yearly = 0;
            for (int day = 0 ; day < 365 ; day++)
            {
                long dialyCount = GenerateFactories(Size::SMALL, factoriesCount);
                yearly += dialyCount;
            }
            cout << "6.25% = "<< rise << " vs " << yearly << " current factories count: " << factoriesCount << endl;
            if (yearly > rise) // 6.25 % from previous year
            {
                break;
            }
            else
            {
                factoriesCount += 10;
            }
        }
        cout << "po "<< year <<" rokoch, pocet fabrik potrebny vybudovat aby sa stihala vyroba:"<< factoriesCount << endl;


        totalByNYears = totalByNYears + yearly;
    }


    return 0;
}