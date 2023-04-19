#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <string>

#include "Chaining.h"

typedef struct hashNode
{
    struct hashNode *next;
    std::vector<std::string> expresion;
    NODE *node;
}HASHNODE;

static int tableSize = 2;
static int nodeCount = 0;

static HASHNODE **globalTable;

static void TableInit(HASHNODE ***table)
{
    *table = (HASHNODE **)calloc(tableSize, sizeof(HASHNODE *));

    if (!*table)
    {
        printf("Table Out of memory\n");
        exit(1);
    }
}

static HASHNODE *CreateNode(std::vector<std::string> expresion, NODE *node)
{
    HASHNODE *element = new HASHNODE;
    
    if (!element)
    {
        printf("Out of memory\n");
        exit(1);
    }

    element->next = NULL;
    element->expresion = expresion;
    element->node = node;

    return element;
}

//polynomialna akumulacia
static int HashFunction(std::vector<std::string> expresion)
{
    int i = 0;
    unsigned long long hash = 0;


    for (int i = 0; i < expresion.size(); i++)
    {
        for (int j = 0; j < expresion.at(i).size(); j++)
        {
            hash = hash * 33 + expresion.at(i)[j];
        }
    }

    return hash % tableSize;
}

static HASHNODE *SearchNode(HASHNODE **table, std::vector<std::string> expresion)
{
    int index = HashFunction(expresion);
    HASHNODE *node;

    if (table[index])
    {
        node = table[index];
        while (node && node->expresion != expresion)
        {
            node = node->next;
        }

        if (node)
        {
            return node;
        }
    }

    return NULL;
}

static int AlreadyInTable(HASHNODE **table, HASHNODE *node)
{
    if (SearchNode(table, node->expresion) != NULL)
    {
        return 1;
    }
    return 0;
}

static void InsertToTable(HASHNODE ***table, HASHNODE *node, int index)
{
    node->next = (*table)[index];

    (*table)[index] = node;

    nodeCount++;
}

static void ChangeTableSize(HASHNODE ***table)
{
    HASHNODE *node;
    HASHNODE *temp;
    HASHNODE **newTable;
    int index;
    int oldTableSize = tableSize;
    int oldNodeCount = nodeCount;
    double loadFactor = (double)nodeCount / (double)tableSize;

    if (loadFactor >= 0.75)
    {
        tableSize *= 2;
    }
    else if (loadFactor <= 0.25)
    {
        tableSize /= 2;
    }
    else
    {
        return;
    }

    TableInit(&newTable);
      

    for (int i = 0; i < oldTableSize; i++)
    {
        node = (*table)[i];

        while (node)
        {
            temp = node->next;
            index = HashFunction(node->expresion);
            InsertToTable(&newTable, node, index);
            node = temp;
        }
    }
    free(*table);

    *table = newTable;
    nodeCount = oldNodeCount;
}

static void Insert(HASHNODE ***table, std::vector<std::string> expresion, NODE *node)
{
    HASHNODE *element = CreateNode(expresion, node);
    int index = HashFunction(expresion);

    if (AlreadyInTable(*table, element))
    {
        free(element);
        return;
    }
    
    InsertToTable(table, element, index);

    ChangeTableSize(table);
}

static void FreeMem(HASHNODE ***table)
{
    for (int i = 0; i < tableSize; i++)
    {
        while ((*table)[i] != NULL)
        {
            HASHNODE *temp = (*table)[i];
            (*table)[i] = (*table)[i]->next;
            
            free(temp->node);
            free(temp);
            temp = NULL;
        }
    }

    free(*table);
    *table = NULL;

    tableSize = 2;
    nodeCount = 0;
}

void CHT_Init()
{
    TableInit(&globalTable);
}

void CHT_Insert(std::vector<std::string> expresion, NODE *node)
{
    Insert(&globalTable, expresion, node);
}

NODE *CHT_Search(std::vector<std::string> expresion)
{
    HASHNODE *element = SearchNode(globalTable, expresion);

    return element != NULL ? element->node : NULL;
}

void CHT_FreeMem()
{
    FreeMem(&globalTable);
}

static void specialPrint(std::vector<std::string> myVector){
    for (int i = 0; i < myVector.size(); i++)
    {
        std::cout << myVector.at(i)<< ", ";
    }

    std::cout << "\n";
    
}

void visualize()
{
    std::cout << "velkost tabulky je: " << tableSize << std::endl; 
    for (int i = 0; i < tableSize; i++)
    {
        HASHNODE *node = globalTable[i];
        if(node)
            std::cout << node->expresion.size() << ' ';
        while (node != NULL)
        {
            specialPrint(node->expresion);
            printf("\n");

            node = node->next;
        }
        
    }
    
}
