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
#include <fstream>
#include <chrono>

#include "Chaining.h"


#define NUMTOLETTER 49
#define CHARTOINDEX 97

typedef struct bdd
{
    NODE *root;
    int varCount;
    int nodeCount;
}BDD;

//struktura prvku v hash tabulke
typedef struct hashNode
{
    struct hashNode *next;
    std::set<std::string> expresion;
    NODE *node;
}HASHNODE;

NODE *trueNode;
NODE *falseNode;
NODE *globalNode;

int nodeCount;
bool trueNodeUsed;
bool falseNodeUsed;

void InitLeafNodes(){
    trueNode = (NODE *)malloc(sizeof(NODE));
    falseNode = (NODE *)malloc(sizeof(NODE));

    trueNode->high = NULL;
    falseNode->high = NULL;

    trueNode->low = NULL;
    falseNode->low = NULL;

    CHT_Init();
}

int countVariables(std::string bfunkcia)
{
    int count = 0;
    std::set<char> variables;
    char currentChar;

    for (int i = 0; i < bfunkcia.size(); i++)
    {
        currentChar = bfunkcia.at(i);

        if (currentChar >= 'a' && currentChar <= 'z')
        {
            variables.insert(currentChar);
        }
        
    }
    
    return variables.size();
}

void splitDNFstring(std::string string, std::vector<std::string>& products)
{
    std::stringstream stream(string);
    std::string p;

    while (getline(stream, p, '+'))
    {
        products.push_back(p);
    }
}

void printVector(std::vector<std::string> myVector){
    for (int i = 0; i < myVector.size(); i++)
    {
        std::cout << myVector.at(i)<< ", ";
    }

    std::cout << "\n";
    
}

int checkForCredibility(std::vector<std::string> myVector, int index)
{
    for (int i = 0; i < myVector.size(); i++)
    {
        if (myVector.at(i).find(myVector.at(index)[0]) == -1)
        {
            return 0;
        }   
    }

    return 1;
}

int ifSingleVar(std::vector<std::string> myVector, char var)
{
    for (int i = 0; i < myVector.size(); i++)
    {
        int found = myVector.at(i).find(var);

        if (found != -1 && found == 0)
        {
            if (myVector.at(i).size() == 2 && myVector.at(i)[1] == '\'')
            {
                return 1;
            }
            else if (myVector.at(i).size() == 1)
            {
                return 0;
            }
        }
        
    }
    
    return -1;
}

int checkIfSingleVar(std::vector<std::string> myVector)
{
    std::string string;
    int index;

    for (int i = 0; i < myVector.size(); i++)
    {
        string = myVector.at(i);

        if (string.size() <= 2)
        {
            if (string[1] == '\'' || string.size() == 1)
            {
                return i;            
            }
            
        }     
    }

    return -1;
}

int checkForSingleNegVar(std::vector<std::string> myVector)
{
    return 1;
}

void deleteClause(std::vector<std::string>& myVector, char var[2])
{
    for (int i = 0; i < myVector.size(); i++)
    {
        int index = myVector.at(i).find(var[0]);

        if (index != -1)
        {
            if (var[1] == '\'')
            {
                if (myVector.at(i)[index + 1] == '\'')
                {
                    myVector.erase(myVector.begin() + i);
                    i--;
                }
                
            }
            else
            {
                if (myVector.at(i)[index + 1] != '\'' || myVector.at(i).size() == 1)
                {
                    myVector.erase(myVector.begin() + i);
                    i--;
                }
            }
            
        }
    }
}

void deleteSingleVar(std::vector<std::string>& myVector, char var[2])
{
    for (int i = 0; i < myVector.size(); i++)
    {
        int index = myVector.at(i).find(var[0]);

        if (index != -1)
        {
            if (var[1] == '\'' && myVector.at(i)[index + 1] == '\'')
            {
                myVector.at(i).erase(index, 2);        
            }
            else if (var[1] != '\'' && myVector.at(i)[index + 1] != '\'')
            {
                
                myVector.at(i).erase(index, 1);
                
            }

            if (myVector.at(i).size() == 0)
            {
                myVector.erase(myVector.begin() + i);
                i--;
            }
        }
    }
}

NODE *searchInHash(std::vector<std::string> myVector)
{   
    std::vector<std::string> vectorCopy = myVector;

    std::sort(vectorCopy.begin(), vectorCopy.end());

    std::set<std::string> set(vectorCopy.begin(), vectorCopy.end());

    std::vector<std::string> sortedVector(set.begin(), set.end());    

    NODE *node = CHT_Search(sortedVector);

    return node;
}

void insertToHash(std::vector<std::string> myVector, NODE *node)
{
    std::vector<std::string> vectorCopy = myVector;

    std::sort(vectorCopy.begin(), vectorCopy.end());

    std::set<std::string> set(vectorCopy.begin(), vectorCopy.end());

    std::vector<std::string> sortedVector(set.begin(), set.end());

    CHT_Insert(sortedVector, node);
}

bool compareVectors(std::vector<std::string> vector1, std::vector<std::string> vector2)
{
    std::sort(vector1.begin(), vector1.end());
    std::sort(vector2.begin(), vector2.end());

    std::set<std::string> set1(vector1.begin(), vector1.end()); 
    std::set<std::string> set2(vector2.begin(), vector2.end()); 

    return set1 == set2;
}

NODE *createNode(char var)
{
    NODE* node = (NODE *)malloc(sizeof(NODE));
    
    if (!node)
    {
        std::cout << "nedostatok pamate" << std::endl;
        exit(1);
    }
    
    node->var = var;

    node->low = NULL;
    node->high = NULL;

    nodeCount++;
    return node;
}

NODE *recMakeBdd(std::string poradie, std::vector<std::string> products, int indexPoradia)
{
    if (products.size() == 0)
    {
        falseNodeUsed = true;
        return falseNode;
    }
    
    NODE *node = searchInHash(products);
    
    bool falseVar = true;

    int singleVarIndex = ifSingleVar(products, poradie[indexPoradia]);

    if (singleVarIndex != -1)
    {
        char var[2];

        var[0] = poradie[indexPoradia];

        trueNodeUsed = true;

        NODE *node = createNode(poradie[indexPoradia]);

        if (singleVarIndex == 0)
        {
            node->high = trueNode;
            
            var[1] = '0';
        }
        else
        {
            node->low = trueNode;

            var[1] = '\'';
        }

        deleteClause(products, var);
    
        int originalSize = products.size();

        //znamena ze tam bola iba prave tato premenna
        if (originalSize == 0)
        {
            singleVarIndex == 0 ? node->low = falseNode : node->high = falseNode;

            falseNodeUsed = true;

            std::vector<std::string> tempVector;
            std::string tempString{poradie[indexPoradia]};
            tempVector.push_back(tempString);

            NODE *temp = searchInHash(tempVector);

            if (temp)
                return temp;
            

            insertToHash(tempVector, node);

            return node;
        }
        
        singleVarIndex == 0 ? var[1] = '\'' : var[1] = '0';

        deleteSingleVar(products, var);

        if (products.size() < originalSize)
        {
            //free(node);
            nodeCount--;

            return trueNode;
        }    


        if (singleVarIndex == 0)
        {
            NODE *temp = searchInHash(products);

            if (temp == NULL)
            {
                node->low = recMakeBdd(poradie, products, indexPoradia);

                insertToHash(products, node->low);
            }
            else
            {
                node->low = temp;
            }
        }
        else
        {
            NODE *temp = searchInHash(products);

            if (temp == NULL)
            {
                node->high = recMakeBdd(poradie, products, indexPoradia);

                insertToHash(products, node->high);
            }
            else
            {
                node->high = temp;
            }
        }

        if(node->low == node->high)
        {
            NODE *temp = node->low;

            free(node);
            nodeCount--;

            return temp;
        }
        
        return node;
    }
    
    std::size_t varIndex;
    
    std::vector<std::string> lowExpresion;
    std::vector<std::string> highExpresion;

    
    node = createNode(poradie[indexPoradia]);    

    //shannon expansion + rozdelenie co ide do low a co do high
    for (int i = 0; i < products.size(); i++)
    {        
        //printf("%s %d", products[i], poradie[indexPoradia]);
        varIndex = products.at(i).find(poradie[indexPoradia]);

        if (varIndex != -1)
        {
            falseVar = true;
            //printf("nachadza sa\n");
            products.at(i).erase(varIndex, 1);

            if (products.at(i)[varIndex] == '\'')
            {
                products.at(i).erase(varIndex, 1);

                if(products[i].size())
                    lowExpresion.push_back(products[i]);
            }
            else
            {
                if(products[i].size())
                    highExpresion.push_back(products[i]);
            }
        }
        else
        {
            lowExpresion.push_back(products[i]);
                        
            highExpresion.push_back(products[i]);
        }
    }

    //osetrenie toho, ked sa v klauzule nachadzaju 2 rovnake premenne
    for (int i = 0; i < lowExpresion.size(); i++)
    {
        varIndex = lowExpresion.at(i).find(poradie[indexPoradia]);

        if (varIndex != -1)
        {
            if (lowExpresion.at(i)[varIndex + 1] == '\'')
            {
                lowExpresion.at(i).erase(varIndex, 2); 
            }
            
            if (lowExpresion.at(i)[varIndex + 1] != '\'' || lowExpresion.at(i).size() == 0)
            {
                lowExpresion.erase(lowExpresion.begin() + i);
            }
        }
    }

    for (int i = 0; i < highExpresion.size(); i++)
    {
        varIndex = highExpresion.at(i).find(poradie[indexPoradia]);

        if (varIndex != -1)
        {
            if (highExpresion.at(i)[varIndex + 1] != '\'')
            {
                highExpresion.at(i).erase(varIndex, 2); 
            }
            
            if (highExpresion.at(i)[varIndex + 1] == '\'' || highExpresion.at(i).size() == 0)
            {
                highExpresion.erase(highExpresion.begin() + i);
            }
        }
    }

    if (compareVectors(lowExpresion, highExpresion))
    {
        //free(node);
        nodeCount--;

        NODE *temp = searchInHash(lowExpresion);

        if (temp)
            return temp;
        
        temp = recMakeBdd(poradie, lowExpresion, indexPoradia + 1);

        insertToHash(lowExpresion, temp);


        return temp;
    }
    
    if (lowExpresion.size() != 0)
    {
        NODE *temp = searchInHash(lowExpresion);

        if (temp == NULL)
        {
            node->low = recMakeBdd(poradie, lowExpresion, indexPoradia + 1);

            insertToHash(lowExpresion, node->low);
        }
        else
        {
            node->low = temp;
        }
        

    }else
    {
        node->low = falseNode;
        falseNodeUsed = true;
    }
    
    
    if (highExpresion.size() != 0)
    {
        NODE *temp = searchInHash(highExpresion);

        if (temp == NULL)
        {
            node->high = recMakeBdd(poradie, highExpresion, indexPoradia + 1);

            insertToHash(highExpresion, node->high);
        }
        else
        {
            node->high = temp;
        }

    }else
    {
        node->high = falseNode;
        falseNodeUsed = true;
    }

    if(node->low == node->high)
    {
        NODE *temp = node->low;
        //free(node);
        nodeCount--;

        return temp;
    }
    

    return node;
}

BDD *BDD_create(std::string bfunkcia, std::string poradie)
{
    //rozbije DNF string na array, kde kazdy prvok je jedna klauzula - divej ako viem ML
    std::vector<std::string> products;
    splitDNFstring(bfunkcia, products);


    //inicializacia globalnych premennych pred vytvorenim stromu
    nodeCount = 0;
    trueNodeUsed = false;
    falseNodeUsed = false;

    //vytvorenie datovej struktury BDD
    globalNode = recMakeBdd(poradie, products, 0);
    
    NODE *root = globalNode;

    //vytvorenie "objektu" BDD
    BDD *bdd = (BDD *)malloc(sizeof(BDD));

    bdd->root = root;
    bdd->varCount = countVariables(bfunkcia);

    if (trueNodeUsed)
        nodeCount++;

    if (falseNodeUsed)
        nodeCount++;
    
    bdd->nodeCount = nodeCount;

    return bdd;
}

void freeMem(BDD *bdd)
{
    CHT_FreeMem();
    delete bdd->root;
    delete bdd;
    
    CHT_Init();
}

BDD *BDD_create_with_best_order (std::string bfunkcia)
{
    BDD *bdd;

    //aspon tolko pokusov kolko je premennych
    int varCount = countVariables(bfunkcia);

    std::string poradie;
    std::string bestPoradie;
    int bestNodeCount = 0;

    //vytvorenie prveho poradia
    for (int i = 0; i < varCount; i++)
    {
        poradie += i + 97;
    }

    bdd = BDD_create(bfunkcia, poradie);

    bestNodeCount = bdd->nodeCount;
    bestPoradie = poradie;

    freeMem(bdd);

    std::cout << poradie;
    std::cout << std::endl;

    //linearne posuva premenne
    for (int i = 0; i < varCount - 1; i++)
    {
        poradie.insert(0, 1, poradie[varCount - 1]);

        poradie.erase(varCount, 1);

        bdd = BDD_create(bfunkcia, poradie);

        
        if (bdd->nodeCount < bestNodeCount)
        {
            bestNodeCount = bdd->nodeCount;
            bestPoradie = poradie;
        }

        freeMem(bdd);
    }

    //znova vytvori najlepsi BDD
    bdd = BDD_create(bfunkcia, bestPoradie);

    std::cout << bestPoradie << std::endl;


    return bdd;
}

//char *vstupy treba zadat ako string cisel, kde cislo na prvom indexe je hodnota premennej 'a', na druhom indexe premenna 'b',... 
char BDD_use (BDD *bdd, std::string vstupy)
{
    NODE *node = bdd->root;
    char variable;

    while (node != trueNode && node != falseNode)
    {
        variable = vstupy[node->var - CHARTOINDEX];

        if (variable == '0')
        {
            node = node->low;
        }else
        {
            node = node->high;
        }
    }
    
    if (node == trueNode)
    {
        return '1';
    }else if (node == falseNode)
    {
        return '0';
    }
    
    std::cout << "chyba2\n";
    exit(0);
}

void vektorPrint(BDD *bdd)
{   
    for (int i = 0; i < pow(2, bdd->varCount); i++)
    {
        std::string values = std::bitset<16>(i).to_string();
        values.erase(values.begin(), values.end() - bdd->varCount);

        std::cout << BDD_use(bdd, values) << std::ends;
    }
}

int main()
{
    int integer = 0;
    char character = 0;
    char switchTemp = 0;
    char secSwitchTemp = 0;

    int pocetPremennych;

    std::string inputFile = "boolFunkcia";
    std::ifstream file;
    std::string bfunkcia;
    std::string poradie;

    BDD *bdd;

    //prva inicializacia
    InitLeafNodes();

    while (1)
    {
        std::cout << "Zadajte prikaz :" << std::endl << "1 - BDD_create test" << std::endl << "2 - BDD_create_with_best_order test" << std::endl << "3 - vlastny vstup pre BDD" << std::endl << "k - koniec programu" << std::endl;

        std::cin >> switchTemp;

        switch (switchTemp)
        {
        case '1':
            std::cout << "zadajte pocet premennych, pre ktore sa vykona test (minimalne 3, maximalne 20)" << std::endl;
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

            file.open(inputFile);

            //vytvorenie poradia, podla ktoreho sa spravi BDD
            for (int i = 0; i < 24; i++)
            {
                poradie += i + 97;
            }

            while (file)
            {
                file >> bfunkcia;

                auto start = std::chrono::steady_clock::now();


                BDD *bdd = BDD_create(bfunkcia, poradie);

                auto end = std::chrono::steady_clock::now(); 
                int cas = int(std::chrono::duration_cast <std::chrono::nanoseconds> (end - start).count());

                std::cout << cas << std::endl;

                freeMem(bdd);
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

            file.open(inputFile);

            while (file)
            {
                file >> bfunkcia;

                auto start = std::chrono::steady_clock::now();


                BDD *bdd = BDD_create_with_best_order(bfunkcia);

                auto end = std::chrono::steady_clock::now(); 
                int cas = int(std::chrono::duration_cast <std::chrono::nanoseconds> (end - start).count());

                std::cout << cas << std::endl;

                freeMem(bdd);
            }


            break;

        case '3':
            std::cout << "zadajte boolovsku funkciu pre BDD" << std::endl;
            std:: cin >> bfunkcia;
            std::cout << "zadajte poradie v ktorom sa ma vytvorit BDD" << std::endl;
            std::cin >> poradie;
            std::cout << "vyberte si funkciu" << std::endl << "1 - BDD_create" << std::endl << "2 - BDD_create_with_best_order" << std::endl;
            std::cin >> secSwitchTemp;

            if (secSwitchTemp = '1')
            {
                bdd = BDD_create(bfunkcia, poradie);
            }else if (secSwitchTemp = '2')
            {
                bdd = BDD_create_with_best_order(bfunkcia);
            }else
            {
                std::cout << "zle zadany prikaz" << std::endl;
                break;
            }
            
            std::cout << "Vektor pre funkciu" << std::endl;
            vektorPrint(bdd);
            std::cout << std::endl;

            std::cout << "Pocet premennych vo funkcii " << bdd->varCount << std::endl;
            std::cout << "Pocet uzlov v BDD " << bdd->nodeCount << std::endl;


            freeMem(bdd);
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

        bfunkcia.clear();
        poradie.clear();
        inputFile = "boolFunkcia";
        file.close();

    }    
}  



/*int main ()
{
    InitLeafNodes();

    //pre tieto vstup to fahci...paraada
    //std::string input = "ab\'c\'+abc+a\'bc\'+a\'b\'c";
    //std::string input = "ab+ac+bc";
    //std::string input = "ab+bc+a\'b\'c\'";
    //std::string input = "abc+bc"; //kontrola ci funguje ak sa low a high expression rovnaju - rovnaju sa
    //std::string input = "a\'b\'c+a\'bc\'+a\'bc+ab\'c\'+ab\'c+abc\'+abc";
    //std::string input = "abc+a\'c+b+b\'"; //skusa tautologie
    //std::string input = "a\'b\'c+a\'bc\'+a\'bc+ab\'c\'+ab\'c+abc\'+abc+a\'b\'c\'"; //aj komplikovanejsia tautologia funguje
    //std::string input = "ab+cd+ef+gh";
    //std::string input = "abc"; //uz to zvladne aj jednu klauzulu
    //std::string input = "aa\'bc"; //skuska kontradicie
    //std::string input = "a\'i\'j\'kl\'no\'+ad\'e\'gm\'p+a\'ceg\'ik+a\'d\'f\'i\'j\'k\'l\'n\'q\'+acg\'i\'mq\'+a\'cde\'g\'k\'np\'+a\'bce\'gh\'mq+ac\'gj\'kl\'m\'o\'p+a\'ce\'g\'i\'mo\'+ac\'e\'fgj\'m\'pq\'+ad\'e\'f\'i\'mqr\'+acd\'i\'mo\'+a\'bc\'ghq\'+ac\'g\'i\'k\'m\'+c\'f\'g\'h\'i\'n\'pq\'";
    //std::string input = "ab+a\'d+db\'";

    //std::string input = "a\'fj\'oqst\'+acf\'g\'ops+f+f\'+a\'b\'emp\'qr\'+a\'ij\'lqt\'+eg\'i\'jkmp+aem\'no\'p\'q\'+ac\'ef\'g\'j\'n\'qt+a\'e\'giq\'r\'s+a\'e\'gk\'lp\'+ac\'gikl\'rt+d\'f\'h\'imo\'pq\'+a\'cdf\'kno\'p\'+aop\'q";
    std::string input = "a\'bdghk\'lm+a\'b\'di\'k\'l+ad\'fijk\'l\'+ab\'c\'d\'e\'f\'gjm+a\'de\'f\'g\'h\'il\'m+a\'b\'d\'e\'fh\'l\'m\'+be\'fghi\'jk\'m\'+a\'ce\'f\'g\'ikl\'+ad\'eh\'m\'";

    //std::string input = "ac\'+abc+a\'b+b\'c";



    std::string postupnost = "abcdefghijklmnopqrst";

    BDD *bdd = BDD_create(input, postupnost);
    std::cout << "\npocet premennych " << countVariables(input) << std::endl;
    std::cout << "pocet uzlov " << bdd->nodeCount << std::endl;

    //test(bdd);
    
    std::cout << std::endl;

    //visualize();



    std::cout << "pohoda uz to vymazalo" << std::endl;
    freeMem(bdd);
    
    return 0;
}
*/