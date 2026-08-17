#pragma once

int Clamp(int value,int low, int high);
int selfHealth(int playerHealth);
int selfHealth(int playerhealth, int maxHP);
void PrintMenu();
bool IsDead(int hp);
int ReadChoice();
double RollDamage(int enDef, int playeratta, double variance);
int ApplyDamage(int damage, int EnHP);
void PrintStatus(int damage, int CurrentEnHP, int enHP);