#include <vector>
#include <iostream>
#include <math.h>

// Uncomment to save final point result possibilities
//#define SAVE_RESULTS

#define FASTEST_LAP

// Include abnormal races
#define HALF_POINTS
#define CANCELLED_RACES

const float maxPoints = 332.5;
const float lewisPoints = 318.5;

float scoresystem[] = { 25, 18, 15, 12, 10, 8, 6, 4, 2, 1, 0 };
float sprintsystem[] = { 3, 2, 1, 0 };


struct PointScenario{
    float maxPoints_;
    float lewisPoints_;
};

// Include sprint race
const int NoRaces = 5;

unsigned long maxWinScenarios = 0;
unsigned long lewisWinScenarios = 0;
unsigned long drawScenarios = 0;

void calculate(){
    std::vector<std::vector<PointScenario>> scenarios;

    // For every race weekend
    for(int race_index=0; race_index<NoRaces; race_index++){
        scenarios.push_back(*(new std::vector<PointScenario>()));

        // Calculate outcome of every weekend
        // Sprint
        if(race_index == 0){
            for(auto score_max : sprintsystem){
                for(auto score_lewis : sprintsystem){
                    // Possible results have different points or both 0
                    // Technically both finishing outside the points is different than
                    // the race be cancelled but the end result is the same so don't
                    // count it twice
                    if(score_max != score_lewis){// || (score_max == 0 && score_lewis == 0)){
                        scenarios[race_index].push_back(PointScenario {score_max, score_lewis});

                        // Half points needed only when both dont have 0
                        // Is this even in the rulebook? Yes it gets halved too
                        if(!(score_max == 0 && score_lewis == 0)){
                            #ifdef HALF_POINTS
                            scenarios[race_index].push_back(PointScenario {score_max/2, score_lewis/2});
                            #endif
                        }
                    }
                }
            }
            // Count only one cancelled race per race (of course, smh...)
            #ifdef CANCELLED_RACES
            scenarios[race_index].push_back(PointScenario {0,0});
            #endif
        } else {
            // Normal races
            for(auto score_max : scoresystem){
                for(auto score_lewis : scoresystem){
                    // Possible results have different points or both 0
                    // Technically both finishing outside the points is different than
                    // the race be cancelled but the end result is the same so don't
                    // count it twice
                    if(score_max != score_lewis){// || (score_max == 0 && score_lewis == 0)){
                        scenarios[race_index].push_back(PointScenario {score_max, score_lewis});
                        
                        // Half points case only when either gets even some points
                        if(!(score_max == 0 && score_lewis == 0)){
                            #ifdef HALF_POINTS
                            scenarios[race_index].push_back(PointScenario {score_max/2, score_lewis/2});
                            #endif
                        }

                        // Add fastest lap scenarios
                        // Above is the case where neither gets it
                        // Apparently fastest lap point can get halved too
                        // 0: Max, 1: Lewis
                        #ifdef FASTEST_LAP
                        for(int i=0; i<2; i++){
                            if(i==0 && score_max != 0){
                                scenarios[race_index].push_back(PointScenario {score_max+1, score_lewis});

                                #ifdef HALF_POINTS
                                scenarios[race_index].push_back(PointScenario {(score_max+1)/2, score_lewis/2});
                                #endif
                            }
                            else if(i==1 && score_lewis != 0){
                                scenarios[race_index].push_back(PointScenario {score_max, score_lewis+1});

                                #ifdef HALF_POINTS
                                scenarios[race_index].push_back(PointScenario {score_max/2, (score_lewis+1)/2});
                                #endif
                            }
                        }
                        #endif
                    }
                }
            }
            // Count only one cancelled race per race (of course, smh...)
            #ifdef CANCELLED_RACES
            scenarios[race_index].push_back(PointScenario {0,0});
            #endif
        }
    }

    // Calculate permutations
    #ifdef SAVE_SCENARIOS
    std::vector<PointScenario> finalPoints;
    #endif

    // Keep count for stats
    unsigned long permus = 0;

    #pragma omp parallel for reduction(+: maxWinScenarios, lewisWinScenarios, drawScenarios, permus)
    for(auto i0 : scenarios[0]){
        for(auto i1 : scenarios[1]){
            for(auto i2 : scenarios[2]){
                for(auto i3 : scenarios[3]){
                    for(auto i4 : scenarios[4]){
                        permus++;
                        
                        // Calculate points for both
                        PointScenario scenario {
                            i0.maxPoints_+i1.maxPoints_+i2.maxPoints_+i3.maxPoints_+i4.maxPoints_,
                            i0.lewisPoints_+i1.lewisPoints_+i2.lewisPoints_+i3.lewisPoints_+i4.lewisPoints_
                        };
                        // And add to total
                        PointScenario finalScenario {
                                maxPoints+scenario.maxPoints_,
                                lewisPoints+scenario.lewisPoints_};

                        // This is not necessary to store
                        #ifdef SAVE_SCENARIOS
                        finalPoints.push_back(finalScenario);
                        #endif

                        // Count winnings
                        // Remember floating point precision
                        if(finalScenario.maxPoints_ - finalScenario.lewisPoints_ > 0.25){
                            maxWinScenarios++;
                        }
                        else if (std::abs(finalScenario.maxPoints_ - finalScenario.lewisPoints_) < 0.25){
                            drawScenarios++;
                        } else {
                            lewisWinScenarios++;
                        }
                    }
                }
            }
        }
    }

    unsigned long cases = 0;
    for(auto race : scenarios){
        cases += race.size();
        std::cout << "Cases in race: " << race.size() << "\n";
    }
    std::cout << "Cases total: " << cases << "\n";
    std::cout << "Permutations: " << permus << "\n";
}

int main()
{
    calculate();

    double totalScenarios = maxWinScenarios + lewisWinScenarios + drawScenarios;
    std::cout << "No. win scenarios for Max: " << maxWinScenarios << 
    "(" << (double)maxWinScenarios/totalScenarios*100 << "%)\n";
    std::cout << "No. win scenarios for Lewis: " << lewisWinScenarios << 
    "(" << (double)lewisWinScenarios/totalScenarios*100 << "%)\n";
    std::cout << "No. draw scenarios: " << drawScenarios << "(" << 
    (double)drawScenarios/totalScenarios*100 << "%)\n";

    return 0;
}