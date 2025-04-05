#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <time.h>
#include <windows.h>
#include <locale.h>

#define COR "\x1b[31m"

#define CC "\x1b[0m"

#define COG "\x1b[32m"

int last_id = 0;

void clear_console() {
    printf("\033[H\033[J");
}

void save_tasks();

void load_tasks();

void menu();

void print_menu();

struct tasks{
    int id;
    int next_id;
    char name[50];
    char description[250];
    char deadline[50];
    time_t now;
    bool status;
    struct tasks *next;
};

int id_generator(){
    return ++last_id;
};

struct tasks *headTask = NULL;

void add_tasks(){
    struct tasks *newTask = (struct tasks*)malloc(sizeof(struct tasks));

    newTask->id = id_generator();

    printf("Enter name of the task:\n");
    fgets(newTask->name, 50, stdin);
    newTask->name[strcspn(newTask->name, "\n")] = '\0';

    printf("Enter description of the task:\n");
    fgets(newTask->description, 250, stdin);
    newTask->description[strcspn(newTask->description, "\n")] = '\0';

    printf("Enter deadline of the task (text):\n");
    fgets(newTask->deadline, 50, stdin);
    newTask->deadline[strcspn(newTask->deadline, "\n")] = '\0';

    newTask->now = time(NULL);

    newTask->status = false;
    newTask->next = NULL;
    
    if(headTask == NULL){
        headTask = newTask;
    }
    else{
        struct tasks *current = headTask;
        while (current->next != NULL){
            current = current->next;
        }
        current->next = newTask;
    }
    clear_console();
}

void print_tasks(){

    if (headTask == NULL) {
        clear_console();
        printf("Tasks list is empty.\n----------------------------\n");
        return;
    }

    struct tasks *current = headTask;
    while (current != NULL){
        printf("%sID: %d\n", current->status ? COG : COR, current->id);
        printf("Name: %s\n", current->name);
        printf("Description: %s\n", current->description);
        printf("Deadline: %s\n", current->deadline);
        printf("Created at: %s\n", ctime(&current->now));
        printf("Status: %s%s\n", current->status ? "Completed" : "Not completed", CC);
        printf("----------------------------\n");
        current = current->next;
    }
    print_menu();
}

void menu(){
    int choice;
    printf("Choose variant:\n 1. Add tasks.\n 2. Show tasks.\n 3. Save tasks.\n");
    scanf("%d", &choice);
    getchar();
    switch(choice){
        case 1:
            clear_console();
            add_tasks();
            break;
        case 2:
            clear_console();
            print_tasks();
            break;
        case 3:
            save_tasks();
        default:
            return;
    }
}

void print_menu(){
    int choice;
    bool flag = false;
    printf("Choose variant:\n 1. Change status of the task.\n 2. Get back to menu.\n");
    scanf("%d", &choice);
    getchar();
    switch (choice){
    case 1:
        printf("Choose id of tasks to change the status:\n");
        scanf("%d", &choice);
        getchar();
        struct tasks *current = headTask;
        while(!flag){
            if(current->id == choice){
                current->status = !current->status;
                flag = true;
            }
            else{
                current = current->next;
            }
        }
        break;
    case 2:
        clear_console();
    default:
        return;
    }
}

void save_tasks() {
    FILE *file = fopen("tasks.dat", "a");
    if (!file) {
        perror("Ошибка сохранения");
        return;
    }

    struct tasks *current = headTask;
    while (current != NULL) {
        current->next_id = (current->next) ? current->next->id : -1;
        
        fprintf(file, "%d|%d|%s|%s|%s|%ld|%d\n",
                current->id,
                current->next_id,
                current->name,
                current->description,
                current->deadline,
                (long)current->now,
                current->status);
        
        current = current->next;
    }
    
    fclose(file);
}

void load_tasks() {
    FILE *file = fopen("tasks.dat", "r");
    if (!file) return;

    struct tasks *temp_array[1000] = {NULL};
    int id, next_id, status;
    char name[50], description[250], deadline[16];
    long created_time;
    
    while (fscanf(file, "%d|%d|%49[^|]|%249[^|]|%15[^|]|%ld|%d\n",
                 &id, &next_id, name, description, deadline, &created_time, &status) == 7) {
        
        struct tasks *newTask = malloc(sizeof(struct tasks));
        newTask->id = id;
        newTask->next_id = next_id;
        strcpy(newTask->name, name);
        strcpy(newTask->description, description);
        strcpy(newTask->deadline, deadline);
        newTask->now = (time_t)created_time;
        newTask->status = status;
        newTask->next = NULL;
        
        temp_array[id] = newTask;

        if (id > last_id) {
            last_id = id;
        }
    }
    
    for (int i = 0; i < 100; i++) {
        if (temp_array[i]) {
            if (temp_array[i]->next_id != -1) {
                temp_array[i]->next = temp_array[temp_array[i]->next_id];
            }
        }
    }
    
    for (int i = 0; i < 100; i++) {
        if (temp_array[i]) {
            headTask = temp_array[i];
            break;
        }
    }
    
    fclose(file);
}

int main() {
    load_tasks();
    while (1) {
        menu();
    }
    return 0;
}