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


#define NUMTOLETTER 49
#define CHARTOINDEX 97

typedef struct node
{
    char var;
    struct node *low;
    struct node *high;
}NODE;

typedef struct bdd
{
    NODE *root;
    int varCount;
    int nodeCount;
}BDD;

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

void deleteNegCharFrom(std::vector<std::string>& myVector, char Char)
{
    for (int i = 0; i < myVector.size(); i++)
    {
        int index = myVector.at(i).find(Char);

        if (index != -1)
        {
            std::cout << "maze sa: "<< myVector.at(i) << std::endl;

            myVector.at(i).erase(index, 1);
        
            if (myVector.at(i)[index] == '\'')
            {
                myVector.erase(myVector.begin() + i);
                
            }
            
            if (myVector.size() && myVector.at(i).size() == 0)
            {
                myVector.erase(myVector.begin() + i);
            }

            i--;
        }
    }
}

void deleteCharFrom(std::vector<std::string>& myVector, char Char)
{
    for (int i = 0; i < myVector.size(); i++)
    {
        int index = myVector.at(i).find(Char);

        if (index != -1)
        {
            std::cout << "maze sa: "<< myVector.at(i) << std::endl;
            myVector.at(i).erase(index, 1);
        
            if (myVector.at(i)[index] != '\'')
            {
                myVector.erase(myVector.begin() + i);
                
            }
            else if (myVector.at(i)[index] == '\'')
            {
                myVector.at(i).erase(index, 1);
                
            }

            if (myVector.size() && myVector.at(i).size() == 0)
            {
                myVector.erase(myVector.begin() + i);
            }

            i--;
        }

        std::cout << "velkost vektora je: " << myVector.size() << std::endl;
    }
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
    bool falseVar = true;

    int singleVarIndex = checkIfSingleVar(products);

    std::cout << "index: " <<  indexPoradia << std::endl;
    std::cout << "vector size: " <<  products.size() << std::endl;


    if (singleVarIndex != -1)
    {
        //poradie.erase(indexPoradia, 1); 

        //indexPoradia = poradie.find(products.at(singleVarIndex)[0] - NUMTOLETTER);

        std::cout << "nasla sa samotna premenna " << products.at(singleVarIndex) << std::endl;        

        NODE *node = createNode(products.at(singleVarIndex)[0]);

        if (products.at(singleVarIndex)[1] == '\'')
        {
            std::cout << "je negovana\n";
            node->low = trueNode;
            trueNodeUsed = true;

            //products.erase(products.begin() + singleVarIndex);
            deleteNegCharFrom(products, products.at(singleVarIndex)[0]);
            if (products.size() != 0)
            {         
                node->high = recMakeBdd(poradie, products, indexPoradia);
            }
            else
            {
                node->high = falseNode;
                falseNodeUsed = true;
            }

        }
        else
        {
            std::cout << "nie je negovana\n";
            node->high = trueNode;
            trueNodeUsed = true;
            
            //products.erase(products.begin() + singleVarIndex);
            std::cout << "vector z ktoreho sa maze je: ";
            printVector(products);
            std::cout << std::endl;          
            deleteCharFrom(products, products.at(singleVarIndex)[0]);
            std::cout << "ostal tam: ";
            printVector(products);
            if (products.size() != 0)
            {
                node->low = recMakeBdd(poradie, products, indexPoradia);
            }
            else
            {
                node->low = falseNode;
                falseNodeUsed = true;  
            }
        }

        return node;
    }
    
    std::size_t varIndex;
    
    std::vector<std::string> lowExpresion;
    std::vector<std::string> highExpresion;

    
    NODE *node = createNode(poradie[indexPoradia] + NUMTOLETTER);    

    //shannon expansion + rozdelenie co ide do low a co do high
    for (int i = 0; i < products.size(); i++)
    {        
        //printf("%s %d", products[i], poradie[indexPoradia]);
        varIndex = products.at(i).find(poradie[indexPoradia] + NUMTOLETTER);
        std::cout << "hlada: "<< poradie[indexPoradia] + NUMTOLETTER << "v: ";
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
            }else
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
    //std::string input = "ab+ac+bc";
    //std::string input = "ab+bc+a\'b\'c\'";
    //std::string input = "a\'b\'c+a\'bc\'+a\'bc+ab\'c\'+ab\'c+abc\'+abc";
    //std::string input = "ab+cd+ef+gh";

    std::string input = "a+b";
    std::string ciselka = "10";

    BDD *bdd = BDD_create(input, ciselka);
    
    std::cout << std::endl;
    test(bdd);

    std::cout << "\npozor kurvaa " << countVariables(input) << std::endl;
    std::cout << "pocet uzlov ty pico " << bdd->nodeCount << std::endl;
    std::cout << "prva premenna ty pico " << bdd->root->var << std::endl;
    //freeMem(bdd->root);

    return 0;
}

