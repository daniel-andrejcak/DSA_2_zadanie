#include "bdd.h"

void CHT_Init();
void CHT_Insert(std::vector<std::string> expresion, NODE *node);
NODE *CHT_Search(std::vector<std::string> expresion);
void CHT_FreeMem();
void visualize();