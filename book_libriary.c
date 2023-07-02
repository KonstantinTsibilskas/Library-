#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <locale.h>
#include <Windows.h>
#include <conio.h>

#define TRUE 1
#define FALSE 0
#define MAXFILENAMELEN 64
#define CURRENT_YEAR 2022
#define MAX_DATA_SIZE 256
#define MAX_VALUE_FOR_ATOI 10
typedef unsigned char uchar;

enum Param
{
	// Параметр, по которому сортируется дерево
	ID,
	TITLE,
	AUTH,
	YEAR,
	PUBL,
	RATE,
	COST,
	GENRE
};


/*
Структура для хранения данных об 1 книге
*/

typedef struct Tree {

	//int id; // порядковый номер (для начальной сортировки)
	unsigned char* title; // название
	unsigned char* auth; // автор(авторы)
	int year; // год выпуска
	unsigned char* publ; // издательство
	int rate; // оценка читателей
	int cost; // цена
	unsigned char* genre; // жанр
	unsigned char* desc; // краткое описание
	int height;           //Высота АВЛ-дерева
	struct Tree* left; // указатели на левое и правое поддерево
	struct Tree* right;

} TTree;

void Insert(TTree** Tree,/* TTree *InTree*/enum Param param, unsigned char* title, unsigned char* auth, int year, unsigned char* publ, int rate, int cost, unsigned char* genre, unsigned char* desc);
TTree* Delete(TTree* tree, unsigned char* title);
int f_FIND(TTree** title_tree, TTree** auth_tree, TTree** year_tree, TTree** publ_tree, TTree** rate_tree, TTree** cost_tree, TTree** genre_tree);
TTree* f_SORT(TTree** title_tree, TTree** auth_tree, TTree** year_tree, TTree** publ_tree, TTree** rate_tree, TTree** cost_tree, TTree** genre_tree);
int f_REMOVE(TTree** title_tree, TTree** auth_tree, TTree** year_tree, TTree** publ_tree, TTree** rate_tree, TTree** cost_tree, TTree** genre_tree);
int f_EDIT(TTree** tree_title, TTree** tree_auth, TTree** tree_year, TTree** tree_publ, TTree** tree_rate, TTree** tree_cost, TTree** tree_genre);
int f_PRINT(TTree* tree);
int loading_from_file(TTree** tree_title, TTree** tree_auth, TTree** tree_year, TTree** tree_publ, TTree** tree_rate, TTree** tree_cost, TTree** tree_genre, \
	const char* input_name, int level);

//TTree* rotate_rihgt(TTree* tree)


void usage(const char* program_name) {
	fprintf(stdout, "\t\tИспользуйте %s -h/--help для вывода справки о работе программы\n\
		Используйте %s -i/--input <путь к входному файлу> для передечи пути к файлу базы данных\n\
		Используйте %s -p/--print для вывода информации о всех имеющихся книгах\n\
		Используйте %s -a/--add для добавления книги\n\
		Используйте %s -r/--remove для удаления книги\n\
		Используйте %s -e/--edit для редактирования книги\n\
		Используйте %s -f/--find для поиска книги по определенному критерию/критериям (критерии выбираются)\n\
		Используйте %s -s/--sort для сортировки книг по заданному полю\n\
		Используйте %s -o/--output <путь к выходному файлу> для передачи пути к выходному файлу базы данных\n", program_name, program_name, program_name, program_name, program_name, program_name, program_name, program_name, program_name);
}
/*
Определяет тип действия, в том случае, если задано действие
и как минимум одно имя файла
*/
typedef enum ONE_ACTION_TYPE
{
	HELP = -1,
	ADD, // добавление
	FIND, // поиск
	SORT, // сортировка
	REMOVE, // удаление
	EDIT, // редактирование
	PRINT // печать
}eAType;


/*
Какое значение вернет функция TakeKeysFromConsole
0 - ошибка обработки ключей
1 - выполняем 1 действие, например вставка в дерево
2 - задано имя ВХОДНОГО файла + действие
3 - задано имя ВЫХОДНОГО файла + действие
4 - задано имя ВХОДНОГО и ВЫХОДНОГО файлов + действие
*/

typedef enum CONSOLE_LOG {
	CONSOLE_LOG_ERROR,
	CONSOLE_LOG_HELP,
	CONSOLE_LOG_ACTION,
	CONSOLE_LOG_IN_ACTION,
	CONSOLE_LOG_OUT_ACTION,
	CONSOLE_LOG_IN_OUT_ACTION
}eCLog;


eAType FindCommandFromConsole(char* cmd) {

	if (strcmp(cmd, "-a") == 0 || strcmp(cmd, "--add") == 0)
	{
		return ADD;
	}
	if (strcmp(cmd, "-f") == 0 || strcmp(cmd, "--find") == 0)
	{
		return FIND;
	}
	if (strcmp(cmd, "-s") == 0 || strcmp(cmd, "--sort") == 0)
	{
		return SORT;
	}
	if (strcmp(cmd, "-r") == 0 || strcmp(cmd, "--remove") == 0)
	{
		return REMOVE;
	}
	if (strcmp(cmd, "-e") == 0 || strcmp(cmd, "--edit") == 0)
	{
		return EDIT;
	}
	if (strcmp(cmd, "-p") == 0 || strcmp(cmd, "--print") == 0)
	{
		return PRINT;
	}

	return -2;

}


eCLog GetKeysFromConsole(int argc, char* argv[], eAType* etype, char* InputFileName, char* OutputFileName) {
	/*int IsFound1;
	int isFound2;*/

	if (argc < 2)
	{
		return CONSOLE_LOG_ERROR;
	}
	else if (argc == 2)
	{
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
		{
			usage(argv[0]);
			return CONSOLE_LOG_HELP;
		}
		else if (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--add") == 0)
		{
			// выполняем добавление книги в дерево
			(*etype) = ADD;
			return CONSOLE_LOG_ACTION;
		}
		else if (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "--find") == 0)
		{
			// выполняем поиск книги
			(*etype) = FIND;
			return CONSOLE_LOG_ACTION;
		}
		else if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--sort") == 0)
		{
			// выполняем сортировку дерева по опр параметру
			(*etype) = SORT;
			return CONSOLE_LOG_ACTION;
		}
		else if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--remove") == 0)
		{
			// выполняем удаление книги из дерева
			(*etype) = REMOVE;
			return CONSOLE_LOG_ACTION;
		}
		else if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--edit") == 0)
		{
			// выполняем изменение книги в библиотеке (то есть, сначала поиск, потом изменение)
			(*etype) = EDIT;
			return CONSOLE_LOG_ACTION;
		}
		else if (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--print") == 0)
		{
			// печатаем информацию о книгах
			(*etype) = PRINT;
			return CONSOLE_LOG_ACTION;
		}
		else {
			usage(argv[0]);
			return CONSOLE_LOG_HELP;
		}
	}
	else if (argc == 4) {

		int IsTotal = FALSE;
		int IsAction = FALSE;
		int IsInput = FALSE;
		int IsOutput = FALSE;

		for (int i = 1; i < 4; ++i)
		{
			eAType cmd = FindCommandFromConsole(argv[i]);

			if (cmd >= 0 && cmd <= 5 && !IsAction) // команда для действия нашлась
			{
				IsAction = TRUE; // запоминаем, что команда нашлась
				(*etype) = cmd; // запоминаем команду
				continue;
			}
			else if ((strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) && !IsOutput) {
				// если команда не нашлась -> ищем файл (входной или выходной)
				IsInput = TRUE;
				strcpy(InputFileName, argv[i + 1]);
				i++;
				continue;
			}
			else if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) && !IsInput)
			{
				IsOutput = TRUE;
				strcpy(OutputFileName, argv[i + 1]);
				i++;
				continue;
			}
			else {
				IsTotal = FALSE;
				return CONSOLE_LOG_ERROR;
			}
		}
		IsTotal = (IsAction) && (IsInput || IsOutput);

		if (IsTotal && IsInput)
		{
			return CONSOLE_LOG_IN_ACTION;
		}
		if (IsTotal && IsOutput)
		{
			return CONSOLE_LOG_OUT_ACTION;
		}
	}
	else if (argc == 6)
	{
		int IsTotal = FALSE;
		int IsAction = FALSE;
		int IsInput = FALSE;
		int IsOutput = FALSE;

		for (int i = 1; i < 6; ++i)
		{
			eAType cmd = FindCommandFromConsole(argv[i]);

			if (cmd >= 0 && cmd <= 5) // команда для действия нашлась
			{
				IsAction = TRUE; // запоминаем, что команда нашлась
				(*etype) = cmd; // запоминаем команду
				continue;
			}
			else if ((strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0)) {
				// если команда не нашлась -> ищем файл (входной или выходной)
				IsInput = TRUE;
				strcpy(InputFileName, argv[i + 1]);
				i++;
				continue;
			}
			else if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0))
			{
				IsOutput = TRUE;
				strcpy(OutputFileName, argv[i + 1]);
				i++;
				continue;
			}
			else {
				IsTotal = FALSE;
				return CONSOLE_LOG_ERROR;
			}
		}

		IsTotal = IsAction && IsInput && IsOutput;

		if (IsTotal)
		{
			return CONSOLE_LOG_IN_OUT_ACTION;
		}
		else
		{
			return CONSOLE_LOG_ERROR;
		}
	}
	return CONSOLE_LOG_ERROR;
}



/*
Выводит информацию о книге на экран
*/
void print_tree(TTree* tree) {

	
	// выводим на экран элемент структуры (те информацию о книге)
	printf("\n---------------------------------------------------------------------------------------------\n");
	printf("Название книги: %s\n", tree->title);
	printf("Автор(Авторы): %s\n", tree->auth);
	printf("Год выпуска: %d\n", tree->year);
	printf("Издательство: %s\n", tree->publ);
	printf("Оценка читателей: %d\b\n", tree->rate);
	printf("Цена: %d\n", tree->cost);
	printf("Жанр: %s\n", tree->genre);
	printf("Краткое описание: %s\n", tree->desc);
	printf("\n---------------------------------------------------------------------------------------------\n");

}


int g_print_tree_new_it = 0;

void print_tree_new(TTree* tree, int level) {

	if (tree)
	{
		print_tree_new(tree->left, level + 1);
		g_print_tree_new_it++;
		// выводим на экран элемент структуры (те информацию о книге)
		printf("\n---------------------------------------------------------------------------------------------\n");
		printf("Название книги: %s\n", tree->title);
		printf("Автор(Авторы): %s\n", tree->auth);
		printf("Год выпуска: %d\n", tree->year);
		printf("Издательство: %s\n", tree->publ);
		printf("Оценка читателей: %d\b\n", tree->rate);
		printf("Цена: %d\n", tree->cost);
		printf("Жанр: %s\n", tree->genre);
		printf("Краткое описание: %s\n", tree->desc);
		printf("\n---------------------------------------------------------------------------------------------\n");
		print_tree_new(tree->right, level + 1);
	}
}






TTree* title_search(TTree** tree, unsigned char* title) {

	if (*tree != NULL)
	{
		if (strcmp((const char *)(*tree)->title, (const char*)title) == 0)
		{
			return *tree; // книга найдена
		}
		else if (strcmp((const char*)(*tree)->title, (const char*)title) > 0 && (*tree)->left != NULL)
		{
			title_search(&((*tree)->left), title); // идем в левое поддерево
		}
		else if (strcmp((const char*)(*tree)->title, (const char*)title) < 0 && (*tree)->right != NULL)
		{
			title_search(&((*tree)->right), title); // в правое поддерево
		}
	}
	else
	{
		return NULL; // книга не найдена
	}
	return* tree;
}


TTree* auth_search(TTree** tree, unsigned char* auth) {

	if (*tree != NULL)
	{
		if (strcmp((const char*)(*tree)->auth, (const char*)auth) == 0)
		{
			return *tree; // книга найдена
		}
		else if (strcmp((const char*)(*tree)->auth, (const char*)auth) > 0 && (*tree)->left != NULL)
		{
			auth_search(&((*tree)->left), auth); // идем в левое поддерево
		}
		else if (strcmp((const char*)(*tree)->auth, (const char*)auth) < 0 && (*tree)->right != NULL)
		{
			auth_search(&((*tree)->right), auth); // в правое поддерево
		}
	}
	else
	{
		return NULL; // книга не найдена
	}
	return* tree;
}


TTree* year_search(TTree** tree, int year) {

	if (*tree != NULL)
	{
		if ((*tree)->year == year)
		{
			return *tree; // книга найдена
		}
		else if (year < (*tree)->year && (*tree)->left != NULL)
		{
			year_search(&((*tree)->left), year); // идем в левое поддерево
		}
		else if (year > (*tree)->year && (*tree)->right != NULL)
		{
			year_search(&((*tree)->right), year); // в правое поддерево
		}
	}
	else
	{
		return NULL; // книга не найдена
	}
	return *tree;
}


TTree* publ_search(TTree** tree, unsigned char* publ) {

	if (*tree != NULL)
	{
		if (strcmp((const char*)(*tree)->publ, (const char*)publ) == 0)
		{
			return *tree; // книга найдена
		}
		else if (strcmp((const char*)(*tree)->publ, (const char*)publ) > 0 && (*tree)->left != NULL)
		{
			publ_search(&((*tree)->left), publ); // идем в левое поддерево
		}
		else if (strcmp((const char*)(*tree)->publ, (const char*)publ) < 0 && (*tree)->right != NULL)
		{
			publ_search(&((*tree)->right), publ); // в правое поддерево
		}
	}
	else
	{
		return NULL; // книга не найдена
	}
	return *tree;
}


TTree* rate_search(TTree** tree, int rate) {

	if (*tree != NULL)
	{
		if ((*tree)->rate == rate)
		{
			return *tree; // книга найдена
		}
		else if (rate < (*tree)->rate && (*tree)->left != NULL)
		{
			rate_search(&((*tree)->left), rate); // идем в левое поддерево
		}
		else if (rate > (*tree)->rate && (*tree)->right != NULL)
		{
			rate_search(&((*tree)->right), rate); // в правое поддерево
		}
	}
	else
	{
		return NULL; // книга не найдена
	}
	return *tree;
}



TTree* cost_search(TTree** tree, int cost) {

	if (*tree != NULL)
	{
		if ((*tree)->cost == cost)
		{
			return *tree; // книга найдена
		}
		else if (cost < (*tree)->cost && (*tree)->left != NULL)
		{
			cost_search(&((*tree)->left), cost); // идем в левое поддерево
		}
		else if (cost > (*tree)->cost && (*tree)->right != NULL)
		{
			cost_search(&((*tree)->right), cost); // в правое поддерево
		}
	}
	else
	{
		return NULL; // книга не найдена
	}
	return *tree;
}



TTree* genre_search(TTree** tree, unsigned char* genre) {

	if (*tree != NULL)
	{
		if (strcmp((const char*)(*tree)->genre, (const char*)genre) == 0)
		{
			return *tree; // книга найдена
		}
		else if (strcmp((const char*)(*tree)->genre, (const char*)genre) > 0 && (*tree)->left != NULL)
		{
			genre_search(&((*tree)->left), genre); // идем в левое поддерево
		}
		else if (strcmp((const char*)(*tree)->genre, (const char*)genre) < 0 && (*tree)->right != NULL)
		{
			genre_search(&((*tree)->right), genre); // в правое поддерево
		}
	}
	else
	{
		return NULL; // книга не найдена
	}
	return *tree;
}


void title_search_print(TTree** tree, unsigned char* title, int* counter) {

	if (tree != NULL)
	{
		if (strcmp((const char*)(*tree)->title, (const char*)title) == 0)
		{
			print_tree(*tree); // книга найдена
			(*counter)++;
		}

		if ((*tree)->left != NULL) title_search_print(&((*tree)->left), title, counter); // идем в левое поддерево

		if ((*tree)->right != NULL) title_search_print(&((*tree)->right), title, counter); // в правое поддерево
	}
}


void auth_search_print(TTree** tree, unsigned char* auth, int* counter) {

	if (tree != NULL)
	{
		if (strcmp((const char*)(*tree)->auth, (const char*)auth) == 0)
		{
			print_tree(*tree); // книга найдена
			(*counter)++;
		}

		if ((*tree)->left != NULL) auth_search_print(&((*tree)->left), auth, counter); // идем в левое поддерево

		if ((*tree)->right != NULL) auth_search_print(&((*tree)->right), auth, counter); // в правое поддерево
	}
}


void year_search_print(TTree** tree, int year, int* counter) {

	if (tree != NULL)
	{
		if ((*tree)->year == year)
		{
			print_tree(*tree); // книга найдена
			(*counter)++;
		}

		if ((*tree)->left != NULL) year_search_print(&((*tree)->left), year, counter); // идем в левое поддерево

		if ((*tree)->right != NULL) year_search_print(&((*tree)->right), year, counter); // в правое поддерево
	}
}



void publ_search_print(TTree** tree,unsigned char* publ, int* counter) {

	if (tree != NULL)
	{
		if (strcmp((const char*)(*tree)->publ, (const char*)publ) == 0)
		{
			print_tree(*tree); // книга найдена
			(*counter)++;
		}

		if ((*tree)->left != NULL) publ_search_print(&((*tree)->left), publ, counter); // идем в левое поддерево

		if ((*tree)->right != NULL) publ_search_print(&((*tree)->right), publ, counter); // в правое поддерево
	}
}


void rate_search_print(TTree** tree, int rate, int* counter) {

	if (tree != NULL)
	{
		if ((*tree)->rate == rate)
		{
			print_tree(*tree); // книга найдена
			(*counter)++;
		}

		if ((*tree)->left != NULL) rate_search_print(&((*tree)->left), rate, counter); // идем в левое поддерево

		if ((*tree)->right != NULL) rate_search_print(&((*tree)->right), rate, counter); // в правое поддерево
	}
}



void cost_search_print(TTree** tree, int cost, int* counter) {

	if (tree != NULL)
	{
		if ((*tree)->cost == cost)
		{
			print_tree(*tree); // книга найдена
			(*counter)++;
		}

		if ((*tree)->left != NULL) cost_search_print(&((*tree)->left), cost, counter); // идем в левое поддерево

		if ((*tree)->right != NULL) cost_search_print(&((*tree)->right), cost, counter); // в правое поддерево
	}
}


void genre_search_print(TTree** tree, unsigned char* genre, int* counter) {

	if (tree != NULL)
	{
		if (strcmp((const char*)(*tree)->genre, (const char*)genre) == 0)
		{
			print_tree(*tree); // книга найдена
			(*counter)++;
		}

		if ((*tree)->left != NULL) genre_search_print(&((*tree)->left), genre, counter); // идем в левое поддерево

		if ((*tree)->right != NULL) genre_search_print(&((*tree)->right), genre, counter); // в правое поддерево
	}
}


int i = 0;
int Addition(TTree** tree_title, TTree** tree_auth, TTree** tree_year, TTree** tree_publ, TTree** tree_rate, TTree** tree_cost, TTree** tree_genre, \
	unsigned char* title, unsigned char* auth, int* year, unsigned char* publ, int* rate, int* cost, unsigned char* genre, unsigned char* desc, int scanf_param) {                                           //Добавление книги  
	printf("\n---------------------------------------------------------------------------------------------\n");
	printf("Введите данные книги\n");

	fflush(stdin);
	fflush(stdout);

	printf("Название книги: ");

	memset(title, 0, MAX_DATA_SIZE);
	 char not_correct[100];
	if (scanf_param != 1) {
		gets_s(not_correct, 100);
	}
	/*if (i != 0)
	{
		gets_s(not_correct, 100);
	}*/
	i++;
	int counter;
	if ((counter = scanf("%[^\n]s", title)) != 1)
	{
		return EXIT_FAILURE;
	}
	if ((title[0] == ' ' && strlen((const char *)title) == 1))
	{
		printf("Введены неккоректные данные!\n");
		return EXIT_FAILURE;
	}
	fflush(stdin);

	gets_s(not_correct, 100);

	fflush(stdin);

	printf("\nАвтор: ");
	memset(auth, 0, MAX_DATA_SIZE);
	if (scanf("%[^\n]s", auth) != 1)
	{
		return EXIT_FAILURE;
	}
	if ((auth[0] == ' ' && strlen((const char*)auth) == 1))
	{
		printf("Введены неккоректные данные!\n");
		return EXIT_FAILURE;
	}
	fflush(stdin);

	printf("\nГод выпуска: ");
	if ((counter = scanf("%d", year)) != 1)
	{
		return EXIT_FAILURE;
	}
	if (*year > CURRENT_YEAR || *year < 0)
	{
		printf("Введены неккоректные данные!\n");
		return EXIT_FAILURE;
	}
	fflush(stdin);

	gets_s(not_correct, 100);

	fflush(stdin);

	printf("\nИздательство: ");
	memset(publ, 0, MAX_DATA_SIZE);
	if ((counter = scanf("%[^\n]s", publ)) != 1)
	{
		return EXIT_FAILURE;
	}
	if ((publ[0] == ' ' && strlen((const char*)publ) == 1))
	{
		printf("Введены неккоректные данные!\n");
		return EXIT_FAILURE;
	}
	fflush(stdin);

	printf("\nОценка читателей: ");
	if (scanf("%d", rate) != 1)
	{
		return EXIT_FAILURE;
	}
	if (*rate < 0)
	{
		printf("Введены неккоректные данные!\n");
		return EXIT_FAILURE;
	}
	fflush(stdin);

	printf("\nЦена: ");
	if (scanf("%d", cost) != 1)
	{
		return EXIT_FAILURE;
	}
	if (*cost < 0)
	{
		printf("Введены неккоректные данные!\n");
		return EXIT_FAILURE;
	}
	fflush(stdin);

	printf("\nЖанр: ");

	memset(genre, 0, MAX_DATA_SIZE);
	gets_s(not_correct, 100);

	fflush(stdin);

	if (scanf("%[^\n]s", genre) < 1)
	{
		return EXIT_FAILURE;
	}
	if ((genre[0] == ' ' && strlen((const char*)genre) == 1))
	{
		printf("Введены неккоректные данные!\n");
		return EXIT_FAILURE;
	}
	fflush(stdin);

	printf("\nКраткое описание: ");

	gets_s(not_correct, 100);

	fflush(stdin);

	memset(desc, 0, 1024);
	if (scanf("%[^\n]s", desc) < 1)
	{
		return EXIT_FAILURE;
	}
	if ((desc[0] == ' ' && strlen((const char*)desc) == 1))
	{
		printf("Введены неккоректные данные!\n");
		return EXIT_FAILURE;
	}
	fflush(stdin);
	fflush(stdout);

	printf("\n---------------------------------------------------------------------------------------------\n");

	Insert(tree_title, TITLE, title, auth, *year, publ, *rate, *cost, genre, desc);
	Insert(tree_auth, AUTH, title, auth, *year, publ, *rate, *cost, genre, desc);
	Insert(tree_year, YEAR, title, auth, *year, publ, *rate, *cost, genre, desc);
	Insert(tree_publ, PUBL, title, auth, *year, publ, *rate, *cost, genre, desc);
	Insert(tree_rate, RATE, title, auth, *year, publ, *rate, *cost, genre, desc);
	Insert(tree_cost, COST, title, auth, *year, publ, *rate, *cost, genre, desc);
	Insert(tree_genre, GENRE, title, auth, *year, publ, *rate, *cost, genre, desc);

	return EXIT_SUCCESS;
}


void redact_B(TTree** title_tree, TTree** auth_tree, TTree** year_tree, TTree** publ_tree, TTree** rate_tree, TTree** cost_tree, TTree** genre_tree, unsigned char* title) {

	int icounter = 0;
	TTree* temp = (TTree*)malloc(sizeof(TTree));
	TTree* temp1 = (TTree*)malloc(sizeof(TTree));
	if (temp == NULL || temp1 == NULL) return;

	//// Ищем книгу с названием title
	
	temp = title_search(title_tree, title);
	if (temp == NULL)
	{
		printf("Книга не найдена");
		return;
	}

	// сюда присваиваем его (в temp)

	int kolvo;
	printf("\nВведите количество параметров которые хотите отредактировать: ");
	if (scanf("%d", &kolvo) != 1) {
		return;
	}
	 char not_correct[100];
	unsigned char* title1 = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
	memset(title1, 0, MAX_DATA_SIZE);
	unsigned char* auth = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
	memset(auth, 0, MAX_DATA_SIZE);
	int year;
	unsigned char* publ = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
	memset(publ, 0, MAX_DATA_SIZE);
	int rate;
	int cost;
	unsigned char* genre = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
	memset(genre, 0, MAX_DATA_SIZE);
	unsigned char* desc = (unsigned char*)malloc(sizeof(unsigned char) * 1024);
	memset(desc, 0, 1024);
	if (title == NULL || auth == NULL || publ == NULL || genre == NULL || desc == NULL) return;
	if (title1[0] == '\0')
	{
		strcpy((char*)title1, (char*)title);
	}
	strcpy((char*)auth, (char*)(*title_tree)->auth);
	strcpy((char*)publ, (char*)(*title_tree)->publ);
	strcpy((char*)genre, (char*)(*title_tree)->genre);
	year = (*title_tree)->year;
	rate = (*title_tree)->rate;
	cost = (*title_tree)->cost;
	strcpy((char*)desc, (char*)(*title_tree)->desc);
	int action;
	for (int coun= 0; coun < kolvo; coun++)
	{
		printf("\nЕсли хотите отредактировать, нажмите\n\t 1.Название\n\t2.Авторы\n\t3.Год выпуска\n\t4.Издательство\n\t5.Оценка\n\t6.Цена\n\t7.Жанр\n\t8.Краткое описание\n");
		if (scanf("%d", &action) != 1 && action < 1 && action > 8)
		{
			return;
		}
		else
		{
			if (action == TITLE)
			{
				//printf("")-------------------------------------------------------------------------------------------------------------------------------------------------------------

				gets_s(not_correct, 100);
				printf("Название: ");
				icounter = scanf("%[^\n]s", title1);
				if ((title[0] == ' ' && strlen((const char*)title) == 1))
				{
					printf("Введены неккоректные данные!\n");
					return;
				}
			}
			if (action == AUTH)
			{
				gets_s(not_correct, 100);
				printf("Авторы: ");
				icounter = scanf("%[^\n]s", auth);
				if ((auth[0] == ' ' && strlen((const char*)auth) == 1))
				{
					printf("Введены неккоректные данные!\n");
					return;
				}
			}
			if (action == YEAR)
			{
				//int temp_year;
				printf("Год выпуска");
				icounter = scanf("%d", &year);
				if (year > CURRENT_YEAR || year < 0)
				{
					printf("Введены неккоректные данные!\n");
					return ;
				}
				//= temp_year;
			}
			if (action == PUBL)
			{
				gets_s(not_correct, 100);
				printf("Издательство: ");
				icounter = scanf("%[^\n]s", publ);
				if ((publ[0] == ' ' && strlen((const char*)publ) == 1))
				{
					printf("Введены неккоректные данные!\n");
					return ;
				}
			}
			if (action == RATE)
			{
				printf("Оценка: ");
				icounter = scanf("%d", &rate);
				if (rate < 0)
				{
					printf("Введены неккоректные данные!\n");
					return ;
				}
				//temp->rate = temp_rate;
			}
			if (action == COST)
			{
				//int temp_cost;
				printf("Цена: ");
				icounter = scanf("%d", &cost);
				if (cost < 0)
				{
					printf("Введены неккоректные данные!\n");
					return ;
				}

			}
			if (action == GENRE)
			{
				gets_s(not_correct, 100);
				printf("Краткое описание: ");
				icounter = scanf("%[^\n]s", genre);
				if ((genre[0] == ' ' && strlen((const char*)genre) == 1))
				{
					printf("Введены неккоректные данные!\n");
					return ;
				}
			}
		}

	}

	Insert(title_tree, TITLE, title1, auth, year, publ, rate, cost, genre, desc);
	Insert(auth_tree, AUTH, title1, auth, year, publ, rate, cost, genre, desc);
	Insert(year_tree, YEAR, title1, auth, year, publ, rate, cost, genre, desc);
	Insert(publ_tree, PUBL, title1, auth, year, publ, rate, cost, genre, desc);
	Insert(rate_tree, RATE, title1, auth, year, publ, rate, cost, genre, desc);
	Insert(cost_tree, COST, title1, auth, year, publ, rate, cost, genre, desc);
	Insert(genre_tree, GENRE, title1, auth, year, publ, rate, cost, genre, desc);

	*title_tree = Delete(*title_tree, title);
	*auth_tree = Delete(*auth_tree, title);
	*year_tree = Delete(*year_tree, title);
	*publ_tree = Delete(*publ_tree, title);
	*rate_tree = Delete(*rate_tree, title);
	*cost_tree = Delete(*cost_tree, title);
	*genre_tree = Delete(*genre_tree, title);
}

int height(TTree* p)
{
	return p ? p->height : 0;
}

int bfactor(TTree* p)
{
	return height(p->right) - height(p->left);
}

void fixheight(TTree* my_tree)
{
	int hl = height(my_tree->left);
	int hr = height(my_tree->right);
	my_tree->height = (hl > hr ? hl : hr) + 1;
}
TTree* rotateright(TTree* right_rotate) // правый поворот вокруг right_rotate
{
	TTree* q = right_rotate->left;
	right_rotate->left = q->right;
	q->right = right_rotate;
	fixheight(right_rotate);
	fixheight(q);
	return q;
}

TTree* rotateleft(TTree* left_rotate) // левый поворот вокруг left_rotate
{
	TTree* tmp = left_rotate->right;
	left_rotate->right = tmp->left;
	tmp->left = left_rotate;
	fixheight(left_rotate);
	fixheight(tmp);
	return tmp;
}
TTree* balance(TTree* node) // балансировка узла  node
{
	fixheight(node);
	if (bfactor(node) == 2)
	{
		if (bfactor(node->right) < 0)
			node->right = rotateright(node->right);
		return rotateleft(node);
	}
	if (bfactor(node) == -2)
	{
		if (bfactor(node->left) > 0)
			node->left = rotateleft(node->left);
		return rotateright(node);
	}
	return node; // балансировка не нужна
}
void Insert(TTree** Tree,/* TTree *InTree*/enum Param param, unsigned char* title, unsigned char* auth, int year, unsigned char* publ, int rate, int cost, unsigned char* genre, unsigned char* desc) {

	if (*Tree == NULL)
	{
		*Tree = (struct Tree*)malloc(sizeof(struct Tree));
		assert(*Tree != NULL);
		memset(*Tree, 0x00, sizeof(struct Tree));
		(*Tree)->title = title;
		(*Tree)->auth = auth;
		(*Tree)->year = year;
		(*Tree)->publ = publ;
		(*Tree)->rate = rate;
		(*Tree)->cost = cost;
		(*Tree)->genre = genre;
		(*Tree)->desc = desc;
	}

	else if (param == TITLE)
	{
		if (strcmp((const char*)(*Tree)->title, (const char*)title) > 0)
		{
			Insert(&((*Tree)->left), param, title, auth, year, publ, rate, cost, genre, desc);
		}
		else
		{
			Insert(&((*Tree)->right), param, title, auth, year, publ, rate, cost, genre, desc);
		}
	}

	else if (param == AUTH)
	{
		if (strcmp((const char*)(*Tree)->auth, (const char*)auth) > 0)
		{
			Insert(&((*Tree)->left), param, title, auth, year, publ, rate, cost, genre, desc);
		}
		else
		{
			Insert(&((*Tree)->right), param, title, auth, year, publ, rate, cost, genre, desc);
		}
	}
	else if (param == YEAR)
	{
		if ((*Tree)->year > year)
		{
			Insert(&((*Tree)->left), param, title, auth, year, publ, rate, cost, genre, desc);
		}
		else
		{
			Insert(&((*Tree)->right), param, title, auth, year, publ, rate, cost, genre, desc);
		}
	}

	else if (param == PUBL)
	{
		if (strcmp((const char*)(*Tree)->publ, (const char*)publ) > 0)
		{
			Insert(&((*Tree)->left), param, title, auth, year, publ, rate, cost, genre, desc);
		}
		else
		{
			Insert(&((*Tree)->right), param, title, auth, year, publ, rate, cost, genre, desc);
		}
	}

	else if (param == RATE)
	{
		if ((*Tree)->rate > rate)
		{
			Insert(&((*Tree)->left), param, title, auth, year, publ, rate, cost, genre, desc);
		}
		else
		{
			Insert(&((*Tree)->right), param, title, auth, year, publ, rate, cost, genre, desc);
		}
	}

	else if (param == COST)
	{
		if ((*Tree)->cost > cost)
		{
			Insert(&((*Tree)->left), param, title, auth, year, publ, rate, cost, genre, desc);
		}
		else
		{
			Insert(&((*Tree)->right), param, title, auth, year, publ, rate, cost, genre, desc);
		}
	}

	else if (param == GENRE)
	{
		if (strcmp((const char*)(*Tree)->genre, (const char*)genre) > 0)
		{
			Insert(&((*Tree)->left), param, title, auth, year, publ, rate, cost, genre, desc);
		}
		else
		{
			Insert(&((*Tree)->right), param, title, auth, year, publ, rate, cost, genre, desc);
		}
	}
	*Tree = balance(*Tree);
}




/*
Удаление книги по названию
tree - дерево всех книг
title - название книги для удаления
*/

int g_delete_it = 0; // для сбора сведений о том, была ли удалена книга

TTree* Delete(TTree* tree, unsigned char* title) {

	if (tree == NULL)
	{
		return tree;
	}


	if (strcmp((const char*)tree->title, (const char*)title) == 0)
	{

		TTree* tmp = (TTree*)malloc(sizeof(TTree));
		if (tree->right == NULL) // если правое поддерево пустое
		{
			tmp = tree->left;
			g_delete_it++;
		}
		else
		{
			TTree* ptr = (TTree*)malloc(sizeof(TTree));

			ptr = tree->right;
			g_delete_it++;
			if (ptr->left == NULL)
			{
				ptr->left = tree->left;
				tmp = ptr;
			}
			else
			{
				TTree* pmin = (TTree*)malloc(sizeof(TTree));
				if (pmin == NULL) return NULL;

				pmin = ptr->left;

				g_delete_it++;
				while (pmin->left != NULL)
				{
					ptr = pmin;
					pmin = ptr->left;
				}

				ptr->left = pmin->right;
				pmin->left = tree->left;
				pmin->right = tree->right;
				tmp = pmin;
			}
		}
		free(tree);
		return tmp;
	}
	else if (strcmp((const char*)title, (const char*)tree->title) < 0)
	{
		tree->left = Delete(tree->left, title);
	}
	else
	{
		tree->right = Delete(tree->right, title);
	}
	tree = balance(tree);
	return tree;

}


void CreateFileNames(char* input_name, char* title_name, char* auth_name, char* year_name, char* publ_name, char* rate_name,
	char* cost_name, char* genre_name) {
	int count = (int)strlen(input_name);
	char ost[10] = { 0 };
	int counter = 0;
	while (input_name[count--] != '.')
	{
		ost[counter++] = input_name[count];
	}
	counter++;

	ost[counter] = '\0';

	_strrev(ost);

	strcat(title_name, ost);

	strcat(auth_name, ost);

	strcat(year_name, ost);

	strcat(publ_name, ost);

	strcat(rate_name, ost);

	strcat(cost_name, ost);

	strcat(genre_name, ost);

	//char 
}


void WriteToFile(TTree* tree, FILE* f_title, const char* title_name, int level) {


	if (tree)
	{
		WriteToFile(tree->left, f_title, title_name, level + 1);
		// выводим на экран элемент структуры (те информацию о книге)

		fprintf(f_title, "%s\n", tree->title);
		fprintf(f_title, "%s\n", tree->auth);
		fprintf(f_title, "%s\n", tree->publ);
		fprintf(f_title, "%s\n", tree->genre);
		fprintf(f_title, "%s\n", tree->desc);
		fprintf(f_title, "%d\n", tree->year);
		fprintf(f_title, "%d\n", tree->rate);
		fprintf(f_title, "%d\n", tree->cost);
		WriteToFile(tree->right, f_title, title_name, level + 1);
	}

}



int print_menu(TTree** tree_title, TTree** tree_auth, TTree** tree_year, TTree** tree_publ, TTree** tree_rate, TTree** tree_cost, TTree** tree_genre) {

	int icounter = 0;

	int FLAG = TRUE;

	while (FLAG)
	{

		printf("\nВыберите действие которое хотите сделать:\n\t1.Добавление книги\n\t2.Удаление книги\n\t3.Редактирование книги\n\t4.Поиск\n\t5.Сортировка\n\t6.Сохранение\n\t7.Загрузка\n\t8.Вывод информации\n\t9. Выход из программы\n");
		int action;
		int ctr = 0;
		printf("=> ");
		if (scanf("%d", &action) != 1)
		{
			return EXIT_FAILURE;
		}
		if (action == 1)
		{

			unsigned char* title = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
			memset(title, 0, MAX_DATA_SIZE);
			unsigned char* auth = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
			memset(auth, 0, MAX_DATA_SIZE);
			int year;
			unsigned char* publ = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
			memset(publ, 0, MAX_DATA_SIZE);
			int rate;
			int cost;
			unsigned char* genre = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
			memset(genre, 0, MAX_DATA_SIZE);
			unsigned char* desc = (unsigned char*)malloc(sizeof(unsigned char) * 1024);
			memset(desc, 0, 1024);
			if (title == NULL || auth == NULL || publ == NULL || genre == NULL || desc == NULL) return EXIT_FAILURE;
			Addition(tree_title, tree_auth, tree_year, tree_publ, tree_rate, tree_cost, tree_genre,
				title, auth, &year, publ, &rate, &cost, genre, desc, 0);

			print_tree_new(*tree_title, 0);
			if (g_print_tree_new_it == 0)
			{
				printf("\n\nБиблиотека книг на данный момент пуста\n\n");
			}
		}
		if (action == 2)
		{

			g_delete_it = 0;
			g_print_tree_new_it = 0;

			 char not_correct[100];

			gets_s(not_correct, 100);

			unsigned char* title_to_delete = (unsigned char*)malloc(sizeof(unsigned char) * 128);
			if (title_to_delete == NULL) return EXIT_FAILURE;

			printf("Введите название книги для удаления: ");

			ctr = scanf("%[^\n]s", title_to_delete);

			*tree_title = Delete(*tree_title, title_to_delete);

			if (g_delete_it == 0)
			{
				printf("\nНе удалось удалить книгу: книга с данным именем не найдена...\n\n");
			}
			/*Delete;*/
			print_tree_new(*tree_title, 0);
			if (g_print_tree_new_it == 0)
			{
				printf("\n\nБиблиотека кнмг в данный момент пуста\n\n");
			}
		}

		if (action == 3)
		{

			 char not_correct[100];

			gets_s(not_correct, 100);

			unsigned char* title = (unsigned char*)malloc(sizeof(char) * 128);
			if (title == NULL) return EXIT_FAILURE;


			fprintf(stdout, "Введите название книги, которую хотите отредактировать: ");

			icounter = scanf("%[^\n]s", title);

			/*РЕДАКТИРУЕМ*/
			redact_B(tree_title, tree_auth, tree_year, tree_publ, tree_rate, tree_cost, tree_genre, title);
		}

		if (action == 4)
		{
			/*ПОИСК*/
			int counter = 0;

			int search;
			
			printf("По какому параметру хотите осуществить поиск?\n\t1.Название\n\t2.Авторы\n\t3.Год выпуска\n\t4.Издательство\n\t5.Оценка\n\t6.Цена\n\t7.Жанр\n");
			if (scanf("%d", &search) != 1)
			{
				return EXIT_FAILURE;
			}
			if (search == 1) {
				unsigned char* book_name = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);

				printf("Введите название книги которую хотите найти: ");
				 char not_correct[100];

				gets_s(not_correct, 100);
				if (scanf("%[^\n]s", book_name) != 1)
				{
					return EXIT_FAILURE;
				}
				if ((book_name[0] == ' ' && strlen((const char*)book_name) == 1))
				{
					printf("Введены неккоректные данные!\n");
					return EXIT_FAILURE;
				}
				title_search_print(tree_title, book_name, &counter);
				if (counter == 0) printf("Книги по данному запросу не найдены...\n\n");

			}
			if (search == 2)
			{
				unsigned char* book_auth = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
				printf("Введите автора: ");
				 char not_correct[100];

				gets_s(not_correct, 100);
				if (scanf("%[^\n]s", book_auth) != 1)
				{
					return EXIT_FAILURE;
				}
				if ((book_auth[0] == ' ' && strlen((const char*)book_auth) == 1))
				{
					printf("Введены неккоректные данные!\n");
					return EXIT_FAILURE;
				}
				auth_search_print(tree_auth, book_auth, &counter);
				if (counter == 0) printf("Книги по данному запросу не найдены...\n\n");
			}
			if (search == 3)
			{
				int book_year;
				printf("Введите год: ");
				if (scanf("%d", &book_year) != 1)
				{
					return EXIT_FAILURE;
				}
				if (book_year>CURRENT_YEAR || book_year < 0)
				{
					printf("Неверный ввод!\n");
				}
				year_search_print(tree_year, book_year, &counter);
				if (counter == 0) printf("Книги по данному запросу не найдены...\n\n");
			}
			if (search == 4)
			{
				unsigned char* book_publ = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
				printf("Введите издательство: ");
				 char not_correct[100];
				gets_s(not_correct, 100);
				if (scanf("%[^\n]s", book_publ) != 1)
				{
					return EXIT_FAILURE;
				}
				if ((book_publ[0] == ' ' && strlen((const char*)book_publ) == 1))
				{
					printf("Введены неккоректные данные!\n");
					return EXIT_FAILURE;
				}
				publ_search_print(tree_publ, book_publ, &counter);
				if (counter == 0) printf("Книги по данному запросу не найдены...\n\n");
			}
			if (search == 5)
			{
				int book_rate;
				printf("Введите год: ");
				if (scanf("%d", &book_rate) != 1)
				{
					return EXIT_FAILURE;
				}
				if (book_rate < 0)
				{
					printf("Неверный ввод!\n");
					return EXIT_FAILURE;
				}
				rate_search_print(tree_rate, book_rate, &counter);
				if (counter == 0) printf("Книги по данному запросу не найдены...\n\n");

			}
			if (search == 6)
			{

				int book_cost;
				printf("Введите ценy: ");
				if (scanf("%d", &book_cost) != 1)
				{
					return EXIT_FAILURE;
				}
				if (book_cost < 0)
				{
					printf("Неверный ввод\n");
					return EXIT_FAILURE;
				}
				cost_search_print(tree_cost, book_cost, &counter);
				if (counter == 0) printf("Книги по данному запросу не найдены...\n\n");
			}
			if (search == 7)
			{
				unsigned char book_genre[100];
				printf("Введите жанр: ");
				 char not_correct[100];
				gets_s(not_correct, 100);
				if (scanf("%[^\n]s", &book_genre) != 1)
				{
					return EXIT_FAILURE;
				}
				if ((book_genre[0] == ' ' && strlen((const char*)book_genre) == 1))
				{
					printf("Введены неккоректные данные!\n");
					return EXIT_FAILURE;
				}
				genre_search_print(tree_genre, book_genre, &counter);
				if (counter == 0) printf("Книги по данному запросу не найдены...\n\n");
			}

		}

		if (action == 5)
		{
			/*СОРТИРОВКА*/
			TTree* cur_tree = (TTree*)malloc(sizeof(TTree));
			if (cur_tree == NULL) return EXIT_FAILURE;
			cur_tree = f_SORT(tree_title, tree_auth, tree_year, tree_publ, tree_rate, tree_cost, tree_genre);
		}

		if (action == 6)
		{
			/*CОХРАНЯЕМ*/
			char not_correct[100];
			int icounter_1 = 0;
			char* file_output = (char*)malloc(sizeof(char) * 64);
			if (file_output == NULL) return EXIT_FAILURE;

			memset(file_output, 0, sizeof(file_output));

			printf("\nВведите имя файла, в который будет сохранена БД: ");

			gets_s(not_correct, 100);

			icounter_1 = scanf("%[^\n]s", file_output);

			if (icounter_1 < 0) // если строку считать не удалось
			{
				fprintf(stderr, "Не удалось считать имя файла...\n\n");
				return EXIT_FAILURE;
			}



			FILE* f_out = NULL;

			f_out = fopen(file_output, "w+");
			if (f_out == NULL)
			{
				fprintf(stderr, "Ошибка: невозможно создать файл для сохранения БД\n\n");
				return EXIT_FAILURE;
			}

			WriteToFile(*tree_title, f_out, file_output, 0);
			fprintf(f_out, "/");

			fclose(f_out);

			printf("Запись БД в файл с именем: %s прошла успешно\n\n", file_output);
		}

		if (action == 7)
		{
			char not_correct[100];

			gets_s(not_correct, 100);

			int icounter_2 = 0;

			char* file_input = (char*)malloc(sizeof(char) * 64);
			if (file_input == NULL) return EXIT_FAILURE;

			memset(file_input, 0, sizeof(file_input));

			printf("Введите имя файла, из которого будет считана БД: ");

			icounter_2 = scanf("%[^\n]s", file_input);

			if (icounter_2 < 0)
			{
				fprintf(stderr, "Не удалось считать имя файла...\n\n");
				return EXIT_FAILURE;
			}

			int istatus = loading_from_file(tree_title, tree_auth, tree_year, tree_publ, tree_rate, tree_cost, tree_genre, file_input, 0);

			if (istatus == EXIT_FAILURE)
			{
				fprintf(stderr, "Ошибка при чтении БД из файла...\n\n");
				return EXIT_FAILURE;
			}
			else
			{
				fprintf(stdout, "Чтение БД из файла прошло успешно\n\n");
			}
		}
		if (action == 8)
		{
			g_print_tree_new_it = 0;
			print_tree_new(*tree_title, 0);
			if (g_print_tree_new_it == 0)
			{
				printf("\n\nБиблиотека книг на данный момент пуста\n\n");
			}
		}
		if (action == 9)
		{
			FLAG = FALSE;
		}
	}
	return EXIT_SUCCESS;
}



int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "rus");
	SetConsoleCP(1251); //установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); //установка кодовой страницы win-cp 1251 в поток

	eCLog estatus; // статус для считанных с консоли ключей
	eAType etype; // выполняемое действие

	char* InputFileName = (char*)malloc(sizeof(char) * MAXFILENAMELEN); // входной файл
	char* OutputFileName = (char*)malloc(sizeof(char) * MAXFILENAMELEN); // выходной файл

	if (InputFileName == NULL || OutputFileName == NULL)
	{
		return EXIT_FAILURE;
	}

	memset(InputFileName, 0, sizeof(InputFileName));
	memset(OutputFileName, 0, sizeof(OutputFileName));

	TTree* cur_tree = NULL;
//	enum Param cur_param = 0;

	TTree* title_tree = NULL;
	TTree* auth_tree = NULL;
	TTree* year_tree = NULL;
	TTree* publ_tree = NULL;
	TTree* rate_tree = NULL;
	TTree* cost_tree = NULL;
	TTree* genre_tree = NULL;


	unsigned char* title = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
	memset(title, 0, MAX_DATA_SIZE);
	unsigned char* auth = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
	memset(auth, 0, MAX_DATA_SIZE);
	int year;
	unsigned char* publ = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
	memset(publ, 0, MAX_DATA_SIZE);
	int rate;
	int cost;
	unsigned char* genre = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
	memset(genre, 0, MAX_DATA_SIZE);
	unsigned char* desc = (unsigned char*)malloc(sizeof(unsigned char) * 1024);
	memset(desc, 0, 1024);
	if (title == NULL || auth == NULL || publ == NULL || genre == NULL || desc == NULL) return EXIT_FAILURE;

	estatus = GetKeysFromConsole(argc, argv, &etype, InputFileName, OutputFileName);


	if (estatus == CONSOLE_LOG_ERROR) // введены неверные параметры командной строки
	{
		fprintf(stderr, "Ошибка: введены неправильные команды\n");
		fprintf(stdout, "\nПереходим в интерактивный режим...\n\n");
		// Выполнение действий интерактивного режима

		int Ok = print_menu(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree/*,
			title, auth, &year, publ, &rate, &cost, genre, desc*/);

		return Ok;
	}

	else if (estatus == CONSOLE_LOG_HELP) // вывод справки о работе программы
	{
		// вывели справку о работе программы
		// выходим из программы
		return EXIT_SUCCESS;
	}
	else if (estatus == CONSOLE_LOG_ACTION) // Выполняем 1 действие
	{

		int istatus = 0;

		// Выполняем 1 действие, которое хранится в etype

		if (etype == ADD) // добавление
		{

			istatus = Addition(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree,
				title, auth, &year, publ, &rate, &cost, genre, desc, 1);

			return istatus;
		}

		else if (etype == FIND) // поиск
		{
			istatus = f_FIND(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);

			return istatus;
		}

		else if (etype == SORT) // сортировка
		{
			cur_tree = f_SORT(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);

			return EXIT_SUCCESS;
		}

		else if (etype == REMOVE) // удаление
		{
			istatus = f_REMOVE(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);

			return istatus;
		}

		else if (etype == EDIT) // редактирование
		{
			istatus = f_EDIT(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);

			return istatus;
		}

		else if (etype == PRINT) // вывод дерева на экран
		{
			istatus = f_PRINT(title_tree);

			return istatus;
		}
	}
	else if (estatus == CONSOLE_LOG_IN_ACTION) // чтение базы данных из файла + выполнение действия
	{
		int istatus = 0;

		// чтение базы данных
		loading_from_file(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree, InputFileName, 0);

		if (etype == ADD) // добавление
		{
			istatus = Addition(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree,
				title, auth, &year, publ, &rate, &cost, genre, desc, 1);
		}

		else if (etype == FIND) // поиск
		{
			istatus = f_FIND(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);


		}

		else if (etype == SORT) // сортировка
		{
			cur_tree = f_SORT(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);


		}

		else if (etype == REMOVE) // удаление
		{
			istatus = f_REMOVE(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);


		}

		else if (etype == EDIT) // редактирование
		{
			istatus = f_EDIT(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);


		}

		else if (etype == PRINT) // вывод дерева на экран
		{
			istatus = f_PRINT(title_tree);


		}

		// Записываем в исходный файл полученную БД

		FILE* f_in = fopen(InputFileName, "w");
		WriteToFile(title_tree, f_in, InputFileName, 0);
		fprintf(f_in, "/");
		// КОНЕЦ

	}

	else if (estatus == CONSOLE_LOG_OUT_ACTION) // выполнение действия + запись БД в файл
	{

		int istatus = 0;


		if (etype == ADD) // добавление
		{
			unsigned char* title_1 = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
			unsigned char* auth_1 = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
			int year_1;
			unsigned char* publ_1 = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
			int rate_1;
			int cost_1;
			unsigned char* genre_1 = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
			unsigned char* desc_1 = (unsigned char*)malloc(sizeof(unsigned char) * 1024);
			memset(title_1, 0, MAX_DATA_SIZE);
			memset(auth_1, 0, MAX_DATA_SIZE);
			memset(publ_1, 0, MAX_DATA_SIZE);
			memset(genre_1, 0, MAX_DATA_SIZE);
			memset(desc_1, 0, 1024);
			if (title == NULL || auth == NULL || publ == NULL || genre == NULL || desc == NULL) return EXIT_FAILURE;

			istatus = Addition(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree,
				title_1, auth_1, &year_1, publ_1, &rate_1, &cost_1, genre_1, desc_1, 1);


		}

		else if (etype == FIND) // поиск
		{
			istatus = f_FIND(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);


			return istatus;
		}

		else if (etype == SORT) // сортировка
		{
			cur_tree = f_SORT(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);

		}

		else if (etype == REMOVE) // удаление
		{
			istatus = f_REMOVE(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);

		}

		else if (etype == EDIT) // редактирование
		{
			istatus = f_EDIT(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);

		}

		else if (etype == PRINT) // вывод дерева на экран
		{
			istatus = f_PRINT(title_tree);

		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////

		//  Запись БД в файл
		FILE* f_out;
		f_out = fopen(OutputFileName, "w+");
		if (f_out == NULL)
		{
			printf("Ошибка открытия файла");
			return EXIT_FAILURE;
		}
		WriteToFile(title_tree, f_out, OutputFileName, 0);
	}

	else if (estatus == CONSOLE_LOG_IN_OUT_ACTION) // чтение БД из файла + действие + запись БД в файл
	{

		int istatus = 0;


		// чтение БД мз файлв
		loading_from_file(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree, InputFileName, 0);

		////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (etype == ADD) // добавление
		{
			unsigned char* title_1 = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
			unsigned char* auth_1 = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
			int year_1;
			unsigned char* publ_1 = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
			int rate_1;
			int cost_1;
			unsigned char* genre_1 = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
			unsigned char* desc_1 = (unsigned char*)malloc(sizeof(unsigned char) * 1024);
			memset(title_1, 0, MAX_DATA_SIZE);
			memset(auth_1, 0, MAX_DATA_SIZE);
			memset(publ_1, 0, MAX_DATA_SIZE);
			memset(genre_1, 0, MAX_DATA_SIZE);
			memset(desc_1, 0, 1024);
			if (title == NULL || auth == NULL || publ == NULL || genre == NULL || desc == NULL) return EXIT_FAILURE;

			istatus = Addition(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree,
				title_1, auth_1, &year_1, publ_1, &rate_1, &cost_1, genre_1, desc_1, 1);


		}

		else if (etype == FIND) // поиск
		{
			istatus = f_FIND(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);


		}

		else if (etype == SORT) // сортировка
		{
			cur_tree = f_SORT(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);

		}

		else if (etype == REMOVE) // удаление
		{
			istatus = f_REMOVE(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);

		}

		else if (etype == EDIT) // редактирование
		{
			istatus = f_EDIT(&title_tree, &auth_tree, &year_tree, &publ_tree, &rate_tree, &cost_tree, &genre_tree);

		}

		else if (etype == PRINT) // вывод дерева на экран
		{
			istatus = f_PRINT(title_tree);

		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		//  Запись БД в файл
		FILE* F_OUT = fopen(OutputFileName, "w+");
		WriteToFile(title_tree, F_OUT, OutputFileName, 0);

	}



	// fclose(f_title); fclose(f_auth); fclose(f_publ); fclose(f_rate); fclose(f_cost); fclose(f_genre);

	return EXIT_SUCCESS;
}


int f_FIND(TTree** title_tree, TTree** auth_tree, TTree** year_tree, TTree** publ_tree, TTree** rate_tree, TTree** cost_tree, TTree** genre_tree)
{
	int icounter = 0;
	int action;

	int counter = 0;

	unsigned char* sdata = (unsigned char*)malloc(sizeof(unsigned char) * MAX_DATA_SIZE);
	if (sdata == NULL) return EXIT_FAILURE;

	int idata = 0;

	printf("\nВыберите, по какому полю будет осуществляться поиск\n\t 1.Поиск по названию\n\t2.Поиск по автору(авторам)\n\t3.Поиск по году выпуска\n\t4.Поиск по издательству\n\t5.Поиск по оценке\n\t6.Поиск по цене\n\t7.Поиск по жанру\n\n");
	printf("=> ");
	if (scanf("%d", &action) != 1 && action < 1 && action > 7)
	{
		return EXIT_FAILURE;
	}
	else
	{

		if (action == TITLE)
		{

			printf("Введите название для поиска: ");
			icounter = scanf("%[^\n]s", sdata);
			/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
			if ((sdata[0] == ' ' && strlen((const char*)sdata) == 1))
			{
				printf("Введены неккоректные данные!\n");
				return EXIT_FAILURE;
			}
			title_search_print(title_tree, sdata, &counter);
		}
		if (action == AUTH)
		{

			printf("Введите автора для поиска: ");
			icounter = scanf("%[^\n]s", sdata);
			if ((sdata[0] == ' ' && strlen((const char*)sdata) == 1))
			{
				printf("Введены неккоректные данные!\n");
				return EXIT_FAILURE;
			}
			auth_search_print(auth_tree, sdata, &counter);

		}
		if (action == YEAR)
		{

			printf("Введите год для поиска: ");
			icounter = scanf("%d", &idata);
			if (idata < 0)
			{
				printf("Введены неккоректные данные!\n");
				return EXIT_FAILURE;
			}
			year_search_print(year_tree, idata, &counter);
		}
		if (action == PUBL)
		{

			printf("Введите издательство для поиска: ");
			icounter = scanf("%[^\n]s", sdata);
			if ((sdata[0] == ' ' && strlen((const char*)sdata) == 1))
			{
				printf("Введены неккоректные данные!\n");
				return EXIT_FAILURE;
			}
			publ_search_print(publ_tree, sdata, &counter);
		}
		if (action == RATE)
		{

			printf("Введите оценку для поиска: ");
			icounter = scanf("%d", &idata);
			if (idata < 0)
			{
				printf("Неверный ввод\n");
				return EXIT_FAILURE;
			}
			rate_search_print(rate_tree, idata, &counter);
		}
		if (action == COST)
		{

			printf("Введите цену для поиска: ");
			icounter = scanf("%d", &idata);
			if (idata < 0)
			{
				printf("Неверный ввод\n");
				return EXIT_FAILURE;
			}
			cost_search_print(cost_tree, idata, &counter);
		}
		if (action == GENRE)
		{

			printf("Введите жанр для поиска: ");
			icounter = scanf("%[^\n]s", sdata);
			if ((sdata[0] == ' ' && strlen((const char*)sdata) == 1))
			{
				printf("Введены неккоректные данные!\n");
				return EXIT_FAILURE;
			}
			genre_search_print(genre_tree, sdata, &counter);
		}

		if (counter == 0)
		{
			printf("\n\nНе удается найти книгу по заданному запросу...");
		}

		printf("\n\n");
	}

	return EXIT_SUCCESS;
}


TTree* f_SORT(TTree** title_tree, TTree** auth_tree, TTree** year_tree, TTree** publ_tree, TTree** rate_tree, TTree** cost_tree, TTree** genre_tree)
{
	printf("\nВыберите, по какому полю будет осуществляться сортировка\n\t 1.Сортировка по названию\n\t2.Сортировка по автору(авторам)\n\t3.Сортировка по году выпуска\n\t4.Сортировка по издательству\n\t5.Сортировка по оценке\n\t6.Сортировка по цене\n\t7.Сортировка по жанру\n\n");
	printf("=> ");

	int action;

	if (scanf("%d", &action) != 1 && action < 1 && action > 7)
	{
		return NULL;
	}
	else
	{
		if (action == TITLE)
		{
			printf("\n\nСортировка успешно выполнена\n\n");
			return *title_tree;
		}
		if (action == AUTH)
		{
			printf("\n\nСортировка успешно выполнена\n\n"); return *auth_tree;
		}
		if (action == YEAR)
		{
			printf("\n\nСортировка успешно выполнена\n\n"); return *year_tree;
		}
		if (action == PUBL)
		{
			printf("\n\nСортировка успешно выполнена\n\n"); return *publ_tree;
		}
		if (action == RATE)
		{
			printf("\n\nСортировка успешно выполнена\n\n"); return *rate_tree;
		}
		if (action == COST)
		{
			printf("\n\nСортировка успешно выполнена\n\n"); return *cost_tree;
		}
		if (action == GENRE)
		{
			printf("\n\nСортировка успешно выполнена\n\n"); return *genre_tree;
		}

	}
	return NULL;

}

int f_REMOVE(TTree** title_tree, TTree** auth_tree, TTree** year_tree, TTree** publ_tree, TTree** rate_tree, TTree** cost_tree, TTree** genre_tree)
{

	int icounter;

	unsigned char* title = (unsigned char*)malloc(sizeof(unsigned char) * 128);
	if (title == NULL) return EXIT_FAILURE;


	printf("Введите название книги для удаления: ");

	icounter = scanf("%[^\n]s", title);

	TTree* temp = (TTree*)malloc(sizeof(TTree));

	temp = Delete(*title_tree, title);
	temp = Delete(*auth_tree, title);
	temp = Delete(*year_tree, title);
	temp = Delete(*publ_tree, title);
	temp = Delete(*rate_tree, title);
	temp = Delete(*cost_tree, title);
	temp = Delete(*genre_tree, title);

	printf("Удаление прошло успешно\n\nСледующая книга была удалена:\n");
	print_tree(temp);

	return EXIT_SUCCESS;
}


int f_EDIT(TTree** tree_title, TTree** tree_auth, TTree** tree_year, TTree** tree_publ, TTree** tree_rate, TTree** tree_cost, TTree** tree_genre) {

	int icounter = 0;

	unsigned char* title = (unsigned char*)malloc(sizeof(char) * 128);
	if (title == NULL) return EXIT_FAILURE;


	printf("Введите название книги, которую хотите отредактировать: ");

	icounter = scanf("%s", title);

	/*РЕДАКТИРУЕМ*/
	redact_B(tree_title, tree_auth, tree_year, tree_publ, tree_rate, tree_cost, tree_genre, title);

	return EXIT_SUCCESS;
}


int f_PRINT(TTree* tree) {

	printf("\nВыводим информацию о всех имеющихся книгах...\n\n\n");

	print_tree_new(tree, 0);

	if (g_print_tree_new_it == 0) // если не одной книги не вывели на экран
	{
		printf("Библиотека не содержит книг...\n\n");
	}

	return EXIT_SUCCESS;
}

int loading_from_file(TTree** tree_title, TTree** tree_auth, TTree** tree_year, TTree** tree_publ, TTree** tree_rate, TTree** tree_cost, TTree** tree_genre, \
	const char* input_name, int level) {

	FILE* f_input;
	f_input = fopen(input_name, "r");
	if (f_input == NULL)
	{
		return EXIT_FAILURE;
	}
	level = 0;
	while (1)
	{

		unsigned char* title = (unsigned char*)malloc(sizeof(int) * MAX_DATA_SIZE);
		memset(title, 0, MAX_DATA_SIZE);
		unsigned char* auth = (unsigned char*)malloc(sizeof(int) * MAX_DATA_SIZE);
		memset(auth, 0, MAX_DATA_SIZE);
		int year = 0;
		unsigned char* publ = (unsigned char*)malloc(sizeof(int) * MAX_DATA_SIZE);
		memset(publ, 0, MAX_DATA_SIZE);
		int rate = 0;
		int cost = 0;
		unsigned char* genre = (unsigned char*)malloc(sizeof(int) * MAX_DATA_SIZE);
		memset(genre, 0, MAX_DATA_SIZE);
		unsigned char* desc = (unsigned char*)malloc(sizeof(int) * 1024);
		memset(desc, 0, MAX_DATA_SIZE);
		int i_1 = 0;
		unsigned char ch;

		//////////////////////////////////////////////////////////////////

		//fseek(f_input, -1, SEEK_CUR);
		while ((ch = (unsigned char)fgetc(f_input)) != '\n')
		{
			title[i_1++] = ch;
		}
		i_1 = 0;
		while ((ch = (unsigned char)fgetc(f_input)) != '\n')
		{
			auth[i_1++] = ch;
		}
		i_1 = 0;
		while ((ch = (unsigned char)fgetc(f_input)) != '\n')
		{
			publ[i_1++] = ch;
		}
		i_1 = 0;
		while ((ch = (unsigned char)fgetc(f_input)) != '\n')
		{
			genre[i_1++] = ch;
		}
		i_1 = 0;
		while ((ch = (unsigned char)fgetc(f_input)) != '\n')
		{
			desc[i_1++] = ch;
		}
		unsigned char* year_char = (unsigned char*)malloc(sizeof(unsigned char) * MAX_VALUE_FOR_ATOI);
		memset(year_char, 0, MAX_VALUE_FOR_ATOI);
		i_1 = 0;
		while ((ch = (unsigned char)fgetc(f_input)) != '\n')
		{
			year_char[i_1++] = ch;
		}
		year = atoi((const char *)year_char);
		unsigned char* rate_char = (unsigned char*)malloc(sizeof(unsigned char) * MAX_VALUE_FOR_ATOI);
		memset(rate_char, 0, MAX_VALUE_FOR_ATOI);
		i_1 = 0;
		while ((ch = (unsigned char)fgetc(f_input)) != '\n')
		{
			rate_char[i_1++] = ch;
		}
		rate_char[i_1] = '\0';
		rate = atoi((const char*)rate_char);
		unsigned char* cost_char = (unsigned char*)malloc(sizeof(unsigned char) * MAX_VALUE_FOR_ATOI);
		memset(cost_char, 0, MAX_VALUE_FOR_ATOI);
		i_1 = 0;
		while ((ch = (unsigned char)fgetc(f_input)) != '\n')
		{
			cost_char[i_1++] = ch;
		}
		cost = atoi((const char*)cost_char);
		//fseek(f_input, -1, SEEK_CUR);
		//ch = fgetc(f_input); //для считывания последнего \n
		i_1 = 0;

		Insert(tree_title, TITLE, title, auth, year, publ, rate, cost, genre, desc);
		Insert(tree_auth, AUTH, title, auth, year, publ, rate, cost, genre, desc);
		Insert(tree_year, YEAR, title, auth, year, publ, rate, cost, genre, desc);
		Insert(tree_publ, PUBL, title, auth, year, publ, rate, cost, genre, desc);
		Insert(tree_rate, RATE, title, auth, year, publ, rate, cost, genre, desc);
		Insert(tree_cost, COST, title, auth, year, publ, rate, cost, genre, desc);
		Insert(tree_genre, GENRE, title, auth, year, publ, rate, cost, genre, desc);



		if ((ch = (unsigned char)fgetc(f_input)) == '/')
		{
			break;
		}
		else           
		{
			fseek(f_input, -1, SEEK_CUR);
		}
	}

	print_tree_new(*tree_title, 0);
	return EXIT_SUCCESS;
}