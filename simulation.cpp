/**
 * @file simulation.cpp
 * @author Rebeka Cernianska (xcerni13), Peter Rucek (xrucek00)
 * @brief Simulation of model from http://www.jiem.org/index.php/jiem/article/view/2742 
 *          on various experiments 
 * @date 2021-12-08
 * @copyright Copyright (c) 2021
 */

#include <iostream>
#include <random>
#include <cmath>
#include <stdlib.h>
#include <chrono>
#include <cstring>


typedef std::chrono::high_resolution_clock myclock;
using std::cout;
using std::endl;

double TruncateNumber(double In, unsigned int Digits)
{
    double f = pow(10, Digits);
    return ((int)(In*f))/f;
}

/**
 * @brief Generates random number in normal distribution
 * 
 */
class NormalDistribution {
    myclock::time_point beginning = myclock::now();
    std::default_random_engine generator;
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

/**
 * @brief Generates random number in exponential distribution
 * 
 */
class ExponentialDistribution {
    myclock::time_point beginning = myclock::now();
    std::default_random_engine generator;
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

/**
 * @brief Represents the semiconductors factory
 * 
 */
class Factory
{
public:
    
    int M;  // Number of machines
    int Td; // Time factory is working [seconds]
    Factory(int m = 3, int td = 60*60*24)
    {
        M = m;
        Td = td;
    }

    /**
     * @brief Simulates the model factory
     * 
     * @return number of semiconductors manufactured per @param Td
     */
    long SimulateTime()
    {
        NormalDistribution CTwb(6.0902, 6.4609);   // Cycle Time Wire Bond
        NormalDistribution CTda(2.8072, 2.9460);   // Cycle Time Die Attach
        NormalDistribution CTpc(0.9882, 1.0498);   // Cycle Time Pre-Cap Inspection
        ExponentialDistribution DDda(2141, 4391);  // Downtime Duration Die Attach
        ExponentialDistribution DDwb(1364, 2797);  // Downtime Duration Wire Bond
        ExponentialDistribution DFda(1066, 2187);  // Downtime Frequency Die Attach
        ExponentialDistribution DFwb(1421, 2193);  // Downtime Frequency Wire Bond
        ExponentialDistribution STda(2957, 6063);  // Setup Time Die Attach
        ExponentialDistribution STwb (1324, 2714); // Setup Time Wire Bond

        int Q[] = {2200,3080,11264}; // Batch size 
        srand(time(NULL));
        int Qcur = Q[0];

        int CToc = 7200; // Cycle Time Oven Cure

        Qcur = Q[rand() % 3];
        long Ttotal= ( (Td-STda())*((86400-DDda())/86400) + Td +(Td-STwb())*((86400-DDwb())/86400)+Td ) * M ;
        long Tb = CToc + 387.1239*(CTda()-2.8766)/0.0694 + 1008.0011*(CTwb()-6.2756)/0.1854 +
            157.9690*(CTpc()-1.019)/0.0308 + 9.5167*(DDda()-3266)/1125 + 3.9325*(DDwb()-2080.5)/716.5 +
            2.4477*(DFda()-1626.5)/560.5 - 0.1807*(DFwb()-1807)/386 - 2.6148*(STda()-4510)/1553 +
            24.3713*(STwb()-2019)/695 + 10.1729*Qcur;

        long Om = Ttotal * Qcur / Tb;

        return Om;
    }
};


/**
 * @brief Generates size factories with parameters
 *
 * @param size size of factory (m)
 * @param count count of facotries to be generated
 * @param time time, the factories will be working [sec]
 * @return  number of semiconductors per @time from @count facotries of @size 
 */
long GenerateFactories(int size, int count, int time)
{
    long num = 0;
    for (int i = 0 ; i < count ; i++)
    {
        Factory *factory = new Factory(size, time);
        int Om = factory->SimulateTime();
        num += Om;
    }
    return num;
}

/**
 * @brief Calculates mean and standart deviation of model and our representation
 * 
 */
void Expetiment1()
{
    const int NUMBER_OF_TEST_SAMPLES = 21;

    // data from tables
    double CTDA_VALS []= {2.8346, 2.907, 2.9342, 2.8936, 2.9046, 2.8971, 2.8378,
                          2.8432, 2.8342,2.9032, 2.9178, 2.8190, 2.8772, 2.8100,
                          2.9328, 2.8477, 2.8342,2.8146, 2.9055, 2.8418, 2.9181};

    double CTWB_VALS []= {6.1796, 6.3721, 6.1890, 6.3850, 6.2920, 6.2297, 6.2860,
                          6.1901, 6.2720, 6.1126, 6.3301, 6.1450, 6.3021, 6.2378,
                          6.2891, 6.3310, 6.3187, 6.1722, 6.1137, 6.2671, 6.1263};
                          
    double CTPC_VALS []= {1.0199, 0.9977, 1.0288, 0.9880, 1.0186, 0.9883, 1.0078,
                          1.0079, 0.9220, 1.0260, 1.0348, 0.9972, 1.0152, 1.020,
                          0.9973, 1.0256, 1.0200, 1.0450, 1.0123, 1.0150, 1.0190};

    int DDDA_VALS []= {2241, 3190, 2845, 3986, 4097, 3967, 2988,
                       3990, 4010, 3720, 2899, 2690, 2477, 3320,
                       3966, 4210, 3277, 3547, 4177, 4091, 2851};

    int DDWB_VALS []= {1599, 1987, 2366, 1822, 1769, 2544, 1479,
                       2265, 2740, 1608, 1790, 2011, 2390, 2700,
                       2541, 1922, 2410, 2180, 2419, 2655, 2399};

    int DFDA_VALS []= {1601, 2067, 1339, 1845, 1937, 2080, 2065,
                       1205, 1580, 1945, 1540, 1766, 2010, 2087,
                       2019, 1579, 1368, 1392, 1752, 2180, 1611};

    int DFWB_VALS []= {1790, 1544, 1988, 1756, 2147, 2079, 1655,
                       1742, 1823, 2009, 1990, 2080, 1590, 1630,
                       1762, 1934, 2076, 1855, 1988, 1628, 2070};

    int STDA_VALS []= {3867, 3099, 5427, 4635, 4906, 3782, 3056,
                       4932, 5109, 3550, 5742, 5230, 4480, 5020,
                       5564, 5716, 4288, 4399, 4980, 5927, 3899};

    int STWB_VALS []= {2613, 2561, 1762, 2611, 2701, 1677, 1577,
                       2430, 2054, 2689, 2090, 2190, 1988, 1420,
                       1436, 1788, 1867, 2090, 1645, 2654, 2017};

    long TTOTAL_VALS []= {994433, 992858, 979236, 980697, 981625, 980148, 993847,
                          975787, 975532, 984679, 977745, 979935, 984775, 981757,
                          993745, 994579, 1002224,999328, 994918, 994747, 1005139};

    long OM_VALS []= {72902, 91956, 92674, 82019, 80998, 92276, 76479,
                      90772, 78789, 80136, 89995, 75844, 72967, 79716,
                      81412, 72110, 90693, 70974, 72538, 78620, 92051};

    // calculating table means
    double ctda_mean = 0;
    double ctwb_mean = 0;
    double ctpc_mean = 0;
    double ddda_mean = 0;
    double ddwb_mean = 0;
    double dfda_mean = 0;
    double dfwb_mean = 0;
    double stda_mean = 0;
    double stwb_mean = 0;
    long ttotal_mean = 0;
    long om_mean = 0;
    for (int i = 0; i < NUMBER_OF_TEST_SAMPLES; i++)
    {
        ctda_mean += CTDA_VALS[i];
        ctwb_mean += CTWB_VALS[i];
        ctpc_mean += CTPC_VALS[i];
        ddda_mean += DDDA_VALS[i];
        ddwb_mean += DDWB_VALS[i];
        dfda_mean += DFDA_VALS[i];
        dfwb_mean += DFWB_VALS[i];
        stda_mean += STDA_VALS[i];
        stwb_mean += STWB_VALS[i];
        ttotal_mean += TTOTAL_VALS[i];
        om_mean += OM_VALS[i];
    }
    ctda_mean /= NUMBER_OF_TEST_SAMPLES;
    ctwb_mean /= NUMBER_OF_TEST_SAMPLES;
    ctpc_mean /= NUMBER_OF_TEST_SAMPLES;
    ddda_mean /= NUMBER_OF_TEST_SAMPLES;
    ddwb_mean /= NUMBER_OF_TEST_SAMPLES;
    dfda_mean /= NUMBER_OF_TEST_SAMPLES;
    dfwb_mean /= NUMBER_OF_TEST_SAMPLES;
    stda_mean /= NUMBER_OF_TEST_SAMPLES;
    stwb_mean /= NUMBER_OF_TEST_SAMPLES;
    ttotal_mean /= NUMBER_OF_TEST_SAMPLES;
    om_mean /= NUMBER_OF_TEST_SAMPLES;
    
    //calculating table standart deviations
    double ctda_sd = 0;
    double ctwb_sd = 0;
    double ctpc_sd = 0;
    double ddda_sd = 0;
    double ddwb_sd = 0;
    double dfda_sd = 0;
    double dfwb_sd = 0;
    double stda_sd = 0;
    double stwb_sd = 0;
    long ttotal_sd = 0;
    long om_sd = 0;
    for (int n = 0; n < NUMBER_OF_TEST_SAMPLES; n++ )
    {
        ctda_sd += (CTDA_VALS[n] - ctda_mean) * (CTDA_VALS[n] - ctda_mean);
        ctwb_sd += (CTWB_VALS[n] - ctwb_mean) * (CTWB_VALS[n] - ctwb_mean);
        ctpc_sd += (CTPC_VALS[n] - ctpc_mean) * (CTPC_VALS[n] - ctpc_mean);
        ddda_sd += (DDDA_VALS[n] - ddda_mean) * (DDDA_VALS[n] - ddda_mean);
        ddwb_sd += (DDWB_VALS[n] - ddwb_mean) * (DDWB_VALS[n] - ddwb_mean);
        dfda_sd += (DFDA_VALS[n] - dfda_mean) * (DFDA_VALS[n] - dfda_mean);
        dfwb_sd += (DFWB_VALS[n] - dfwb_mean) * (DFWB_VALS[n] - dfwb_mean);
        stda_sd += (STDA_VALS[n] - stda_mean) * (STDA_VALS[n] - stda_mean);
        stwb_sd += (STWB_VALS[n] - stwb_mean) * (STWB_VALS[n] - stwb_mean);
        ttotal_sd += (TTOTAL_VALS[n] - ttotal_mean) * (TTOTAL_VALS[n] - ttotal_mean);
        om_sd += (OM_VALS[n] - om_mean) * (OM_VALS[n] - om_mean);
    }
    ctda_sd = sqrt(ctda_sd / NUMBER_OF_TEST_SAMPLES);
    ctwb_sd = sqrt(ctwb_sd / NUMBER_OF_TEST_SAMPLES);
    ctpc_sd = sqrt(ctpc_sd / NUMBER_OF_TEST_SAMPLES);
    ddda_sd = sqrt(ddda_sd / NUMBER_OF_TEST_SAMPLES);
    ddwb_sd = sqrt(ddwb_sd / NUMBER_OF_TEST_SAMPLES);
    dfda_sd = sqrt(dfda_sd / NUMBER_OF_TEST_SAMPLES);
    dfwb_sd = sqrt(dfwb_sd / NUMBER_OF_TEST_SAMPLES);
    stda_sd = sqrt(stda_sd / NUMBER_OF_TEST_SAMPLES);
    stwb_sd = sqrt(stwb_sd / NUMBER_OF_TEST_SAMPLES);
    ttotal_sd = sqrt(ttotal_sd / NUMBER_OF_TEST_SAMPLES);
    om_sd = sqrt(om_sd / NUMBER_OF_TEST_SAMPLES);

    // generating random values
    NormalDistribution CTwb(6.0902, 6.4609);  // Cycle Time Wire Bond
    NormalDistribution CTda(2.8072, 2.9460);  // Cycle Time Die Attach
    NormalDistribution CTpc(0.9882, 1.0498);  // Cycle Time Pre-Cap Inspection
    ExponentialDistribution DDda(2141, 4391); // Downtime Duration Die Attach
    ExponentialDistribution DDwb(1364, 2797); // Downtime Duration Wire Bond
    ExponentialDistribution DFda(1066, 2187); // Downtime Frequency Die Attach
    ExponentialDistribution DFwb(1421, 2193); // Downtime Frequency Wire Bond
    ExponentialDistribution STda(2957, 6063); // Setup Time Die Attach
    ExponentialDistribution STwb (1324, 2714);// Setup Time Wire Bond

    // calculating model simulation means
    double test_ctda_mean = 0;
    double test_ctwb_mean = 0;
    double test_ctpc_mean = 0;
    double test_ddda_mean = 0;
    double test_ddwb_mean = 0;
    double test_dfda_mean = 0;
    double test_dfwb_mean = 0;
    double test_stda_mean = 0;
    double test_stwb_mean = 0;
    long test_ttotal_mean = 0;
    long test_om_mean = 0;

    double ctda_array [NUMBER_OF_TEST_SAMPLES];
    double ctwb_array [NUMBER_OF_TEST_SAMPLES];
    double ctpc_array [NUMBER_OF_TEST_SAMPLES];
    double ddda_array [NUMBER_OF_TEST_SAMPLES];
    double ddwb_array [NUMBER_OF_TEST_SAMPLES];
    double dfda_array [NUMBER_OF_TEST_SAMPLES];
    double dfwb_array [NUMBER_OF_TEST_SAMPLES];
    double stda_array [NUMBER_OF_TEST_SAMPLES];
    double stwb_array [NUMBER_OF_TEST_SAMPLES];
    long ttotal_array [NUMBER_OF_TEST_SAMPLES];
    long om_array [NUMBER_OF_TEST_SAMPLES];

    for (int i = 0; i < NUMBER_OF_TEST_SAMPLES; i++)
    {
        ctda_array[i] = CTda();
        test_ctda_mean += ctda_array[i];
        ctwb_array[i] = CTwb();
        test_ctwb_mean += ctwb_array[i];
        ctpc_array[i] = CTpc();
        test_ctpc_mean += ctpc_array[i];
        ddda_array[i] = DDda();
        test_ddda_mean += ddda_array[i];
        ddwb_array[i] = DDwb();
        test_ddwb_mean += ddwb_array[i] ;
        dfda_array[i] = DFda();
        test_dfda_mean +=  dfda_array[i];
        dfwb_array[i] = DFwb();
        test_dfwb_mean += dfwb_array[i];
        stda_array[i] = STda();
        test_stda_mean += stda_array[i];
        stwb_array[i] = STwb();
        test_stwb_mean += stwb_array[i];

        int Td = 24*60*60; int M= 3;
        ttotal_array[i] = ( (Td-STda())*((86400-DDda())/86400) + Td +(Td-STwb())*((86400-DDwb())/86400)+Td ) * M;
        test_ttotal_mean += ttotal_array[i];

        Factory *factory = new Factory();
        om_array[i] = factory->SimulateTime();
        test_om_mean += om_array[i];
    }
    test_ctda_mean /= NUMBER_OF_TEST_SAMPLES;
    test_ctwb_mean /= NUMBER_OF_TEST_SAMPLES;
    test_ctpc_mean /= NUMBER_OF_TEST_SAMPLES;
    test_ddda_mean /= NUMBER_OF_TEST_SAMPLES;
    test_ddwb_mean /= NUMBER_OF_TEST_SAMPLES;
    test_dfwb_mean /= NUMBER_OF_TEST_SAMPLES;
    test_dfda_mean /= NUMBER_OF_TEST_SAMPLES;
    test_stda_mean /= NUMBER_OF_TEST_SAMPLES;
    test_stwb_mean /= NUMBER_OF_TEST_SAMPLES;
    test_ttotal_mean /= NUMBER_OF_TEST_SAMPLES;
    test_om_mean /= NUMBER_OF_TEST_SAMPLES;

    // calculating model simulation standart deviations
    double test_ctda_sd = 0;
    double test_ctwb_sd = 0;
    double test_ctpc_sd = 0;
    double test_ddda_sd = 0;
    double test_ddwb_sd = 0;
    double test_dfda_sd = 0;
    double test_dfwb_sd = 0;
    double test_stda_sd = 0;
    double test_stwb_sd = 0;
    long test_ttotal_sd = 0;
    long test_om_sd = 0;
    for (int n = 0; n < NUMBER_OF_TEST_SAMPLES; n++ )
    {
        test_ctda_sd += (ctda_array[n] - test_ctda_mean) * (ctda_array[n] - test_ctda_mean);
        test_ctwb_sd += (ctwb_array[n] - test_ctwb_mean) * (ctwb_array[n] - test_ctwb_mean);
        test_ctpc_sd += (ctpc_array[n] - test_ctpc_mean) * (ctpc_array[n] - test_ctpc_mean);
        test_ddda_sd += (ddda_array[n] - test_ddda_mean) * (ddda_array[n] - test_ddda_mean);
        test_ddwb_sd += (ddwb_array[n] - test_ddwb_mean) * (ddwb_array[n] - test_ddwb_mean);
        test_dfda_sd += (dfda_array[n] - test_dfda_mean) * (dfda_array[n] - test_dfda_mean);
        test_dfwb_sd += (dfwb_array[n] - test_dfwb_mean) * (dfwb_array[n] - test_dfwb_mean);
        test_stda_sd += (stda_array[n] - test_stda_mean) * (stda_array[n] - test_stda_mean);
        test_stwb_sd += (stwb_array[n] - test_stwb_mean) * (stwb_array[n] - test_stwb_mean);
        test_ttotal_sd += (ttotal_array[n] - test_ttotal_mean) * (ttotal_array[n] - test_ttotal_mean);
        test_om_sd += (om_array[n] - test_om_mean) * (om_array[n] - test_om_mean);
    }
    test_ctda_sd = sqrt(test_ctda_sd / NUMBER_OF_TEST_SAMPLES);
    test_ctwb_sd = sqrt(test_ctwb_sd / NUMBER_OF_TEST_SAMPLES);
    test_ctpc_sd = sqrt(test_ctpc_sd / NUMBER_OF_TEST_SAMPLES);
    test_ddda_sd = sqrt(test_ddda_sd / NUMBER_OF_TEST_SAMPLES);
    test_ddwb_sd = sqrt(test_ddwb_sd / NUMBER_OF_TEST_SAMPLES);
    test_dfda_sd = sqrt(test_dfda_sd / NUMBER_OF_TEST_SAMPLES);
    test_dfwb_sd = sqrt(test_dfwb_sd / NUMBER_OF_TEST_SAMPLES);
    test_stda_sd = sqrt(test_stda_sd / NUMBER_OF_TEST_SAMPLES);
    test_stwb_sd = sqrt(test_stwb_sd / NUMBER_OF_TEST_SAMPLES);
    test_ttotal_sd = sqrt(test_ttotal_sd / NUMBER_OF_TEST_SAMPLES);
    test_om_sd = sqrt(test_om_sd / NUMBER_OF_TEST_SAMPLES);


    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
    cout << "Experiment 1 - validacia modelu:" << endl << endl;
    cout << "Expected CTDA mean: " << ctda_mean << "\tour: " <<  test_ctda_mean << "\tdiff: " << abs(ctda_mean - test_ctda_mean) <<  endl;
    cout << "Expected CTDA sd  : " << ctda_sd << "\tour: " <<  test_ctda_sd << "\tdiff: " << abs(ctda_sd - test_ctda_sd) <<  endl << endl;

    cout << "Expected CTWB mean: " << ctwb_mean << "\tour: " <<  test_ctwb_mean << "\tdiff: " << abs(ctwb_mean - test_ctwb_mean) <<  endl;
    cout << "Expected CTWB sd  : " << ctwb_sd << "\tour: " <<  test_ctwb_sd << "\tdiff: " << abs(ctwb_sd - test_ctwb_sd) <<  endl << endl;

    cout << "Expected CTPC mean: " << ctpc_mean << "\tour: " <<  test_ctpc_mean << "\tdiff: " << abs(ctpc_mean - test_ctpc_mean) <<  endl;
    cout << "Expected CTPC sd  : " << ctpc_sd << "\tour: " <<  test_ctpc_sd << "\tdiff: " << abs(ctpc_sd - test_ctpc_sd) <<  endl << endl;

    cout << "Expected DDDA mean: " << ddda_mean << "\tour: " <<  test_ddda_mean << "\tdiff: " << abs(ddda_mean - test_ddda_mean) <<  endl;
    cout << "Expected DDDA sd  : " << ddda_sd << "\tour: " <<  test_ddda_sd << "\tdiff: " << abs(ddda_sd - test_ddda_sd) <<  endl << endl;

    cout << "Expected DDWB mean: " << ddwb_mean << "\tour: " <<  test_ddwb_mean << "\tdiff: " << abs(ddwb_mean - test_ddwb_mean) <<  endl;
    cout << "Expected DDWB sd  : " << ddwb_sd << "\tour: " <<  test_ddwb_sd << "\tdiff: " << abs(ddwb_sd - test_ddwb_sd) <<  endl << endl;

    cout << "Expected DFDA mean: " << dfda_mean << "\tour: " <<  test_dfda_mean << "\tdiff: " << abs(dfda_mean - test_dfda_mean) <<  endl;
    cout << "Expected DFDA sd  : " << dfda_sd << "\tour: " <<  test_dfda_sd << "\tdiff: " << abs(dfda_sd - test_dfda_sd) <<  endl << endl;

    cout << "Expected DFWB mean: " << dfwb_mean << "\tour: " <<  test_dfwb_mean << "\tdiff: " << abs(dfwb_mean - test_dfwb_mean) <<  endl;
    cout << "Expected DFWB sd  : " << dfwb_sd << "\tour: " <<  test_dfwb_sd << "\tdiff: " << abs(dfwb_sd - test_dfwb_sd) <<  endl << endl;

    cout << "Expected STDA mean: " << stda_mean << "\tour: " <<  test_stda_mean << "\tdiff: " << abs(stda_mean - test_stda_mean) <<  endl;
    cout << "Expected STDA sd  : " << stda_sd << "\tour: " <<  test_stda_sd << "\tdiff: " << abs(stda_sd - test_stda_sd) <<  endl << endl;

    cout << "Expected STWB mean: " << stwb_mean << "\tour: " <<  test_stwb_mean << " diff: " << abs(stwb_mean - test_stwb_mean) <<  endl;
    cout << "Expected STWB sd  : " << stwb_sd << "\tour: " <<  test_stwb_sd << " diff: " << abs(stwb_sd - test_stwb_sd) <<  endl << endl;


    cout << "Expected Ttotal mean: " << ttotal_mean << "\tour: " <<  test_ttotal_mean << "\tdiff: " << abs(ttotal_mean - test_ttotal_mean) <<  endl;
    cout << "Expected Ttotal sd  : " << ttotal_sd << "\tour: " <<  test_ttotal_sd << "\tdiff: " << abs(ttotal_sd - test_ttotal_sd) <<  endl << endl;

    cout << "Expected Om mean: " << om_mean << "\tour: " <<  test_om_mean << "\tdiff: " << abs(om_mean - test_om_mean) <<  endl;
    cout << "Expected Om sd  : " << om_sd << "\tour: " <<  test_om_sd << "\tdiff: " << abs(om_sd - test_om_sd) <<  endl;
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
}


/**
 * @brief Calculates number of factories needed to manufacture 1.14 trilion semiconductors
 * 
 */
void Expetiment2()
{
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
    cout << "Experiment 2 - kolko modelovych fabrik je treba na to aby zabezpecili vyrobu za rok 2021:" << endl << endl;
    #define TOTAL_2021 1140000000000 // 1.14 trilion semiconductors manufactured during year 2021
    const int numberOfRuns = 10;
    int averageFactoriesCount = 0;

    for (int i = 0; i < numberOfRuns; i++) // average of 10 runs
    {
        int factoriesCount = 0;
        while (true)
        {
            long long count = GenerateFactories(3, factoriesCount, 365*24*60*60); // one year
            if (count > TOTAL_2021)
            {
                break;
            }
            else
            {
                factoriesCount += 500; // approximating
            }
        }
        averageFactoriesCount += factoriesCount;
    }
    averageFactoriesCount /= numberOfRuns;

    cout << "1.14 trilion polovodicov vyrobi "<< averageFactoriesCount << " modelovych fabrik za rok." << endl;
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
}


/**
 * @brief Calculates number of factories to be built every year to satisfy the production
 * 
 */
void Expetiment3()
{
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
    cout << "Experiment 3 - kolko je treba vybudovat modelovych fabrik rocne aby to stacilo prirodzenemu rastu vyroby (6.25%):" << endl << endl;
    #define TOTAL_2021 1140000000000 // 1.14 trilion semiconductors manufactured during year 2021
    const int numberOfRuns = 10;
    long totalByNYears = TOTAL_2021; 

    for(int year = 1 ; year < 6 ; year++) // next 5 years
    {
        int averageFactoriesCount = 0;
        int averageCount = 0;
        for (int i = 0; i < numberOfRuns; i++) // average of 10
        {
            int factoriesCount = 0;
            long rise = (totalByNYears / 100) * 6.25;
            long long count = 0;
            while (true)
            {
                count = GenerateFactories(3, factoriesCount, 365*60*60*24); // one year
                if (count > rise) // 6.25 % from previous year
                {
                    break;
                }
                else
                {
                    factoriesCount += 50; // approximating
                }
            }
            averageFactoriesCount += factoriesCount;
            averageCount += count;
        }
        averageFactoriesCount /= numberOfRuns;
        averageCount /= numberOfRuns;
        
        cout << "po "<< year <<" rokoch, pocet fabrik potrebny vybudovat aby sa stihala vyroba:"<< averageFactoriesCount << endl;
    
        totalByNYears = totalByNYears + averageCount;
    }
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
}


int main(int argc, char *argv[])
{

    bool one = false;
    bool two = false;
    bool three = false;
    for (int i = 1 ; i < argc; i++)
    {
        if (strcmp(argv[i], "-1") == 0)
        {
            one = true;
        }
        else if (strcmp(argv[i], "-2") == 0)
        {
            two = true;
        }
        else if (strcmp(argv[i], "-3") == 0)
        {
            three = true;
        }
        else
        {
            cout << "Usage : ./simualtion [-1] [-2] [-3]" << endl;
            return 1;
        }
    }
    if (one || argc == 1) Expetiment1();
    if (two || argc == 1) Expetiment2();
    if (three || argc == 1) Expetiment3();

    return 0;
}