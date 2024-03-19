#include "main.h"

// Функция для очистки экрана
int clr(char **args) {
    printf("\033[2J\033[1;1H"); // Очистка экрана с помощью ANSI escape-последовательностей
    return 0;
}

// Функция для изменения текущей директории
int cd(char **args) {
    if (args[1] == NULL) {
        // Если отсутствует аргумент, выводим текущий путь
        char path[256];
        getcwd(path, 256); // Получаем текущий рабочий каталог
        printf("%s\n", path);
        return 0;
    } else {
        if (chdir(args[1]) == 0) {
            // При успешной смене директории обновляем переменную окружения PWD
            setenv("PWD", args[1], 1);
            printf("Changed directory to: %s\n", args[1]);
            return 0;
        } else {
            perror("chdir error"); // Вывод сообщения об ошибке
            return -1;
        }
    }
}

// Функция для завершения программы
int quit(char **args) {
    return 1;
}

// Функция для просмотра содержимого директории
int dir(char **args) {
    if (args[1] == NULL) {
        printf("Usage: dir <directory>\n");
        return 0;
    }

    DIR *dir = opendir(args[1]);
    if (!dir) {
        perror("opendir"); // Вывод сообщения об ошибке
        return -1;
    }

    printf("Contents of directory '%s':\n", args[1]);

    while (1) {
        struct dirent *entry = readdir(dir);
        if (entry == NULL) {
            break;
        }
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}

// Массив доступных команд
char* commands[] = {
        "cd",
        "clr",
        "dir",
        "environ",
        "echo",
        "help",
        "pause",
        "quit"
};

// Количество доступных команд
int numCommands = sizeof(commands) / sizeof(commands[0]);

// Описание каждой команды
char* commands_help[] = {
        "cd <directory> - change directory to <directory>. If argument <directory> isn't present, then output current one.",
        "clr - clears the screen",
        "dir <directory> - shows all files in <directory>",
        "environ - shows all environmental variables",
        "echo <anything> - outputs <anything>",
        "help - helpbox",
        "pause - makes a pause, until u press any key",
        "quit - exits from shell"
};
int echo(char **args) {
    int i = 1; // Начинаем с 1, так как args[0] содержит саму команду "echo"

    // Выполняем цикл, пока не достигнем конца списка аргументов (NULL)
    while (args[i] != NULL) {
        printf("%s ", args[i]); // Выводим текущий аргумент
        i++; // Переходим к следующему аргументу
    }

    printf("\n");
    return 0;// Переход на новую строку после вывода всех аргументов
}
extern char **environ;

int my_environ(char **args) {
    char **env = environ;

    // Перебираем и выводим все переменные окружения
    while (*env != NULL) {
        printf("%s\n", *env);
        env++;
    }
    return 0;
}

int help(char **args) {
    printf("List of available commands:\n");

    // Выводим список доступных команд с их описанием
    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        printf("%s - %s\n", commands[i], commands_help[i]);
    }

    return 0;
}

int my_pause(char **args) {
    printf("Press any key to continue\n");
    getchar(); // Ждем нажатия любой клавиши
    return 0;
}

// Массив указателей на функции обработки команд
int (*commandFuncs[])(char**) = {
        cd,
        clr,
        dir,
        my_environ,
        echo,  // Нельзя найти echo, нужно добавить его в массив
        help,
        my_pause,
        quit
};

// Основная функция программы
int main() {
    char input[100];
    char *args[20];

    while (1) {
        printf("Enter a command: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        int i = 0;
        args[i] = strtok(input, " \n");

        // Разбиваем введенную строку на аргументы
        while (args[i] != NULL && i < 19) {
            i++;
            args[i] = strtok(NULL, " \n");
        }
        args[i] = NULL;

        int found = 0;
        for (i = 0; i < numCommands; i++) {
            if (strcmp(args[0], commands[i]) == 0) {
                found = 1;
                break;
            }
        }

        if (found) {
            // Выполняем соответствующую функцию обработки команды
            if (commandFuncs[i](args) == 1) {
                break; // Выходим из цикла при необходимости
            }
        } else {
            printf("Command not found. Type 'help' for a list of available commands.\n");
        }
    }

    return 0;
}
