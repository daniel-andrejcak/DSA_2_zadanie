#include <iostream>
#include <fstream>
#include <chrono>

/*int main()
{
    std::string funkcia = "aefg\'j\'kl\'n\'+ah\'k\'l\'n\'op+abfg\'i\'opq\'+a\'ef\'gh\'i\'pq\'+a\'cdh\'j\'k\'l\'o\'p+abd\'jl\'mp+a\'ef\'jk\'m+ab\'f\'gjmp+a\'cde\'f\'h\'lo\'q+ae\'fgj\'k\'mn\'q+b\'ce\'f\'gklmnp+a\'cfhi\'o\'+a\'b\'hm\'o\'pq+a\'cij\'mo\'pq\'+ad\'h\'io\'p";
    char max = funkcia.at(0);

    for (int i = 1; i < funkcia.size(); i++)
    {
        if (funkcia.at(i) > max && funkcia.at(i) != '\'' && funkcia.at(i) != '+')
        {
            max = funkcia.at(i);
        }
    }
    
    std::cout << max << std::endl;

    std::string input;

    for (int i = 97; i < max + 1; i++)
    {
        input += i;
    }
    
    std::cout << input << std::endl;

    return 0;
}
int test()
{
    int integer = 0;
    std::string input;
    char character = 0;
    char switchTemp = 0;
    char secSwitchTemp = 0;

    int pocetPremennych;

    std::string inputFile = "boolFunkcia";
    std::ifstream file;
    std::string bfunkcia;
    std::string poradie;

    //prva inicializacia
    //InitLeafNodes();

    while (1)
    {
        std::cout << "Zadajte prikaz :" << std::endl << "1 - BDD_create test" << std::endl << "2 - BDD_create_with_best_order test" << "3 - vlastny vstup pre BDD" << std::endl << "k - koniec programu" << std::endl;

        std::cin >> switchTemp;

        switch (switchTemp)
        {
        case '1':
            std::cout << "zadajte pocet premennych, pre ktore sa vykona test" << std::endl;
            std::cin >> pocetPremennych;

            if(pocetPremennych < 10)
            {
                inputFile += pocetPremennych + 48;
            }else
            {
                inputFile += (pocetPremennych / 10) + 48;
                inputFile += (pocetPremennych % 10) +48;
            }

            inputFile += ".txt";

            std::cout << inputFile << std::endl;

            file.open(inputFile);

            //vytvorenie poradia, podla ktoreho sa spravi BDD
            for (int i = 0; i < pocetPremennych; i++)
            {
                poradie += i + 97;
            }

            while (file)
            {
                file >> bfunkcia;
                std::cout << bfunkcia << std::endl;


                auto start = std::chrono::steady_clock::now();


                //BDD *bdd = BDD_create(bfunkcia, poradie);

                auto end = std::chrono::steady_clock::now(); 
                int cas = int(std::chrono::duration_cast <std::chrono::nanoseconds> (end - start).count());

                std::cout << cas << std::endl;

                //freeMem(bdd);
            }
            
            break;

        case '2':
            std::cout << "zadajte pocet premennych, pre ktore sa vykona test" << std::endl;
            std::cin >> pocetPremennych;

            if(pocetPremennych < 10)
            {
                inputFile += pocetPremennych + 48;
            }else
            {
                inputFile += (pocetPremennych / 10) + 48;
                inputFile += (pocetPremennych % 10) +48;
            }

            inputFile += ".txt";

            std::cout << inputFile << std::endl;

            file.open(inputFile);

            while (file)
            {
                file >> bfunkcia;
                std::cout << bfunkcia << std::endl;


                auto start = std::chrono::steady_clock::now();


                //BDD *bdd = BDD_create_with_best_order(bfunkcia);

                auto end = std::chrono::steady_clock::now(); 
                int cas = int(std::chrono::duration_cast <std::chrono::nanoseconds> (end - start).count());

                std::cout << cas << std::endl;

                //freeMem(bdd);
            }

            break;

        case '3':
            std::cout << "zadajte boolovsku funkciu pre BDD" << std::endl;
            std:: cin >> bfunkcia;
            std::cout << "zadajte poradie v ktorom sa ma vytvorit BDD" << std::endl;
            std::cin >> poradie;
            std::cout << "vyberte si funkciu" << "1 - BDD_create" << std::endl << "2 - BDD_create_with_best_order" << std::endl;
            std::cin >> secSwitchTemp;

            if (secSwitchTemp = '1')
            {
                //bdd = BDD_create(bfunkcia, poradie);
            }else if (secSwitchTemp = '2')
            {
                //bdd = BDD_create_with_best_order(bfunkcia);
            }else
            {
                std::cout << "zle zadany prikaz" << std::endl;
            }
            
            
            break;

        case 'k':
            std::cout << "koniec programu" << std::endl;

            //freeMem(bdd);
            //delete trueNode;
            //delete falseNode;
            //CHT_FreeMem();
            
            
            return 0;

        default:
            std::cout << "zle zadany prikaz" << std::endl;

            break;
        }
    }    
}  

*/