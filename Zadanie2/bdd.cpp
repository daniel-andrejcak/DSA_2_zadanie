#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>
#include <set>
#include <bitset>
#include <algorithm>

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
                if (myVector.at(i)[index + 1] != '\'')
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
                myVector.at(i).erase(i, 2);        
            }
            else if (var[1] != '\'' && myVector.at(i)[index + 1] != '\'')
            {
                
                myVector.at(i).erase(i, 1);
                
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
    std::sort(myVector.begin(), myVector.end());

    std::set<std::string> set(myVector.begin(), myVector.end());

    std::vector<std::string> sortedVector(set.begin(), set.end());

    NODE *node = CHT_Search(sortedVector);

    return node;
}

void insertToHash(std::vector<std::string> myVector, NODE *node)
{
    std::sort(myVector.begin(), myVector.end());

    std::set<std::string> set(myVector.begin(), myVector.end());

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
    NODE *node = searchInHash(products);

    /*if (node)
    {
        std::cout << "uz sa nasla taka "; 
        printVector(products);
        std::cout << "pre premennu " << node->var << std::endl;

        return node;
    
    }*/
    
    bool falseVar = true;

    int singleVarIndex = checkIfSingleVar(products);

    std::cout << "index: " <<  indexPoradia << std::endl;
    std::cout << "vector size: " <<  products.size() << std::endl;

    //zisti, ci sa vo funkcii nachádza samotna premenna, co by ovplyvnila celu funkciu
    if (singleVarIndex != -1)
    {
        char var[2];

        var[0] = products.at(singleVarIndex)[0];
        //poradie.erase(indexPoradia, 1); 

        //indexPoradia = poradie.find(products.at(singleVarIndex)[0] - NUMTOLETTER);

        std::cout << "nasla sa samotna premenna " << products.at(singleVarIndex) << std::endl;        

        NODE *node = createNode(products.at(singleVarIndex)[0]);

        if (products.at(singleVarIndex)[1] == '\'')
        {
            var[1] = products.at(singleVarIndex)[1];
            std::cout << "je negovana\n";
            node->low = trueNode;
            trueNodeUsed = true;

            //products.erase(products.begin() + singleVarIndex);
            deleteClause(products, var);
            if (products.size() == 0)
            {
                node->high = falseNode;
                falseNodeUsed = true;
            }
            else
            {
                var[1] = '0';
                deleteSingleVar(products, var);
                
                if (products.size() == 0)
                {
                    free(node);
                    nodeCount--;

                    return trueNode;
                }else
                {
                    node->high = recMakeBdd(poradie, products, indexPoradia);

                    //pripad, ked sa az pri spracovani nasledujucej premennej dojde k redukcii
                    if (node->high == trueNode)
                    {
                        free(node);
                        nodeCount--;

                        return trueNode;
                    }
                }                
            }
        }

        else
        {
            var[1] = '0';
            std::cout << "nie je negovana\n";

            node->high = trueNode;
            trueNodeUsed = true;
            
            deleteClause(products, var);
            if (products.size() == 0)
            {
                node->low = falseNode;
                falseNodeUsed = true;
            }
            else
            {
                var[1] = '\'';
                deleteSingleVar(products, var);
                
                if (products.size() == 0)
                {
                    free(node);
                    nodeCount--;

                    return trueNode;
                }else
                {
                    node->low = recMakeBdd(poradie, products, indexPoradia);

                    //pripad, ked sa az pri spracovani nasledujucej premennej dojde k redukcii
                    if (node->low == trueNode)
                    {
                        free(node);
                        nodeCount--;

                        return trueNode;
                    }
                }                
            }
        }

        //insertToHash(products, node);
        return node;
    }
    
    std::size_t varIndex;
    
    std::vector<std::string> lowExpresion;
    std::vector<std::string> highExpresion;

    
    node = createNode(poradie[indexPoradia] + NUMTOLETTER);    

    //shannon expansion + rozdelenie co ide do low a co do high
    for (int i = 0; i < products.size(); i++)
    {        
        //printf("%s %d", products[i], poradie[indexPoradia]);
        varIndex = products.at(i).find(poradie[indexPoradia] + NUMTOLETTER);
        std::cout << "hlada: "<< (char)(poradie[indexPoradia] + NUMTOLETTER) << " v: ";
        printVector(products);
        //printf("%d", varIndex);

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

    if(!falseVar)
        indexPoradia--;

    std::cout << "obsah vectorov je: " << std::endl;
    printVector(lowExpresion);
    printVector(highExpresion);

    //insertToHash(products, node);

    if (compareVectors(lowExpresion, highExpresion))
    {
        std::cout << "vectory sa rovnaju" << std::endl;

        free(node);
        nodeCount--;

        return recMakeBdd(poradie, lowExpresion, indexPoradia + 1);
    }
    
    printf("///////////////////////////////////////////\n");
    if (lowExpresion.size() != 0)
    {
        std::cout << "dalsia uroven pre low" << indexPoradia + 1 << " velkost je" << lowExpresion.size() << std::endl;
        printVector(lowExpresion);
        node->low = recMakeBdd(poradie, lowExpresion, indexPoradia + 1);
    }else
    {
        node->low = falseNode;
        falseNodeUsed = true;
    }
    
    
    if (highExpresion.size() != 0)
    {
        std::cout << "dalsia uroven pre high" << indexPoradia + 1 << " velkost je" << highExpresion.size() << std::endl;
        printVector(highExpresion);
        node->high = recMakeBdd(poradie, highExpresion, indexPoradia + 1);
    }else
    {
        node->high = falseNode;
        falseNodeUsed = true;
    }
    


    return node;
}

BDD *BDD_create(std::string bfunkcia, std::string poradie)
{
    //rozbije DNF string na array, kde kazdy prvok je jedna klauzula - divej ako viem ML
    std::vector<std::string> products;
    splitDNFstring(bfunkcia, products);

    //toto je nepotrebne - odstranit ked na to bude vhodny cas
    printf("%d", products.size());
    for (int i = 0; i < products.size(); ++i) 
        std::cout << products.at(i) << "\n";


    //inicializacia globalnych premennych pred vytvorenim stromu
    nodeCount = 0;
    trueNodeUsed = false;
    falseNodeUsed = false;

    printf("pohoda uz to pracuje\n");
    //vytvorenie datovej struktury BDD
    NODE *root = recMakeBdd(poradie, products, 0);

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

BDD *BDD_create_with_best_order (std::string bfunkcia)
{
    BDD *bestBDD;
    BDD *curBDD;

    //aspon tolko pokusov kolko je premennych
    int varCount = countVariables(bfunkcia);

    for (int i = 0; i < varCount; i++)
    {
        if (curBDD->nodeCount < bestBDD->nodeCount)
        {
            //dokoncit program, ktory uvolni kazdy node
            free(bestBDD);

            bestBDD = curBDD;
        }   
    }
    
    return bestBDD;
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

void test(BDD *bdd)
{   
    for (int i = 0; i < pow(2, bdd->varCount); i++)
    {
        std::string values = std::bitset<16>(i).to_string();
        values.erase(values.begin(), values.end() - bdd->varCount);

        std::cout << BDD_use(bdd, values) << std::ends;
    }
}

void freeMem(NODE *node)
{
    if (node == NULL)
        return;
    
    freeMem(node->low);
    freeMem(node->high);

    free(node);
    node = NULL;
}


/*
uz to vie vytvorit neredukovany bdd
uz to vie dat koncove uzly aj ku inej premennej ako poslednej - to je dobre, tak to ma byt - !!!nemenit
uz to redukuje koncove uzly
uz to vie pouzit bdd
uz sa to tvari ze to vie zrobit best order, ale to je klam
uz aj test() fahci popici
*/

//TODO najst problem, lebo kopiruje tu istu premennu do 2 po sebe iducich nodes, je to asi chyba v prvej podmienke
int main ()
{
    InitLeafNodes();

    //pre tieto vstup to fahci...paraada
    //std::string input = "ab\'c\'+abc+a\'bc\'+a\'b\'c";
    std::string input = "ab+ac+bc";
    //std::string input = "ab+bc+a\'b\'c\'";
    //std::string input = "a\'b\'c+a\'bc\'+a\'bc+ab\'c\'+ab\'c+abc\'+abc";
    //std::string input = "abc+bc"; //kontrola ci funguje ak sa low a high expression rovnaju - rovnaju sa pico
    //std::string input = "ab+cd+ef+gh";
    
    //std::string input = "ab+a\'+b\'";

    std::string ciselka = "012";

    BDD *bdd = BDD_create(input, ciselka);
    
    std::cout << std::endl;
    test(bdd);

    std::cout << "\npozor kurvaa " << countVariables(input) << std::endl;
    std::cout << "pocet uzlov ty pico " << bdd->nodeCount << std::endl;
    //freeMem(bdd->root);


    /*std::vector<std::string> myVector = {"ab", "bc"};
    NODE *node = (NODE *)malloc(sizeof(NODE));  

    node->var = 'a';

    CHT_Insert(myVector, node);

    NODE *searchNode = CHT_Search(myVector);

    std::cout << node->var << " prva premenna pico" << std::endl;
    std::cout << searchNode->var << " druha premenna pico" << std::endl;*/


    return 0;
}
