#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct student student;

struct student {
	char *name;
	struct list_elem elem;
};


void insert (struct list *student_list) {
	printf("%s\n","Vad heter studenten?");
	char str[64];
	scanf("%s",str);
	student *sd = malloc(sizeof(student));
	printf("%s", "allocd student\n");
	sd->name = malloc(sizeof(char[64]));
	printf("%s", "allocd std->name\n");
	strcpy(sd->name, str);
	list_push_back(student_list, &sd->elem);
	printf("added %s\n",str);
}

void delete (struct list *student_list) {
	printf("%s\n","Vad heter studenten?");
	char str[64];
	scanf("%s",str);
	struct list_elem *start = list_begin(student_list);
	student *std = list_entry(start, struct student, elem);
	printf("looking for: %s\n", str);
	while (*std->name != *str && start != list_tail(student_list)) {
		printf("checking: %s\n",std->name);
		start = list_next(start);
		std = list_entry(start, struct student, elem);
		printf("checking: %s\n",std->name);
	}
	if(*std->name == *str && start != list_tail(student_list)) {
		list_remove(start);
		printf("removed %s\n",std->name);
		free(std->name);
		printf("%s", "Freed std->name\n");
		free(std);
		printf("%s", "Freed std\n");
	}
	else {
		printf("Studenten finns inte i listan.");
	}
}

void list (struct list *student_list) {
	struct list_elem *start = list_begin(student_list);
	student *std = list_entry(start, struct student, elem);
	while (start != list_tail(student_list)) {
		printf("name_ %s\n",std->name);
		start = list_next(start);
		std = list_entry(start, struct student, elem);
	}
}

void quit (struct list *student_list) {
	struct list_elem *start = list_begin(student_list);
	student *std = list_entry(start, struct student, elem);
	while (start != list_tail(student_list)) {
		start = list_next(start);
		free(std->name);
		printf("%s", "Freed std->name\n");
		free(std);
		printf("%s", "Freed std\n");
		std = list_entry(start, struct student, elem);
	}
}

int main() {
	struct list student_list;
	list_init (&student_list);
	int opt;

	do {
		printf("Menu:\n");
		printf("1 - Insert student\n");
		printf("2 - Delete student\n");
		printf("3 - List students\n");
		printf("4 - Exit\n");
		scanf("%d", &opt);
		switch (opt) {
			case 1:
				{
					insert(&student_list);
					break;
				}
			case 2:
				{
					delete(&student_list);
					break;
				}
			case 3:
				{
					list(&student_list);
					break;
				}

			case 4:
				{
					quit(&student_list);
					break;
				}
			default:
				{
					printf("Quit? (1/0):\n");
					scanf("%d", &opt);
					if (opt)
						quit(&student_list);
					break;
				}
		}
	} while(1);

	return 0;
}
