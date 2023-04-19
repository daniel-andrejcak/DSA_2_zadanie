#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>
#include <set>
#include <bitset>
#include <algorithm>
#include <time.h>
#include <fstream>

int kokot()
{
    int varCount = 3;

    srand(time(NULL));

    int pocetKlauzul = varCount - (rand() % (varCount / 2));

    std::cout << pocetKlauzul << std::endl;

    std::string output;

    std::set<char> DNF;

    for (int i = 3; i < 21; i++)
    {
        std::string test = "boolFunkcia";

        if(i < 10)
        {
            test += i + 48;
        }else
        {
            test += (i / 10) + 48;
            test += (i % 10) +48;
        }
        
        test += ".txt";

        std::ofstream out(test);

        test.clear();

        int pocetKlauzul = varCount - (rand() % (varCount / 2));

        for (int i = 0; i < 100; i++)
        {
            for (int i = 0; i < pocetKlauzul; i++)
            {
                for (int i = 0; i < 10; i++)
                {
                    char var;
                    var = 97 + rand() * rand() % varCount;

                    DNF.insert(var);
                }

                for(char var : DNF)
                {
                    output += var;
                    if (rand() % 2 == 0)
                    {
                        output += '\\';
                        output += '\'';
                    }
                }

                if(i != pocetKlauzul - 1)
                output += '+';

                DNF.clear();
            }

            out << output << std::endl;

            output.clear();

        }

        varCount++;
    }

    return 0;    
}