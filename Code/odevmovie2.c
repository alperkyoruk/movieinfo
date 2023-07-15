#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 512
#define MAX_GENRES 15
#define MAX_DIRECTORS 10
#define MAX_MOVIES 4000

unsigned int hash(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++) != 0) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

struct Budget {
    int budget;
    int year;
    char* title;
    char* titletype;
    char* top250;
    char* runtime;
};

struct Name {
    char** genre;
    char** directors;
    char* mustsee;
    double rating;
    char* url;
    char* votes;
    int year;
    char* title;
};

void free_movie_data(struct Budget* budget, struct Name* name, int count) {
    for (int i = 0; i < count; i++) {
        free(budget[i].title);
        free(budget[i].titletype);
        free(budget[i].top250);
        free(budget[i].runtime);

        for (int j = 0; name[i].genre[j] != NULL; j++) {
            free(name[i].genre[j]);
        }
        free(name[i].genre);

        free(name[i].directors[0]);
        free(name[i].directors);
        free(name[i].title);
        free(name[i].mustsee);
        free(name[i].url);
        free(name[i].votes);
    }
}

void print_budget_array(const struct Budget* budget, int count) {
    printf("Budget Array:\n");
    for (int i = 0; i < count; i++) {
        printf("Movie %d\n", i + 1);
        printf("Budget: %d\n", budget[i].budget);
        printf("Year: %d\n", budget[i].year);
        printf("Title: %s\n", budget[i].title);
        printf("Title Type: %s\n", budget[i].titletype);
        printf("Top 250: ");
        if (strcmp(budget[i].top250, "Y") == 0) {
            printf("Yes\n");
        } else {
            printf("No\n");
        }
        printf("Runtime: %s\n", budget[i].runtime);
        printf("\n");
    }
}

void print_name_array(const struct Name* name, int count) {
    printf("Name Array:\n");
    for (int i = 0; i < count; i++) {
        printf("Movie %d\n", i + 1);
        printf("Title: %s\n", name[i].title);
        printf("Genre: ");
        for (int j = 0; name[i].genre[j] != NULL; j++) {
            printf("%s", name[i].genre[j]);
            if (name[i].genre[j + 1] != NULL) {
                printf(", ");
            }
        }
        printf("\n");

        printf("Directors: %s\n", name[i].directors[0]);
        printf("Must See: ");
        if (strcmp(name[i].mustsee, "Y") == 0) {
            printf("Yes\n");
        } else {
            printf("No\n");
        }
        printf("Rating: %.1f\n", name[i].rating);
        printf("URL: %s\n", name[i].url);
        printf("Votes: %s\n", name[i].votes);

        printf("\n");
    }
}

struct GenreNode {
    char* genre;
    struct GenreNode* next;
};

struct GenreTable {
    struct GenreNode* buckets[MAX_GENRES];
};

void add_genre(struct GenreTable* table, const char* genre) {
    int index = abs(hash(genre)) % MAX_GENRES;
    struct GenreNode* node = table->buckets[index];
    while (node != NULL) {
        if (strcmp(node->genre, genre) == 0) {
            return;
        }
        node = node->next;
    }
    node = malloc(sizeof(struct GenreNode));
    node->genre = strdup(genre);
    node->next = table->buckets[index];
    table->buckets[index] = node;
}

void free_genre_table(struct GenreTable* table) {
    for (int i = 0; i < MAX_GENRES; i++) {
        struct GenreNode* node = table->buckets[i];
        while (node != NULL) {
            struct GenreNode* next = node->next;
            free(node->genre);
            free(node);
            node = next;
        }
    }
}

void print_genre_list(const struct Name* name, int count) {
    printf("Genre List:\n");
    struct GenreTable table = {0};
    for (int i = 0; i < count; i++) {
        for (int j = 0; name[i].genre[j] != NULL; j++) {
            add_genre(&table, name[i].genre[j]);
        }
    }
    for (int i = 0; i < MAX_GENRES; i++) {
        struct GenreNode* node = table.buckets[i];
        while (node != NULL) {
            printf("%s\n", node->genre);
            node = node->next;
        }
    }
    free_genre_table(&table);
}

void print_movie_list_by_year(const struct Budget* budget, int count) {
    printf("List of Movies Through the Years:\n");

    // Make a copy of the original budget array
    struct Budget* budget_copy = malloc(count * sizeof(struct Budget));
    memcpy(budget_copy, budget, count * sizeof(struct Budget));

    // Sort the copy of the budget array by year
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (budget_copy[j].year > budget_copy[j + 1].year) {
                struct Budget temp = budget_copy[j];
                budget_copy[j] = budget_copy[j + 1];
                budget_copy[j + 1] = temp;
            }
        }
    }

    // Print the sorted movie list
    for (int i = 0; i < count; i++) {
        printf("Year: %d\n", budget_copy[i].year);
        printf("Title: %s\n", budget_copy[i].title);
        printf("\n");
    }

    free(budget_copy); // Free the memory allocated for the copy
}

void print_movie_list_by_rating(struct Budget* budget, const struct Name* name, int count) {
    printf("List of Movies by Rating:\n");

    // Make a copy of the original name array
    struct Name* name_copy = malloc(count * sizeof(struct Name));
    memcpy(name_copy, name, count * sizeof(struct Name));

    // Sort the copy of the name array by rating
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (name_copy[j].rating < name_copy[j + 1].rating) {
                struct Name name_temp = name_copy[j];
                name_copy[j] = name_copy[j + 1];
                name_copy[j + 1] = name_temp;

                struct Budget budget_temp = budget[j];
                budget[j] = budget[j + 1];
                budget[j + 1] = budget_temp;
            }
        }
    }

    // Print the sorted movie list
    for (int i = 0; i < count; i++) {
        printf("Rating: %.1f\n", name_copy[i].rating);
        printf("Title: %s\n", budget[i].title);
        printf("\n");
    }

    free(name_copy); // Free the memory allocated for the copy
}



void print_movie_details(const struct Budget* budget, const struct Name* name, int count, const char* title) {
    bool movieFound = false;
    for (int i = 0; i < count; i++) {
        if (strcmp(budget[i].title, title) == 0) {
            printf("\nMovie Details:\n");
            printf("Title: %s\n", budget[i].title);
            printf("Year: %d\n", budget[i].year);
            printf("Title Type: %s\n", budget[i].titletype);
            printf("Top 250: ");
                if (strcmp(budget[i].top250, "Y") == 0) {
                    printf("Yes\n");
                }
                else {
                    printf("No\n");
                }
            printf("Runtime: %s\n", budget[i].runtime);
            printf("Genre: ");
            for (int j = 0; name[i].genre[j] != NULL; j++) {
                printf("%s", name[i].genre[j]);
                if (name[i].genre[j + 1] != NULL) {
                    printf(", ");
                }
            }
            printf("\n");
            printf("Directors: %s\n", name[i].directors[0]);
            printf("Must See: ");
                if (strcmp(name[i].mustsee, "Y") == 0) {
                printf("Yes\n");
                } 
                else {
                printf("No\n");
                }
            printf("Rating: %.1f\n", name[i].rating);
            printf("URL: %s\n", name[i].url);
            printf("Votes: %s\n", name[i].votes);
            movieFound = true;
            break;
        }
    }
    if (!movieFound) {
        printf("Movie not found.\n");
    }
}

void calculate_genre_frequency(const struct Name* name_array, int count) {
    struct GenreTable table = {0};

    for (int i = 0; i < count; i++) {
        for (int j = 0; name_array[i].genre[j] != NULL; j++) {
            add_genre(&table, name_array[i].genre[j]);
        }
    }

    printf("Genre Frequency:\n");
    for (int i = 0; i < MAX_GENRES; i++) {
        struct GenreNode* node = table.buckets[i];
        while (node != NULL) {
            unsigned int frequency = 0;
            for (int j = 0; j < count; j++) {
                for (int k = 0; name_array[j].genre[k] != NULL; k++) {
                    if (strcmp(name_array[j].genre[k], node->genre) == 0) {
                        frequency++;
                        break;
                    }
                }
            }
            printf("Genre: %s, Frequency: %u\n", node->genre, frequency);
            node = node->next;
        }
    }

    free_genre_table(&table);
}

void print_menu() {
    printf("\nMenu:\n");
    printf("1. List of budget array\n");
    printf("2. List of name array\n");
    printf("3. List of genres\n");
    printf("4. List of the Movie Through the Years\n");
    printf("5. List of the Movie Through the Scores\n");
    printf("6. All Information of a Single Movie\n");
    printf("7. Frequency of the Genres\n");
    printf("8. Exit\n");
}


int main() {
    struct Budget budget[MAX_MOVIES];
    struct Name name[MAX_MOVIES];
    int movie_count = 0;

    FILE* file = fopen("movie.txt", "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        
        char* token;
        token = strtok(line, ";");

        budget[movie_count].budget = atoi(token);

        token = strtok(NULL, ";");
        budget[movie_count].title = strdup(token);
        name[movie_count].title = strdup(token);

        token = strtok(NULL, ";");
        budget[movie_count].titletype = strdup(token);

        token = strtok(NULL, ";");
        name[movie_count].directors = malloc(MAX_DIRECTORS * sizeof(char*));
        name[movie_count].directors[0] = strdup(token);

        token = strtok(NULL, ";");
        name[movie_count].rating = atof(token);

        token = strtok(NULL, ";");
        budget[movie_count].runtime = strdup(token);

        token = strtok(NULL, ";");
        budget[movie_count].year = atoi(token);

        token = strtok(NULL, ";");
        char* genres = strdup(token);
        name[movie_count].genre = malloc(MAX_GENRES * sizeof(char*));
        int genre_count = 0;

        char* genre_start = genres;
        char* genre_end = strchr(genre_start, ',');
        while (genre_end != NULL && genre_count < MAX_GENRES) {
            *genre_end = '\0'; 
            name[movie_count].genre[genre_count] = strdup(genre_start);
            genre_count++;

            genre_start = genre_end + 1;
            genre_end = strchr(genre_start, ',');
        }

        name[movie_count].genre[genre_count] = strdup(genre_start);

        free(genres);

        name[movie_count].genre[genre_count + 1] = NULL;

        token = strtok(NULL, ";");
        name[movie_count].votes = strdup(token);

        token = strtok(NULL, ";");
        budget[movie_count].top250 = strdup(token);

        token = strtok(NULL, ";");
        name[movie_count].mustsee = strdup(token);

        token = strtok(NULL, "\n");
        name[movie_count].url = strdup(token);

        movie_count++;

        if (movie_count >= MAX_MOVIES) {
            printf("Maximum number of movies reached. Exiting.\n");
            break;
        }
    }

    fclose(file);
    
	int choice;
	while (1) {
    	print_menu();
    	printf("Enter your choice: ");
   			if (scanf("%d", &choice) != 1) {
        	printf("Invalid input. Please enter a number.\n");
        while (getchar() != '\n');
        continue;
    }
    getchar(); 

        switch (choice) {
            case 1:
                print_budget_array(budget, movie_count);
                return 0;
            case 2:
                print_name_array(name, movie_count);
                return 0;
            case 3:
                print_genre_list(name, movie_count);
                break;
            case 4:
                print_movie_list_by_year(budget, movie_count);
                return 0;
            case 5:
                print_movie_list_by_rating(budget, name, movie_count);
                return 0;
            case 6:
                printf("Enter a movie name: ");
                char movieName[MAX_LINE_LENGTH];
                fgets(movieName, sizeof(movieName), stdin);
                movieName[strcspn(movieName, "\n")] = '\0';
                print_movie_details(budget, name, movie_count, movieName);
                return 0;
            case 7:
                calculate_genre_frequency(name, movie_count);
                break;
            case 8:
                printf("Exiting the program.\n");
                free_movie_data(budget, name, movie_count);
                return 0;
            default:
                printf("Invalid choice. Please enter a number from 1 to 8.\n");
                break;
        }
    }


    return 0;
}
