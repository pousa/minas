/*
 * include/memtop.h
 *
 * Author: Martinasso Maxime
 *
 * (C) Copyright 2007 INRIA 
 * Projet: MESCAL / ANR NUMASIS
 * 
 */

#ifndef __MEMTOP_H
#define __MEMTOP_H

#define NODES_DISP_PADDING 16
#define COLUMNS_PTRS 41                                                                 
#define MAX_STACK 512

static int hr_fifo;
static int max_nodes = 0;
static memdata_t stack_data[MAX_ADDR_PROCESS];
static int idx_stack = 0;


static WINDOW *top_nodes;
static WINDOW *bottom_nodes;
static WINDOW *top_ptrs;
static WINDOW *bottom_ptrs;
static WINDOW *head_columns;

void clean_display();
void display();
void static_display();
void running();
void makewindows();

#endif
