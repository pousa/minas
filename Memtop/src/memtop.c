/*
 * src/memtop.c
 *
 * Author: Martinasso Maxime
 *
 * (C) Copyright 2007 INRIA 
 * Projet: MESCAL / ANR NUMASIS
 * 
 */

#include<stdio.h>
#include<stdlib.h>

#include <ncurses.h>
#include <numa.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<signal.h>

#include "memdata.h"
#include "memtop.h"

void signal_handler(int sig) {
	endwin();
	exit(0);
}

void human_readable(long size, char* buf) {
	char h =' ';
	long Mo = 1024*1024;
	double dbl_size = 0.0;

	if (size >= 1024 && size < Mo) {
		dbl_size = (double) size / 1024;
		h = 'K';
	} else {
		if (size >= Mo && size < 1024*Mo) {
			dbl_size = (double) size / Mo;
			h = 'M';
		} else {
			if (size >= 1024*Mo) {
				dbl_size = (double) size / (1024*Mo);
				h = 'G';
			} 
		}
	}
	sprintf(buf,"%5.1f%cb",dbl_size,h);
}

void makewindows() {

	top_ptrs = newwin(3,COLUMNS_PTRS,0,0);
	wattron(top_ptrs,A_BOLD);

	top_nodes = newwin(3,max_nodes*NODES_DISP_PADDING+2,4,0);
	wattron(top_nodes,A_BOLD);
	box(top_nodes,0,0);
	
	head_columns = newwin(3,COLUMNS_PTRS+max_nodes*NODES_DISP_PADDING+2,8,0);
	wattron(head_columns,A_BOLD);

	bottom_ptrs = newwin(MAX_ADDR_PROCESS,COLUMNS_PTRS,11,0);
	bottom_nodes = newwin(MAX_ADDR_PROCESS,max_nodes*NODES_DISP_PADDING,11,COLUMNS_PTRS);
	wrefresh(stdscr);

	static_display();
	clean_display();

}

void static_display() {
	char str_nodes[max_nodes*NODES_DISP_PADDING];
	char str_ptr[] = "Total Memory:";
	char str_head[] = "    PID  |  NAME    |  PHY.   |  VIRT.  |   MAPPING";
	char str_cmd[] = " 'q' to quit, 'p' to pause, 'c' to clear";

	int i;

	str_nodes[0]='\0';
	for(i = 0; i < max_nodes; ++i) {
		sprintf(str_nodes,"%s        %d       ",str_nodes,i);
	}
	mvwprintw(top_nodes,0,1,"%s",str_nodes);
	wnoutrefresh(top_nodes);
	
	mvwprintw(top_ptrs,0,0,"%s",str_ptr);
	wnoutrefresh(top_ptrs);
	
	mvwprintw(head_columns,0,0,"%s",str_cmd);
	mvwprintw(head_columns,2,0,"%s",str_head);
	wnoutrefresh(head_columns);
	doupdate();
}

void clean_display() {
	char data_nodes[max_nodes*NODES_DISP_PADDING];
	char str_ptr[COLUMNS_PTRS];
	int i;
	char mem_human[10];

	data_nodes[0]='\0';
	human_readable(0,mem_human);
	for(i = 0; i < max_nodes; ++i) {
		sprintf(data_nodes,"%s%5.1f%(%s) ",data_nodes,0.0,mem_human);
	}
	sprintf(str_ptr,"%5.1f% (%s)",mem_human);
	mvwprintw(top_ptrs,1,11,"%s",str_ptr);
	mvwprintw(top_nodes,1,1,"%s",data_nodes);
	
	wnoutrefresh(top_nodes);
	wnoutrefresh(top_ptrs);

	werase(bottom_ptrs);
	werase(bottom_nodes);
	wnoutrefresh(bottom_ptrs);
	wnoutrefresh(bottom_nodes);
	doupdate();
}

void display() {
	char data_nodes[max_nodes*NODES_DISP_PADDING];
	char global_nodes[max_nodes*NODES_DISP_PADDING];
	char str_data[max_nodes*NODES_DISP_PADDING];
	char str_ptr[COLUMNS_PTRS];
	int i,k;
	int pagesize = getpagesize();
	unsigned long data_size, local_size;
	unsigned long global_size = 0;
	double percent;
	int node_all[max_nodes];
	long long node_size[max_nodes];
	long long global_node_size = 0;
	char mem_human[10];
	char mem_human2[10];
	int count;

	for(i =0; i < max_nodes; ++i) {
		node_all[i] = 0;
		node_size[i]=numa_node_size(i,NULL);
		global_node_size += node_size[i];
	}

	count = 0;
	for(k = 0; k < idx_stack; ++k) {
		data_nodes[0]='\0';
		local_size = 0;
		for(i = 0; i < max_nodes; ++i) {
			data_size=(long long)stack_data[k].pages_node[i]*pagesize;
			global_size += data_size;
			local_size += data_size;
			percent =  ((double)(data_size*100)/node_size[i]);
			node_all[i] += stack_data[k].pages_node[i];
			human_readable(data_size,mem_human);
//			sprintf(data_nodes,"%s%5.1f%(%s) ",data_nodes,percent,mem_human);
		        sprintf(data_nodes,"%s(%s) ",data_nodes,mem_human);
		}
	
		str_data[0]='\0';	
		human_readable(local_size,mem_human);
		human_readable(stack_data[k].size,mem_human2);
		sprintf(str_data,"%8d | %8.8s | %s | %s |",stack_data[k].pid, stack_data[k].name, mem_human, mem_human2);

		mvwprintw(bottom_ptrs,count,0,"%s",str_data);
		mvwprintw(bottom_nodes,count,1,"%s",data_nodes);
		count++;
	}

	data_nodes[0]='\0';
	for(i = 0; i < max_nodes; ++i) {
		data_size=(long long)node_all[i]*pagesize;
		percent =  ((double)(data_size*100)/node_size[i]);
		human_readable(data_size,mem_human);
		sprintf(data_nodes,"%s%5.1f%(%s) ",data_nodes,percent,mem_human);
	}
	percent =  ((double)(global_size*100)/(global_node_size));
	human_readable(global_size,mem_human);
	sprintf(str_ptr,"%5.1f% (%s)",percent,mem_human);
	mvwprintw(top_nodes,1,1,"%s",data_nodes);
	mvwprintw(top_ptrs,1,11,"%s",str_ptr);
	wnoutrefresh(top_nodes);
	wnoutrefresh(top_ptrs);
	wnoutrefresh(bottom_ptrs);
	wnoutrefresh(bottom_nodes);
	doupdate();

}

void running() {
	struct stat stat_buf;
	ssize_t len;
	int size;
	char msg[255];
	char c;

	sprintf(msg,"%s%d",FIFO_PATH_NAME,getuid());
	if (stat(msg, &stat_buf) == 0) {
		if ((hr_fifo = open(msg,O_RDONLY|O_NONBLOCK)) == -1) {
			perror("open:");
		}
	} else {
		if(mkfifo(msg, 0666)) {
			printf ("Error creating fifo pipe\n");
		} else {
			if ((hr_fifo = open(msg,O_RDONLY|O_NONBLOCK)) == -1) {
				perror("open");
			}
		}
	}

	while(1) {
		c = getch();
		/* read data from pipe */
		idx_stack = 0;
		while((len = read(hr_fifo,&size,sizeof(int))) > 0 && size > 0) {
			len = read(hr_fifo, stack_data+idx_stack, size*sizeof(memdata_t));
			idx_stack += size;
		}
		if (idx_stack > 0) {
			clean_display();
			display();
		}
		if (c == 'q' || c == 'Q') {
			break;
		}
		if (c == 'c' || c == 'C') {
			clean_display();
		}
		if (c == 'p' || c == 'P') {
			c = getch();
			while(c = getch()) {
				if (c == 'c' || c == 'C') {
					clean_display();
				}
				if ( c == 'p' || c == 'P' || c == 'q' || c == 'Q') {
					break;
				}
			}
			if (c == 'q' || c == 'Q') {
				break;
			}
		}

	}
}

int main(int argc, char *argv[]) {
	struct sigaction sig_action;

	sig_action.sa_handler = signal_handler;
	sig_action.sa_flags = SA_RESTART;
	sigaction (SIGINT, &sig_action, NULL);

	max_nodes = numa_max_node()+1;
	initscr();
	start_color();
	noecho();
	nonl();
	cbreak();
	use_default_colors();
	nodelay(stdscr, TRUE);
	curs_set(0);

	makewindows();
	running();

	endwin();
	return EXIT_SUCCESS;
}



