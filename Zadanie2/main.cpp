#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>
#include <set>

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
                /*if (checkForCredibility(myVector, i))
                {
                    return i;
                }*/
                return i;            
            }
            
        }        
    }

    return -1;
}

NODE *createNode(char var)
{
    NODE* node = (NODE *)malloc(sizeof(NODE));
    node->var = var;

    node->low = NULL;
    node->high = NULL;

    nodeCount++;
    return node;
}

NODE *recMakeBdd(std::string poradie, std::vector<std::string> products, int indexPoradia)
{
    int singleVarIndex = checkIfSingleVar(products);

    if (singleVarIndex != -1)
    {
        std::cout << "nasla sa samotna premenna " << products.at(singleVarIndex) << std::endl;

        NODE *node = createNode(products.at(singleVarIndex)[0]);

        if (products.at(singleVarIndex)[1] == '\'')
        {
            std::cout << "je negovana\n";
            node->low = trueNode;

            products.erase(products.begin() + singleVarIndex);            
            node->high = recMakeBdd(poradie, products, indexPoradia);
        }
        else
        {
            std::cout << "nie je negovana\n";
            node->high = trueNode;
            
            products.erase(products.begin() + singleVarIndex);            
            node->low = recMakeBdd(poradie, products, indexPoradia);
        }
        

        return node;
    }
    
    std::size_t varIndex;
    
    std::vector<std::string> lowExpresion;
    std::vector<std::string> highExpresion;

    NODE *node = createNode(poradie[indexPoradia] + NUMTOLETTER);

    /*if (poradie[indexPoradia + 1] == '\0')
    {
        std::cout << "posledna premenna ";

        if (products.at(0)[1] == '\0')
        {
            node->low = trueNode;
            node->high = falseNode;
        }
        else
        {
            node->low = falseNode;
            node->high = trueNode;
        }

        return node;
    }    
        



        //najst index vectora, kde sa nachadza posledna premenna
        for (int i = 0; i < products.size(); i++)
        {
            
            std::cout << products.at(i) << "\n";
            if (products.at(i).size() <= 2)
            {
                if(products.at(i).size() == 2 && products.at(i)[1] == '\'')
                {
                    node->var = products.at(i)[0];
                    node->low = trueNode;
                    node->high = falseNode;

                    return node;
                }else
                {
                    node->var = products.at(i)[0];
                    node->low = falseNode;
                    node->high = trueNode;

                    return node;
                }
            }        
        }*/
    

    //shannon expansion + rozdelenie co ide do low a co do high
    for (int i = 0; i < products.size(); i++)
    {        
        //printf("%s %d", products[i], poradie[indexPoradia]);
        varIndex = products.at(i).find(poradie[indexPoradia] + NUMTOLETTER);
        //printf("%d", varIndex);

        if (varIndex != -1)
        {
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

    /*//print po shannon expansion
    std::cout << lowExpresion.size() << highExpresion.size() << std::endl;
    std::cout << "low" << std::endl;
    for (int i = 0; i < lowExpresion.size(); i++)
    {
        std::cout << lowExpresion.at(i) << "\n";
    }

    printf("-\n");

    std::cout << "high" << std::endl;
    for (int i = 0; i < highExpresion.size(); i++)
    {
        std::cout << highExpresion.at(i) << "\n";
    }*/    

    printf("///////////////////////////////////////////\n");
    if (lowExpresion.size() != 0)
    {
        std::cout << "dalsia uroven pre low" << indexPoradia + 1 << " velkost je" << lowExpresion.size() << std::endl;
        printVector(lowExpresion);
        node->low = recMakeBdd(poradie, lowExpresion, indexPoradia + 1);
    }else
    {
        node->low = falseNode;
    }
    
    
    if (highExpresion.size() != 0)
    {
        std::cout << "dalsia uroven pre high" << indexPoradia + 1 << " velkost je" << highExpresion.size() << std::endl;
        printVector(highExpresion);
        node->high = recMakeBdd(poradie, highExpresion, indexPoradia + 1);
    }else
    {
        node->high = falseNode;
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
char BDD_use (BDD *bdd, char *vstupy)
{
    NODE *node = bdd->root;
    int index = 0;
    char variable;

    while (node != trueNode && node != falseNode)
    {
        variable = vstupy[node->var - CHARTOINDEX];
        
        if (variable == '\0')
        {
            printf("chyba\n");
            exit(0);
        }
        

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
    
    std::cout << "chyba\n";
    exit(0);
    //return node == trueNode ? '1' : '0';
}

void test(BDD *bdd)
{   
    char vstupy[3];

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                vstupy[0] = 48 + i;
                vstupy[1] = 48 + j;
                vstupy[2] = 48 + k;

                std::cout << BDD_use(bdd, vstupy) << std::endl;
            }
        }
    }
}

/*
uz to vie vytvorit neredukovany bdd
uz to vie dat koncove uzly aj ku inej premennej ako poslednej - to je dobre, tak to ma byt - !!!nemenit
uz to redukuje koncove uzly
*/

//TODO doplnit aby vsetky nodes dostali true alebo false, aj ked vetva skonci skor ako pri poslednej premennej
int main ()
{
    InitLeafNodes();

    //pre tieto vstup to fahci...paraada
    //std::string input = "a\'b\'c+a\'bc\'+a\'bc+ab\'c\'+ab\'c+abc\'+abc";
    //std::string input = "ab\'c\'+abc+a\'bc\'+a\'b\'c";

    std::string input = "ab+bc+a\'b\'c\'";


    std::string ciselka = "012";

    BDD *bdd = BDD_create(input, ciselka);
    
    test(bdd);

    std::cout << "\npozor kurvaa " << countVariables(input) << std::endl;
    std::cout << "pocet uzlov ty pico " << bdd->nodeCount << std::endl;
    std::cout << "prva premenna ty pico " << bdd->root->var << std::endl;
    //std::cout << BDD_use(bdd, "11");


    return 0;
}

