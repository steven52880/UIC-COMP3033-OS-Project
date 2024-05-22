#pragma once
#include <stdio.h>

extern int statistic_visit_count;
extern int statistic_tlb_hit_count;
extern int statistic_pagefault_count;

void statistic_init();
void count_visit();
void count_tlb_hit();
void count_pagefault();
void statistic_print(FILE *file);