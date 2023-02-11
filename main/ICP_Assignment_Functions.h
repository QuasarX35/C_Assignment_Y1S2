// Introduction to C Programming Assignment
// They Kai Zhe
// TP062802
// APU1F2203CS

#define strdup _strdup
#define MAX_INPUT 200
#define MAX_LINE 1024
#define INVALID_FILE_MESSAGE "\n\t*** Error: File could not be opened. Please try again later. ***\n"
#define INVALID_INPUT_MESSAGE "\n\t*** Invalid input. Please try again. ***\n"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

FILE *staff_file, *customer_file, *item_file, *order_file;

struct staff
{
    char ID[5];
    char name[25];
    char username[21];
    char password[21];
    struct staff *s_link;
} * s_head, *s_last;

struct customer
{
    char ID[5];
    char name[25];
    char gender[7];
    char birth_date[11];
    char HP_num[16];
    char email[30];
    char username[21];
    char password[21];
    struct customer *c_link;
} * c_head, *c_last, *current_cus;

struct item
{
    char ID[5];
    char name[20];
    double price;
    char *desc;
    struct item *i_link;
} * i_head, *i_last;

struct order
{
    char order_ID[7];
    char cus_ID[5];
    char item_ID[5];
    char item_name[20];
    int qty;
    double total;
    struct order *o_link;
} * o_head, *o_last;

// extra functions
int clear_input();
void pause();
int string_check_int(const char *s_ptr);
int string_check_float(const char *s_ptr);
char *input_scanner(char *prompt, const int max_input, char mode, int cancelMode);
int choice_input(int range1, int range2, int cancelMode);
int date_checker(int day, int month, int year);
char **word_wrap(char *string, const int wrap_length, int *rows);
void string_char_replace(char *string, char char_to_replace, char replacing_char);

// main functions
int initialize_structs(int *staff_count, int *customer_count, int *item_count, int *order_count);
int main_menu();
struct staff *staff_login();
struct customer *customer_login();
void view_menu();
void register_new_customer(int *customer_count);

// staff functions
int staff_menu(char *current_user_name, int *item_count, int *customer_count, int *order_count);
void staff_view_items();
int staff_add_item(int *item_count);
void staff_search();
void staff_search_item();
void staff_search_order();
void staff_search_customer();
void staff_modify_item(int *item_count, int *order_count);

// customer functions
int customer_menu(struct customer *current_user_ptr, int *order_count, int item_count);
void customer_view_orders(char *current_userID);
int customer_add_order(int *order_count, int item_count, char *current_userID);
void customer_modify_order(int *order_count, char *current_userID);
void customer_checkout(char *current_userID, int *order_count);
void customer_view_profile(struct customer *current_user);
void customer_change_userpass(struct customer *current_user);

int clear_input()
{
    int c;
    // getchar clears the input buffer
    while ((c = getchar()) != EOF && c != '\n')
    {
    }
    return 1;
}

void pause()
{
    printf(
        "\n------------------------------"
        "\n Enter any key to continue . . . ");
    getchar();
    return;
}

int string_check_int(const char *s_ptr)
{
    while (*s_ptr != 0)
    {
        // check if the pointer to the first element of the string is a digit
        // isdigit will return 0 if a non-numeric character is passed as the parameter
        // then increment the pointer to go to the next element in the string
        if (isdigit(*s_ptr++) == 0)
            return -1;
    }
    return 1;
}

int string_check_float(const char *s_ptr)
{
    int dec_pt = 0;
    while (*s_ptr != 0)
    {
        // check for decimal points in float
        if (*s_ptr == '.')
        {
            *s_ptr++;
            dec_pt++;
            continue;
        }

        else if (isdigit(*s_ptr++) == 0 || dec_pt > 1)
            return -1;
    }
    return 1;
}

int string_check_empty(const char *s_ptr)
{
    while (*s_ptr != 0)
        if (isspace(*s_ptr++) == 0)
            return 1;
    return -1;
}

char *input_scanner(char *prompt, const int max_input, char mode, int cancelMode)
{
    int flag;
    char buffer[500];
    do
    {
        flag = 0;
        // replace all values in string with \0
        memset(buffer, 0, 500);
        printf("%s", prompt);

        // fgets returns null when EOF is entered, which is possible if a text file is piped into stdin in UNIX style operating systems
        if (fgets(buffer, 500, stdin) == NULL)
        {
            printf("\n\t*** Input error ***\n");
            flag = 1;
            continue;
        }

        // replace the first occurence of \n in the string with \0 to null terminate the string
        buffer[strcspn(buffer, "\n")] = '\0';

        // if user presses "enter" or any whitespace character only, continue loop
        if (string_check_empty(buffer) != 1)
        {
            printf(INVALID_INPUT_MESSAGE);
            flag = 1;
            continue;
        }

        // if cancel mode is enabled, when user enters 'c' only then will return NULL
        if (cancelMode == 1)
        {
            if (strncmp(buffer, "c", sizeof(buffer)) == 0)
                return NULL;
        }

        // check if input buffer has exceeded the maximum expected input
        if (strlen(buffer) > max_input)
        {
            printf("\n\t*** Error: input is too long ***\n");
            flag = 1;
            continue;
        }

        switch (mode)
        {
        // nothing will happen if string mode is specified as all the conditions have been met above
        case 's':
            break;

        // to check for integer
        case 'i':
            if (string_check_int(buffer) != 1)
            {
                printf(INVALID_INPUT_MESSAGE);
                flag = 1;
                break;
            }
            else
                break;

        // to check for float
        case 'f':
            if (string_check_float(buffer) != 1)
            {
                printf(INVALID_INPUT_MESSAGE);
                flag = 1;
                break;
            }
            else
                break;

        default:
            break;
        }
    } while (flag == 1);

    // allocate memory to a pointer to the input buffer, with size max expected input + 1 for the /0 at the end
    char *buf_ptr = (char *)malloc((strlen(buffer) + 1) * sizeof(char));

    // assign the value of the input buffer to the buffer pointer
    strcpy(buf_ptr, buffer);

    return buf_ptr;
}

int choice_input(int range1, int range2, int cancelMode)
{
    char prompt[60];
    // to format the prompt with range1 and range2
    snprintf(prompt, 60, "\n-----------------------\n Enter your choice (%d - %d): ", range1, range2);

    int num = range2, digits = 0;
    // count the number of digits in the 2nd range parameter
    while (num != 0)
    {
        digits++;
        num /= 10;
    }

    int output;
    do
    {
        char *input = input_scanner(prompt, digits, 'i', cancelMode);
        if (input == NULL)
            return -1;

        // to get the integer from the input
        sscanf(input, "%d", &output);

        // deleting the memory used by the temporary buffer in input_scanner
        free(input);

        // while loop will check the inputted number and print the error message if condition 1 is not met, then the loop will run again
    } while ((output < range1 || output > range2) && printf(INVALID_INPUT_MESSAGE));

    return output;
}

int date_checker(int day, int month, int year)
{
    if (year < 1900 || year > 2022)
        return -1;

    if (month < 1 || month > 12)
        return -1;

    if (month == 1 || month == 3 || month == 5 ||
        month == 7 || month == 8 || month == 10 ||
        month == 12)
    {
        if (day > 31)
            return -1;
    }

    if (month == 4 || month == 6 || month == 9 || month == 11)
    {
        if (day > 30)
            return -1;
    }

    if (month == 2 && year % 400 == 0 || year % 4 == 0)
    {
        if (day > 29)
            return -1;
    }

    else if (month == 2)
    {
        if (day > 28)
            return -1;
    }

    return 0;
}

char **word_wrap(char *string, const int wrap_length, int *rows)
{
    // setting the last character of the input string to space to allow the code in the for loop to print the last line
    int stringLen = strlen(string);
    string[stringLen + 1] = ' ';

    *rows = 0;
    char *line;
    char **lines = NULL;
    int line_start = 0, lines_size = 0;

    for (int i = wrap_length; i < stringLen + wrap_length; i += wrap_length)
    {
        while (string[i] != ' ')
            i--;
        // move one to the front to move to the character after space
        i++;

        // allocating memory for lines array whenever the number of rows hits a multiple of 10
        if (*rows == lines_size)
        {
            // reallocate extra 10 spaces for lines array
            lines_size += 10;
            char **temp_arr = (char **)realloc(lines, lines_size * sizeof(char *));
            if (temp_arr == NULL)
            {
                fprintf(stderr, "\n\t*** Reallocation failed, please restart program *** \n");
                return NULL;
            }
            lines = temp_arr;
        }

        // allocate memory for the line array
        line = (char *)malloc((i - line_start) * sizeof(char));

        int j;
        for (j = 0; line_start < i; line_start++, j++)
            line[j] = string[line_start];

        // null terminate the line
        line[j] = 0;

        // set the line in the lines array to the current line, then increment rows by 1
        lines[(*rows)++] = line;
    }
    return lines;
}

void string_char_replace(char *string, char char_to_replace, char replacing_char)
{
    int i = 0;
    while (*string != 0)
    {
        if (*string == char_to_replace)
        {
            *string = replacing_char;
        }
        *string++;
    }
}

int initialize_structs(int *staff_count, int *customer_count, int *item_count, int *order_count)
{
    // opening text files
    staff_file = fopen("resources/staffs.txt", "r");
    customer_file = fopen("resources/customers.txt", "r");
    item_file = fopen("resources/items.txt", "r");
    order_file = fopen("resources/orders.txt", "r");
    if (!staff_file || !customer_file || !item_file || !order_file)
    {
        fprintf(stderr, INVALID_FILE_MESSAGE);
        return -1;
    }

    // setting the file line counts to 0 and the head pointers of each struct to null
    *staff_count = 0, *customer_count = 0, *item_count = 0, *order_count = 0;
    s_head = NULL, c_head = NULL, i_head = NULL, o_head = NULL;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), staff_file) != NULL)
    {
        if (line[0] == 'I' || line[0] == '-')
            continue;

        // allocate memory for new node
        struct staff *newNode = (struct staff *)malloc(sizeof(struct staff));

        // storing data in new node using the data gotten from the file
        sscanf(line, "%s %s %s %s\n",
               newNode->ID, newNode->name,
               newNode->username, newNode->password);
        // replacing underscore with space for name
        string_char_replace(newNode->name, '_', ' ');

        // null terminating strings for safety
        (newNode->ID)[strlen(newNode->ID)] = 0;
        (newNode->name)[strlen(newNode->name)] = 0;
        (newNode->username)[strlen(newNode->username)] = 0;
        (newNode->password)[strlen(newNode->password)] = 0;

        newNode->s_link = NULL;

        // putting the new data into the first node of the staff linked list
        if (s_head == NULL)
        {
            s_head = newNode;
            s_last = s_head;
        }
        // putting the new data into the nodes after the first node of the staff linked list
        else
        {
            s_last->s_link = newNode;
            s_last = s_last->s_link;
        }

        (*staff_count)++;
    }

    while (fgets(line, sizeof(line), customer_file) != NULL)
    {
        if (line[0] == 'I' || line[0] == '-')
            continue;

        struct customer *newNode = (struct customer *)malloc(sizeof(struct customer));

        sscanf(line, "%s %s %s %s %s %s %s %s\n",
               newNode->ID, newNode->name,
               newNode->gender, newNode->birth_date,
               newNode->HP_num, newNode->email,
               newNode->username, newNode->password);
        string_char_replace(newNode->name, '_', ' ');

        (newNode->ID)[strlen(newNode->ID)] = 0;
        (newNode->name)[strlen(newNode->name)] = 0;
        (newNode->gender)[strlen(newNode->gender)] = 0;
        (newNode->birth_date)[strlen(newNode->birth_date)] = 0;
        (newNode->HP_num)[strlen(newNode->HP_num)] = 0;
        (newNode->email)[strlen(newNode->email)] = 0;
        (newNode->username)[strlen(newNode->username)] = 0;
        (newNode->password)[strlen(newNode->password)] = 0;

        newNode->c_link = NULL;

        if (c_head == NULL)
        {
            c_head = newNode;
            c_last = c_head;
        }
        else
        {
            c_last->c_link = newNode;
            c_last = c_last->c_link;
        }

        (*customer_count)++;
    }

    while (fgets(line, sizeof(line), item_file) != NULL)
    {
        if (line[0] == 'I' || line[0] == '-')
            continue;

        struct item *newNode = (struct item *)malloc(sizeof(struct item));

        char desc[MAX_LINE];
        sscanf(line,
               "%s %s %lf %s\n",
               newNode->ID, newNode->name,
               &newNode->price, desc);
        string_char_replace(newNode->name, '_', ' ');
        string_char_replace(desc, '_', ' ');

        newNode->i_link = NULL;

        // allocating memory for description in the global item linked list structure then copying the temporary desc variable into it
        newNode->desc = strdup(desc);

        (newNode->ID)[strlen(newNode->ID)] = 0;
        (newNode->name)[strlen(newNode->name)] = 0;
        (newNode->desc)[strlen(newNode->desc)] = 0;

        if (i_head == NULL)
        {
            i_head = newNode;
            i_last = i_head;
        }
        else
        {
            i_last->i_link = newNode;
            i_last = i_last->i_link;
        }

        (*item_count)++;
    }

    while (fgets(line, sizeof(line), order_file) != NULL)
    {
        if (line[3] == 'e' || line[0] == '-')
            continue;

        struct order *newNode = (struct order *)malloc(sizeof(struct order));

        sscanf(line,
               "%s %s %s %d %lf\n",
               newNode->order_ID, newNode->cus_ID,
               newNode->item_ID, &newNode->qty,
               &newNode->total);

        (newNode->order_ID)[strlen(newNode->order_ID)] = 0;
        (newNode->cus_ID)[strlen(newNode->cus_ID)] = 0;
        (newNode->item_ID)[strlen(newNode->item_ID)] = 0;

        // getting the item name from item list
        struct item *itemNode = i_head;
        while (itemNode != NULL)
        {
            // if item ID in order linked list matches item ID in item linked list, copy the name from item linked list to the order linked list
            if (strncmp(newNode->item_ID, itemNode->ID, sizeof(itemNode->ID)) == 0)
            {
                strcpy(newNode->item_name, itemNode->name);
                break;
            }
            itemNode = itemNode->i_link;
        }

        newNode->o_link = NULL;

        if (o_head == NULL)
        {
            o_head = newNode;
            o_last = o_head;
        }
        else
        {
            o_last->o_link = newNode;
            o_last = o_last->o_link;
        }

        (*order_count)++;
    }

    fclose(staff_file);
    fclose(customer_file);
    fclose(item_file);
    fclose(order_file);

    return 1;
}

int main_menu()
{
    system("@cls||clear");

    printf(
        "\n\t==============================="
        "\n\t    Welcome to Dosta Coffee"
        "\n\t==============================="
        "\n"
        "\n\t+------------------------------+"
        "\n\t| 1) Staff Login               |"
        "\n\t| 2) Customer Login            |"
        "\n\t| 3) Menu                      |"
        "\n\t| 4) New Customer Registration |"
        "\n\t|                              |"
        "\n\t| 5) Exit Program              |"
        "\n\t+------------------------------+"
        "\n");

    int choice = choice_input(1, 5, 0);

    return choice;
}

struct staff *staff_login()
{
    system("@cls||clear");

    printf(
        "\n\t==================="
        "\n\t    Staff Login"
        "\n\t==================="
        "\n"
        "\n\t* Enter \"c\" to cancel. *"
        "\n");

    struct staff *temp;
    do
    {
        // no memory allocation is required for this pointer because memory has already been allocated inside the input_scanner function and this pointer only points to the char pointer in the function
        char *username_input = input_scanner("\n\t| Username: ", MAX_INPUT, 's', 1);

        // check if user has entered c to cancel (input_scanner returns NULL when c is entered)
        if (username_input == NULL)
        {
            free(username_input);
            return NULL;
        }

        temp = s_head;
        // iterate through the nodes of staff linked list
        while (temp != NULL)
        {
            // check in the global staff link list for the entered username
            if (strncmp(username_input, temp->username, sizeof(temp->username)) == 0)
                // break if username input matches a username in staff linked list
                break;
            temp = temp->s_link;
        }

        // if username is not matched, will print error message and run the while loop again
        if (temp == NULL)
        {
            printf("\n\t*** Username not found. Please try again. ***\n");
            continue;
        }

        // remove the memory allocated to username_input by input_scanner because it is no longer being used
        free(username_input);

    } while (temp == NULL);

    // user only has 4 tries to input password
    for (int tries = 4; tries >= 0; tries--)
    {
        // if number of tries reached 0, exit program
        if (tries == 0)
            exit(EXIT_SUCCESS);

        char *password_input = input_scanner("\n\t| Password: ", MAX_INPUT, 's', 1);

        // if user enters c, will exit the login page
        if (password_input == NULL)
        {
            free(password_input);
            return NULL;
        }

        // check if entered password matches the password of the current user's username
        if (strncmp(password_input, temp->password, sizeof(temp->password)) == 0)
        {
            free(password_input);
            // return the address of the temporary node pointer if username is matched
            return temp;
        }

        printf("\n\t*** Incorrect password. %d attempts remaining. ***\n", tries - 1);
    }
    // 2 temporary mellocs used, 2 freed
}

struct customer *customer_login()
{
    system("@cls||clear");

    printf(
        "\n\t======================"
        "\n\t    Customer Login"
        "\n\t======================"
        "\n"
        "\n\t* Enter \"c\" to cancel. *"
        "\n");

    struct customer *temp;
    do
    {
        char *username_input = input_scanner("\n\t| Username: ", MAX_INPUT, 's', 1);

        if (username_input == NULL)
        {
            free(username_input);
            return NULL;
        }

        temp = c_head;
        while (temp != NULL)
        {
            if (strncmp(username_input, temp->username, sizeof(temp->username)) == 0)
                break;
            temp = temp->c_link;
        }

        if (temp == NULL)
        {
            printf("\n\t*** Username not found. Please try again. ***\n");
            continue;
        }

        free(username_input);

    } while (temp == NULL);

    for (int tries = 4; tries >= 0; tries--)
    {
        if (tries == 0)
            exit(EXIT_SUCCESS);

        char *password_input = input_scanner("\n\t| Password: ", MAX_INPUT, 's', 1);

        if (password_input == NULL)
        {
            free(password_input);
            return NULL;
        }

        if (strncmp(password_input, temp->password, sizeof(temp->password)) == 0)
        {
            free(password_input);
            return temp;
        }

        printf("\n\t*** Incorrect password. %d attempts remaining. ***\n", tries - 1);
    }
    // 2 temporary mallocs used, 2 freed
}

void view_menu()
{
    system("@cls||clear");

    printf(
        "\n\t\t\t\t============"
        "\n\t\t\t\t    Menu"
        "\n\t\t\t\t============"
        "\n"
        "\n\t+-------------------------------------------------------------+\n");

    int i = 1;
    struct item *temp = i_head;
    while (temp != NULL)
    {
        printf("\t| %d.\t%-45sRM %-5.2f |",
               i++, temp->name, temp->price);

        int rows;
        // outputting description into multiple lines with word wrap function
        char **lines = word_wrap(temp->desc, 42, &rows);

        // printing out lines of description
        for (int j = 0; j < rows; j++)
            printf("\n\t|       %-54s|", lines[j]);

        free(lines);

        printf("\n\t|\t\t\t\t\t\t\t      |\n");
        temp = temp->i_link;
    }
    printf("\t+-------------------------------------------------------------+\n");
    return;
}

void register_new_customer(int *customer_count)
{
    system("@cls||clear");

    printf(
        "\n\t===================="
        "\n\t    Registration    "
        "\n\t===================="
        "\n"
        "\n\t* Note: No spaces allowed except for name *"
        "\n\t* Enter \"c\" to cancel. *"
        "\n");

    // new customer name input, name can be the same because of different customer IDs
    char *new_name = input_scanner("\n\t| (Max. 24 chars) Full Name: ", 24, 's', 1);
    if (new_name == NULL)
    {
        free(new_name);
        return;
    }

    // new customer gender input
    char new_gender[7];
    printf(
        "\n\t| Gender:"
        "\n\t| 1. Male"
        "\n\t| 2. Female"
        "\n\t| 3. Prefer not to say"
        "\n");
    int choice = choice_input(1, 3, 1);
    // if user enters c to cancel (choice input return -1 when user enters c)
    if (choice == -1)
    {
        free(new_name);
        return;
    }
    switch (choice)
    {
    case 1:
        strcpy(new_gender, "Male");
        break;
    case 2:
        strcpy(new_gender, "Female");
        break;
    case 3:
        strcpy(new_gender, "Other");
        break;
    default:
        break;
    }

    // new customer birth date input
    int day, month, year, scan_output;
    char new_birth_date[11];
    do
    {
        char *input_buffer = input_scanner("\n\t| Date of Birth (DD/MM/YYYY): ", 10, 's', 1);
        if (input_buffer == NULL)
        {
            free(new_name);
            free(input_buffer);
            return;
        }

        scan_output = sscanf(input_buffer, "%d/%d/%d", &day, &month, &year);

        // check for proper date input and use date_checker function to validate inputted date
        if (scan_output < 3 || date_checker(day, month, year) != 0)
            continue;
        else
        {
            free(input_buffer);
            break;
        }
    } while (printf("\n\t*** Invalid input date. ***\n"));
    sprintf(new_birth_date, "%02d/%02d/%d", day, month, year);

    // new customer mobile number input
    char *new_HP_num;
    do
    {
        new_HP_num = input_scanner("\n\t| (Max. length: 16) Mobile Number: ", 16, 's', 1);
        if (new_HP_num == NULL)
        {
            free(new_name);
            free(new_HP_num);
            return;
        }
        // if user entered space in the input, loop again
        if (strchr(new_HP_num, ' ') != NULL)
            continue;
        else
            break;
    } while (printf(INVALID_INPUT_MESSAGE));

    // new customer email input
    char *new_email;
    do
    {
        new_email = input_scanner("\n\t| (Max. length: 30) Email: ", 30, 's', 1);
        if (new_email == NULL)
        {
            free(new_name);
            free(new_HP_num);
            free(new_email);
            return;
        }
        if (strchr(new_email, ' ') != NULL)
            continue;
        else
            break;
    } while (printf(INVALID_INPUT_MESSAGE));

    printf(
        "\n\n Please check that all relevant details are correct:"
        "\n----------------------------------"
        "\n Full Name: %s"
        "\n Gender: %s"
        "\n Date of Birth: %s"
        "\n Mobile Number: %s"
        "\n Email: %s"
        "\n----------------------------------",
        new_name, new_gender, new_birth_date, new_HP_num, new_email);

    printf("\n Enter \"y\" if yes or any other key to cancel: ");
    char confirm_choice = getchar();
    clear_input();
    if (confirm_choice == 'Y' || confirm_choice == 'y')
    {
        (*customer_count)++;
        // allocating memory for new node
        struct customer *newNode = (struct customer *)malloc(sizeof(struct customer));

        // storing data in new node
        sprintf(newNode->ID, "C%03d", *customer_count);
        strcpy(newNode->name, new_name);
        strcpy(newNode->gender, new_gender);
        strcpy(newNode->birth_date, new_birth_date);
        strcpy(newNode->HP_num, new_HP_num);
        strcpy(newNode->email, new_email);
        sprintf(newNode->username, "dostaCUS@%s", newNode->ID);
        sprintf(newNode->password, "%s@%d%02d%02d", newNode->ID, year, month, day);

        // null terminating strings in new node
        (newNode->ID)[strlen(newNode->ID)] = 0;
        (newNode->name)[strlen(newNode->name)] = 0;
        (newNode->gender)[strlen(newNode->gender)] = 0;
        (newNode->birth_date)[strlen(newNode->birth_date)] = 0;
        (newNode->HP_num)[strlen(newNode->HP_num)] = 0;
        (newNode->email)[strlen(newNode->email)] = 0;
        (newNode->username)[strlen(newNode->username)] = 0;
        (newNode->password)[strlen(newNode->password)] = 0;
        newNode->c_link = NULL;

        // if there are no registered customers yet
        if (c_head == NULL)
        {
            c_head = newNode;
            c_last = c_head;
        }
        else
        {
            // add the new customer to the end of the linked list
            c_last->c_link = newNode;
            c_last = c_last->c_link;
        }

        printf(
            "\n----------------------------------"
            "\n Your temporary username is: %s"
            "\n and temporary passsword is: %s"
            "\n----------------------------------"
            "\n * Note: Change to a more secure password to prevent unauthorized access. *"
            "\n",
            newNode->username, newNode->password);

        customer_file = fopen("resources/customers.txt", "a");
        if (customer_file == NULL)
        {
            fprintf(stderr, INVALID_FILE_MESSAGE);
            pause();
            return;
        }
        // replacing spaces with underscores for name
        string_char_replace(new_name, ' ', '_');

        // adding new customer details to customer text file
        fprintf(customer_file,
                "%-7s %-24s %-11s %-15s %-15s %-30s %-20s %s\n",
                newNode->ID, new_name,
                newNode->gender, newNode->birth_date,
                newNode->HP_num, newNode->email,
                newNode->username, newNode->password);
        fclose(customer_file);
        printf("\n\t* Customer registered successfully. *\n");
    }
    // 5 temporary mallocs used, 5 freed
    free(new_name);
    free(new_HP_num);
    free(new_email);
    return;
}

int staff_menu(char *current_user_name, int *item_count, int *customer_count, int *order_count)
{
    system("@cls||clear");

    // printing current logged in staff's name with variable aesthetic line length
    printf("\n\t");
    for (int i = 0; i < strlen(current_user_name) + 15; i++)
        printf("=");

    printf("\n\t    Staff: %s\n\t", current_user_name);

    for (int i = 0; i < strlen(current_user_name) + 15; i++)
        printf("=");

    printf(
        "\n"
        "\n\t+--------------------------+"
        "\n\t| 1) View Items            |"
        "\n\t| 2) Add Item              |"
        "\n\t| 3) Search                |"
        "\n\t| 4) Modify / Delete Item  |"
        "\n\t| 5) Register New Customer |"
        "\n\t|                          |"
        "\n\t| 6) Logout                |"
        "\n\t+--------------------------+"
        "\n");

    int choice = choice_input(1, 6, 0);
    switch (choice)
    {
    case 1:
        staff_view_items();
        pause();
        break;

    case 2:
        while (staff_add_item(item_count) != 1)
        {
        }
        break;

    case 3:
        staff_search();
        break;

    case 4:
        staff_modify_item(item_count, order_count);
        pause();
        break;

    case 5:
        register_new_customer(customer_count);
        pause();
        break;

    case 6:
        return 1;

    default:
        break;
    }

    item_file = fopen("resources/items.txt", "w");
    order_file = fopen("resources/orders.txt", "w");

    fprintf(item_file,
            "ID      Name                 Price       Description\n"
            "-----------------------------------------------------------------------------------------------------------------------------\n");

    struct item *temp = i_head;
    while (temp != NULL)
    {
        char *formatted_name = strdup(temp->name);
        char *formatted_desc = strdup(temp->desc);
        string_char_replace(formatted_name, ' ', '_');
        string_char_replace(formatted_desc, ' ', '_');

        fprintf(item_file,
                "%-7s %-20s %-11.2f %s\n",
                temp->ID, formatted_name,
                temp->price, formatted_desc);

        // 2 strdup used, 2 freed
        free(formatted_name);
        free(formatted_desc);

        temp = temp->i_link;
    }

    fprintf(order_file,
            "Order ID    Customer ID  Item ID  Qty   Total(RM)\n"
            "-----------------------------------------------------\n");

    struct order *ordNode = o_head;
    while (ordNode != NULL)
    {
        fprintf(order_file,
                "%-11s %-12s %-8s %-5d %.2f\n",
                ordNode->order_ID, ordNode->cus_ID,
                ordNode->item_ID, ordNode->qty,
                ordNode->total);
        ordNode = ordNode->o_link;
    }

    fclose(item_file);
    fclose(order_file);
    return 0;
}

void staff_view_items()
{
    system("@cls||clear");

    printf(
        "\n\t================="
        "\n\t    Inventory"
        "\n\t================="
        "\n"
        "\n ID      Name                 Price(RM)   Description"
        "\n-----------------------------------------------------------------------------------------------------\n");

    struct item *temp = i_head;
    while (temp != NULL)
    {
        printf(" %-7s %-20s %-11.2f %s\n",
               temp->ID, temp->name,
               temp->price, temp->desc);
        temp = temp->i_link;
    }
    return;
}

int staff_add_item(int *item_count)
{
    system("@cls||clear");

    printf(
        "\n\t===================="
        "\n\t    Add New Item"
        "\n\t===================="
        "\n"
        "\n\t* Enter \"c\" to cancel. *"
        "\n");

    char *new_name = input_scanner("\n\t| (Max. 20 chars) Item Name: ", 20, 's', 1);
    if (new_name == NULL)
    {
        free(new_name);
        return 1;
    }

    double new_price;
    char *input_buffer = input_scanner("\n\t| Item Price (RM): ", 7, 'f', 1);
    if (input_buffer == NULL)
    {
        free(new_name);
        free(input_buffer);
        return 1;
    }
    sscanf(input_buffer, "%lf", &new_price);
    free(input_buffer);

    char *new_desc = input_scanner("\n\t| (Max. 120 chars) Item Description: ", 120, 's', 1);
    if (new_desc == NULL)
    {
        free(new_name);
        free(new_desc);
        return 1;
    }

    printf(
        "\nAdd this new item to inventory?"
        "\n----------------------------------"
        "\n Item Name: %s"
        "\n Item Price (RM): %.2f"
        "\n Item Description: %s"
        "\n----------------------------------",
        new_name, new_price, new_desc);

    printf("\n Enter \"y\" if yes or any other key to cancel: ");
    char confirm_choice = getchar();
    clear_input();
    if (confirm_choice == 'Y' || confirm_choice == 'y')
    {
        (*item_count)++;

        struct item *newNode = (struct item *)malloc(sizeof(struct item));
        sprintf(newNode->ID, "D%03d", *item_count);
        strcpy(newNode->name, new_name);
        newNode->price = new_price;
        newNode->desc = new_desc;

        (newNode->ID)[strlen(newNode->ID)] = 0;
        (newNode->name)[strlen(newNode->name)] = 0;
        (newNode->desc)[strlen(newNode->desc)] = 0;
        newNode->i_link = NULL;

        if (i_head == NULL)
        {
            i_head = newNode;
            i_last = i_head;
        }
        else
        {
            i_last->i_link = newNode;
            i_last = i_last->i_link;
        }

        printf("\n\t* Item added successfully. *\n");
        // 2 temporary mallocs used, 2 freed
        // malloc used by description input will be used for the lifetime of the program, so it is not needed to be freed
        free(new_name);
    }
    else
    {
        free(new_name);
        free(new_desc);
        return 1;
    }

    printf(
        "\n Would you like to add another item?"
        "\n----------------------------------"
        "\n Enter \"y\" if yes or any other key to cancel: ");
    confirm_choice = getchar();
    clear_input();
    if (confirm_choice == 'Y' || confirm_choice == 'y')
        // will run this function again
        return 0;
    else
        return 1;
}

void staff_search()
{
    int choice;
    do
    {
        system("@cls||clear");
        printf(
            "\n\t=============="
            "\n\t    Search"
            "\n\t=============="
            "\n"
            "\n\t+-------------------------+"
            "\n\t| 1) Items                |"
            "\n\t| 2) Orders               |"
            "\n\t| 3) Customers            |"
            "\n\t|                         |"
            "\n\t| 4) Return to Staff Page |"
            "\n\t+-------------------------+"
            "\n");

        choice = choice_input(1, 4, 0);
        switch (choice)
        {
        case 1:
            staff_search_item();
            pause();
            break;
        case 2:
            staff_search_order();
            pause();
            break;
        case 3:
            staff_search_customer();
            pause();
            break;
        case 4:
            break;
        default:
            break;
        }
    } while (choice != 4);
    return;
}

void staff_search_item()
{
    system("@cls||clear");

    item_file = fopen("resources/items.txt", "r");
    if (item_file == NULL)
    {
        fprintf(stderr, INVALID_FILE_MESSAGE);
        pause();
        return;
    }

    int choice;
    printf(
        "\n\t==================="
        "\n\t    Search Item"
        "\n\t==================="
        "\n"
        "\n\t* Enter \"c\" to cancel. *"
        "\n");

    char *search = input_scanner("\n\t| Enter the term to search: ", MAX_LINE, 's', 1);
    if (search == NULL)
    {
        free(search);
        return;
    }

    printf("\n ID      Name                 Price   Description\n"
           "-----------------------------------------------------------------------------------------------------------------------------\n");

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), item_file) != NULL)
    {
        // ignore headers
        if (line[0] == 'I' || line[0] == '-')
            continue;

        // replacing underscore with space
        string_char_replace(line, '_', ' ');

        // null terminating the line
        line[strlen(line)] = 0;

        // search for inputted search term in line and print out the line if found
        if (strstr(line, search) != NULL)
            printf(" %s", line);
    }
    fclose(item_file);
    // 1 temporary malloc used, 1 freed
    free(search);
    return;
}

void staff_search_order()
{
    system("@cls||clear");

    order_file = fopen("resources/orders.txt", "r");
    if (order_file == NULL)
    {
        fprintf(stderr, INVALID_FILE_MESSAGE);
        pause();
        return;
    }

    int choice;
    printf(
        "\n\t===================="
        "\n\t    Search Order"
        "\n\t===================="
        "\n"
        "\n\t* Enter \"c\" to cancel. *"
        "\n");

    char *search = input_scanner("\n\t| Enter the term to search: ", MAX_LINE, 's', 1);
    if (search == NULL)
    {
        free(search);
        return;
    }

    printf("\n Order ID    Customer ID  Item ID  Qty   Total(RM)\n"
           "-----------------------------------------------------\n");

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), order_file) != NULL)
    {
        if (line[3] == 'e' || line[0] == '-')
            continue;

        line[strlen(line)] = 0;

        if (strstr(line, search) != NULL)
            printf(" %s", line);
    }
    fclose(order_file);
    // 1 temporary malloc used, 1 freed
    free(search);
    return;
}

void staff_search_customer()
{
    system("@cls||clear");

    customer_file = fopen("resources/customers.txt", "r");
    if (customer_file == NULL)
    {
        fprintf(stderr, INVALID_FILE_MESSAGE);
        pause();
        return;
    }

    int choice;
    printf(
        "\n\t======================="
        "\n\t    Search Customer"
        "\n\t======================="
        "\n"
        "\n\t* Enter \"c\" to cancel. *"
        "\n");

    char *search = input_scanner("\n\t| Enter the term to search: ", MAX_LINE, 's', 1);
    if (search == NULL)
    {
        free(search);
        return;
    }

    printf("\n ID      Name                     Gender      Date of Birth   HP Number       Email                          Username             Password\n"
           "------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), customer_file) != NULL)
    {
        if (line[0] == 'I' || line[0] == '-')
            continue;

        string_char_replace(line, '_', ' ');

        line[strlen(line)] = 0;

        if (strstr(line, search) != NULL)
            printf(" %s", line);
    }
    fclose(customer_file);
    // 1 temporary malloc used, 1 freed
    free(search);
    return;
}

void staff_modify_item(int *item_count, int *order_count)
{
    system("@cls||clear");
    printf(
        "\n\t============================"
        "\n\t    Modify / Delete Item"
        "\n\t============================"
        "\n"
        "\n No.\tName                Price(RM)   Item Desc"
        "\n---------------------------------------------------------------------------------------------\n");

    // if there are no items in list
    if (i_head == NULL)
    {
        printf("\n\t*** No items in file. Please add an item first. ***\n");
        return;
    }

    struct item *temp = i_head, *prev, *next;

    // displaying items to delete
    int i = 1;
    while (temp != NULL)
    {
        printf(" %d.\t%-19s %-11.2f %s\n",
               i++, temp->name,
               temp->price, temp->desc);
        temp = temp->i_link;
    }
    printf("\n %d.\tReturn to Staff Page\n", i);

    int choice = choice_input(1, i, 0);
    if (choice == i)
        return;

    temp = i_head, prev = NULL, next = NULL;
    for (int j = 1; j < choice; j++)
    {
        // storing the address of the previous node
        prev = temp;
        // moving the temporary pointer to the next node
        temp = temp->i_link;
    }
    // storing the address of the next node
    next = temp->i_link;

    char *new_name = temp->name;
    char *input_buffer;
    double new_price = temp->price;
    // strdup allocates memory to new_desc and copies the value of temp->desc to it
    char *new_desc = strdup(temp->desc);
    do
    {
        system("@cls||clear");
        printf("\n Select attribute of item to modify: \n"
               "----------------------------------"
               "\n 1. Item Name: %s"
               "\n 2. Item Price(RM): %.2f"
               "\n 3. Item Description: %s"
               "\n"
               "\n 4. Delete Item"
               "\n 5. Save Changes"
               "\n 6. Cancel\n"
               "----------------------------------\n",
               new_name, new_price, new_desc);

        choice = choice_input(1, 6, 0);
        // create temporary order linked list pointers to remove any orders where the item is deleted
        struct order *o_current, *o_prev;
        switch (choice)
        {
        // 1: changing the name of the item
        case 1:
            new_name = input_scanner("\n\t| (Max. 20 chars) New Item Name: ", 20, 's', 0);
            break;

        // 2: changing the price of the item
        case 2:
            input_buffer = input_scanner("\n\t| New Item Price (RM): ", 7, 'f', 0);
            sscanf(input_buffer, "%lf", &new_price);
            break;

        // 3: changing the description of the item
        case 3:
            new_desc = input_scanner("\n\t| (Max. 120 chars) New Item Description: ", 120, 's', 0);
            break;

        // 4: deleting the item
        case 4:
            // removing the item from order list
            o_current = o_head;
            o_prev = NULL;
            while (o_current != NULL)
            {
                // if item ID is found in the order list, remove it
                if (strncmp(temp->ID, o_current->item_ID, sizeof(o_current->item_ID)) == 0)
                {
                    (*order_count)--;
                    // if the node to be deleted is the first node of the list
                    if (o_current == o_head)
                    {
                        o_head = o_head->o_link;
                        free(o_current);
                        o_current = o_head;
                        continue;
                    }
                    // if the node to be deleted is after the first node of the list
                    else
                    {
                        o_prev->o_link = o_current->o_link;
                        free(o_current);
                        o_current = o_prev->o_link;
                        continue;
                    }
                }
                else
                {
                    o_prev = o_current;
                    o_current = o_current->o_link;
                }
            }
            o_last = o_current;

            // create temporary node pointer "ordIDtemp" for reordering the order IDs
            struct order *ordIDtemp = o_head;
            i = 1;
            while (ordIDtemp != NULL)
            {
                sprintf(ordIDtemp->order_ID, "ORD%03d", i++);
                ordIDtemp = ordIDtemp->o_link;
            }

            // removing item from item list
            (*item_count)--;

            if (temp == i_head)
                i_head = next;
            else if (next == NULL)
            {
                prev->i_link = NULL;
                i_last = prev;
            }
            else
                prev->i_link = next;

            free(temp);

            // create temporary node pointer "itemIDtemp" for reordering the item IDs
            struct item *itemIDtemp = i_head;
            i = 1;
            while (itemIDtemp != NULL)
            {
                sprintf(itemIDtemp->ID, "D%03d", i++);
                itemIDtemp = itemIDtemp->i_link;
            }

            printf("\n\t* Item deleted successfully. *\n");
            return;

        // 5: saving the changes made to the item
        case 5:
            strcpy(temp->name, new_name);
            // null terminating the name string
            (temp->name)[strlen(temp->name)] = 0;

            temp->price = new_price;

            strcpy(temp->desc, new_desc);
            (temp->desc)[strlen(temp->desc)] = 0;

            // 3 temporary mallocs used, 3 freed
            free(new_name);
            free(new_desc);

            printf("\n\t* Item modified successfully. *\n");
            return;

        // 6: return to the staff page
        case 6:
            return;

        default:
            break;
        }
    } while (choice < 4);
}

int customer_menu(struct customer *current_user_ptr, int *order_count, int item_count)
{
    system("@cls||clear");

    printf("\n\t");
    for (int i = 0; i < strlen(current_user_ptr->name) + 18; i++)
        printf("=");

    printf("\n\t    Customer: %s\n\t", current_user_ptr->name);

    for (int i = 0; i < strlen(current_user_ptr->name) + 18; i++)
        printf("=");

    printf(
        "\n"
        "\n\t+-----------------------------+"
        "\n\t| 1) View Menu                |"
        "\n\t| 2) View Orders              |"
        "\n\t| 3) Add Order                |"
        "\n\t| 4) Modify Order             |"
        "\n\t| 5) Proceed to Checkout      |"
        "\n\t| 6) View Profile             |"
        "\n\t| 7) Change Username/Password |"
        "\n\t|                             |"
        "\n\t| 8) Back to Login            |"
        "\n\t+-----------------------------+"
        "\n");

    int choice = choice_input(1, 8, 0);
    int add_order;
    switch (choice)
    {
    case 1:
        view_menu();
        pause();
        break;

    case 2:
        customer_view_orders(current_user_ptr->ID);
        pause();
        break;

    case 3:
        while (customer_add_order(order_count, item_count, current_user_ptr->ID) != 1)
        {
        }
        break;

    case 4:
        customer_modify_order(order_count, current_user_ptr->ID);
        pause();
        break;

    case 5:
        customer_checkout(current_user_ptr->ID, order_count);
        pause();
        break;

    case 6:
        customer_view_profile(current_user_ptr);
        pause();
        break;

    case 7:
        customer_change_userpass(current_user_ptr);
        pause();
        break;

    case 8:
        return 1;

    default:
        break;
    }

    // saving any overwritten details from the order and customer linked lists into the relevant text files
    order_file = fopen("resources/orders.txt", "w");
    customer_file = fopen("resources/customers.txt", "w");
    if (order_file == NULL || customer_file == NULL)
    {
        fprintf(stderr, INVALID_FILE_MESSAGE);
        pause();
        return 1;
    }

    fprintf(order_file,
            "Order ID    Customer ID  Item ID  Qty   Total(RM)\n"
            "-----------------------------------------------------\n");

    struct order *ordNode = o_head;
    while (ordNode != NULL)
    {
        fprintf(order_file,
                "%-11s %-12s %-8s %-5d %.2f\n",
                ordNode->order_ID, ordNode->cus_ID,
                ordNode->item_ID, ordNode->qty,
                ordNode->total);
        ordNode = ordNode->o_link;
    }

    fprintf(customer_file,
            "ID      Name                     Gender      Date of Birth   HP Number       Email                          Username             Password\n"
            "------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    struct customer *cusNode = c_head;
    while (cusNode != NULL)
    {
        char *formatted_name = strdup(cusNode->name);
        string_char_replace(formatted_name, ' ', '_');

        fprintf(customer_file,
                "%-7s %-24s %-11s %-15s %-15s %-30s %-20s %s\n",
                cusNode->ID, formatted_name,
                cusNode->gender, cusNode->birth_date,
                cusNode->HP_num, cusNode->email,
                cusNode->username, cusNode->password);
        cusNode = cusNode->c_link;

        free(formatted_name);
    }
    fclose(order_file);
    fclose(customer_file);

    return 0;
}

void customer_view_orders(char *current_userID)
{
    system("@cls||clear");

    printf(
        "\n\t=============="
        "\n\t    Orders"
        "\n\t=============="
        "\n"
        "\n Qty\tItem\t\t    Total(RM)"
        "\n------------------------------------------\n");

    // get the order ID, item ID, quantity and total of each order by the customer from the global order linked list
    struct order *ordNode = o_head;
    while (ordNode != NULL)
    {
        // if the current logged in customer's ID is found in the order list, print the customer's order
        if (strncmp(current_userID, ordNode->cus_ID, sizeof(ordNode->cus_ID)) == 0)
            printf(" x %d\t%-19s RM %.2f\n",
                   ordNode->qty, ordNode->item_name, ordNode->total);

        // set the order linked list node to point to the next node
        ordNode = ordNode->o_link;
    }
}

int customer_add_order(int *order_count, int item_count, char *current_userID)
{
    view_menu();

    printf("\n\t* Enter \"c\" to cancel. *\n");

    int choice = choice_input(1, item_count, 1);
    // if user enters c
    if (choice == -1)
        return 1;

    struct item *chosenNode = i_head;
    // iterating through the linked list until the chosen item is found
    for (int i = 1; i < choice; i++)
        chosenNode = chosenNode->i_link;

    int input_qty;
    char *input_buffer = input_scanner("\n-----------------------\n Quantity: ", 3, 'i', 1);
    if (input_buffer == NULL)
    {
        free(input_buffer);
        return 1;
    }
    sscanf(input_buffer, "%d", &input_qty);
    free(input_buffer);

    printf("\n Add the following item to cart?"
           "\n----------------------------------"
           "\n %s"
           "\n %s"
           "\n RM %.2f"
           "\n Qty: %d"
           "\n----------------------------------",
           chosenNode->name, chosenNode->desc,
           chosenNode->price, input_qty);

    printf("\n Enter \"y\" if yes or any other key to cancel: ");
    char confirm_choice = getchar();
    clear_input();
    if (confirm_choice == 'Y' || confirm_choice == 'y')
    {
        struct order *dupeItem = o_head;
        while (dupeItem != NULL)
        {
            // loop through the customer's orders in the order list
            if (strncmp(current_userID, dupeItem->cus_ID, sizeof(dupeItem->cus_ID)) == 0)
            {
                // if the chosen item is found in the order list
                if (strncmp(chosenNode->ID, dupeItem->item_ID, sizeof(dupeItem->item_ID)) == 0)
                    break;
            }
            dupeItem = dupeItem->o_link;
        }

        // if the customer orders an item that already exists in the customer's order list
        if (dupeItem != NULL)
        {
            // update the customer's order in the order list
            dupeItem->qty += input_qty;
            dupeItem->total += (chosenNode->price) * input_qty;
        }
        // if chosen item has not been ordered by the customer yet
        else
        {
            (*order_count)++;
            struct order *newOrd = (struct order *)malloc(sizeof(struct order));

            sprintf(newOrd->order_ID, "ORD%03d", *order_count);
            strcpy(newOrd->cus_ID, current_userID);
            strcpy(newOrd->item_ID, chosenNode->ID);
            strcpy(newOrd->item_name, chosenNode->name);
            newOrd->qty = input_qty;
            newOrd->total = (chosenNode->price) * input_qty;
            newOrd->o_link = NULL;

            if (o_head == NULL)
            {
                o_head = newOrd;
                o_last = o_head;
            }
            else
            {
                o_last->o_link = newOrd;
                o_last = o_last->o_link;
            }
        }
    }
    else
        return 1;

    printf("\n\t* Order added successfully. *\n"
           "\n Would you like to add another order?"
           "\n----------------------------------"
           "\n Enter \"y\" if yes or any other key to cancel: ");
    confirm_choice = getchar();
    clear_input();
    if (confirm_choice == 'Y' || confirm_choice == 'y')
        return 0;
    else
        return 1;
}

void customer_modify_order(int *order_count, char *current_userID)
{
    system("@cls||clear");

    printf(
        "\n\t===================="
        "\n\t    Modify Order"
        "\n\t===================="
        "\n"
        "\n No.\tQty\tItem\t\t    Total(RM)"
        "\n------------------------------------------\n");

    int i = 1;
    struct order *ordNode = o_head;
    while (ordNode != NULL)
    {
        if (strncmp(current_userID, ordNode->cus_ID, sizeof(ordNode->cus_ID)) == 0)
            printf(" %d.\tx %d\t%-19s RM %.2f\n",
                   i++, ordNode->qty, ordNode->item_name, ordNode->total);
        ordNode = ordNode->o_link;
    }
    printf("\n %d.\tReturn to Options Menu\n", i);

    int choice = choice_input(1, i, 0);
    if (choice == i)
        return;

    struct order *prev, *next;
    ordNode = o_head;
    prev = NULL, next = NULL;
    int j = 1;
    while (ordNode != NULL)
    {
        if (strncmp(current_userID, ordNode->cus_ID, sizeof(ordNode->cus_ID)) == 0)
        {
            if (j == choice)
                break;
            else
                j++;
        }
        prev = ordNode;
        ordNode = ordNode->o_link;
    }
    next = ordNode->o_link;

    char *input_buffer;
    int new_qty = ordNode->qty;
    double new_total = ordNode->total;
    do
    {
        system("@cls||clear");
        printf("\n Change quantity of order: \n"
               "----------------------------------"
               "\n Item Name: %s"
               "\n Item Quantity: %d"
               "\n Total Amount Payable (RM): %.2f"
               "\n"
               "\n 1. Change Quantity"
               "\n 2. Save Changes"
               "\n 3. Cancel\n"
               "----------------------------------\n",
               ordNode->item_name, new_qty, new_total);

        choice = choice_input(1, 3, 0);
        switch (choice)
        {
        // 1: changing the quantity of the order
        case 1:
            printf("\n\t* Enter 0 to delete order *\n");
            input_buffer = input_scanner("\n\t| New Quantity: ", 3, 'i', 0);
            sscanf(input_buffer, "%d", &new_qty);
            free(input_buffer);

            // getting the price of the item in the chosen order and updating the new total
            struct item *itemNode = i_head;
            while (itemNode != NULL)
            {
                if (strncmp(ordNode->item_ID, itemNode->ID, sizeof(itemNode->ID)) == 0)
                {
                    new_total = new_qty * (itemNode->price);
                    break;
                }
                itemNode = itemNode->i_link;
            }
            break;

        // 2: saving the changes
        case 2:
            if (new_qty == 0)
            {
                (*order_count)--;
                // deallocate the memory used by the chosen element in the linked list
                free(ordNode);

                // if the temporary pointer points to the first node of the linked list, point the head of the linked list to the 2nd node
                if (prev == NULL)
                    o_head = next;

                // if the temporary pointer points to the last node of the linked list, set the pointer of the previous node to NULL and the last node of the linked list to the previous node
                else if (next == NULL)
                {
                    prev->o_link = NULL;
                    o_last = prev;
                }

                // if the chosen node is between the first and last node of the linked list, set the next link of the previous node to point to the next node
                else
                    prev->o_link = next;

                // create temporary node pointer "IDtemp" for reordering the order IDs
                struct order *IDtemp = o_head;
                i = 1;
                while (IDtemp != NULL)
                {
                    sprintf(IDtemp->order_ID, "ORD%03d", i++);
                    IDtemp = IDtemp->o_link;
                }

                printf("\n\t* Order deleted successfully. *\n");
            }
            // if order is not deleted
            else
            {
                ordNode->qty = new_qty;
                ordNode->total = new_total;

                printf("\n\t* Order quantity updated. *\n");
            }
            return;

        // 3: return to the staff page
        case 3:
            return;

        default:
            break;
        }
    } while (choice != 3);
}

void customer_checkout(char *current_userID, int *order_count)
{
    system("@cls||clear");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("\n * Please take a screenshot of your receipt in case you require it *\n"
           "\n\t==============="
           "\n\t    Receipt"
           "\n\t==============="
           "\n"
           "\n Date: %02d/%02d/%04d, %02d:%02d:%02d"
           "\n"
           "\n Qty\tItem\t\t        Price"
           "\n------------------------------------------\n",
           tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
           tm.tm_hour, tm.tm_min, tm.tm_sec);

    int order_num = 0;
    double subtotal = 0;
    struct order *ordNode = o_head;
    while (ordNode != NULL)
    {
        // if the current logged in customer's ID is found in the order list, print the customer's order
        if (strncmp(current_userID, ordNode->cus_ID, sizeof(ordNode->cus_ID)) == 0)
        {
            printf(" x %d\t%-23s RM %6.2f\n",
                   ordNode->qty, ordNode->item_name, ordNode->total);
            subtotal += ordNode->total;
            order_num++;
        }

        // set the order linked list node to point to the next node
        ordNode = ordNode->o_link;
    }

    if (order_num == 0)
    {
        printf("\n\t*** You have no orders. ***\n");
        return;
    }

    const double service_tax = 0.06;
    double final_total = subtotal * (1 + service_tax);
    printf(
        "------------------------------------------"
        "\n Subtotal:\t\t\tRM %6.2f"
        "\n SST (6%%):\t\t\tRM %6.2f"
        "\n------------------------------------------"
        "\n TOTAL:\t\t\t\tRM %6.2f"
        "\n",
        subtotal, subtotal * service_tax, final_total);

    printf("\n Would you like to pay now?"
           "\n----------------------------------"
           "\n Enter \"y\" if yes or any other key to cancel: ");
    char confirm_choice = getchar();
    clear_input();
    if (confirm_choice == 'Y' || confirm_choice == 'y')
    {
        struct order *prev;
        ordNode = o_head;
        prev = NULL;
        while (ordNode != NULL)
        {
            // if item ID is found in the order list, remove it
            if (strncmp(current_userID, ordNode->cus_ID, sizeof(ordNode->cus_ID)) == 0)
            {
                (*order_count)--;
                // if the node to be deleted is the first node of the list
                if (ordNode == o_head)
                {
                    o_head = o_head->o_link;
                    free(ordNode);
                    ordNode = o_head;
                    continue;
                }
                // if the node to be deleted is after the first node of the list
                else
                {
                    prev->o_link = ordNode->o_link;
                    free(ordNode);
                    ordNode = prev->o_link;
                    continue;
                }
            }
            else
            {
                prev = ordNode;
                ordNode = ordNode->o_link;
            }
        }
        o_last = ordNode;

        // reordering the order IDs
        int i = 1;
        struct order *IDtemp = o_head;
        while (IDtemp != NULL)
        {
            sprintf(IDtemp->order_ID, "ORD%03d", i++);
            IDtemp = IDtemp->o_link;
        }

        printf("\n\t* Transaction successful. Have a nice day :) *\n");
    }
    else
        printf("\n\t* Transaction cancelled. *\n");
    return;
}

void customer_view_profile(struct customer *current_user)
{
    system("@cls||clear");

    printf(
        "\n\t===================="
        "\n\t    View Profile"
        "\n\t===================="
        "\n"
        "\n\t| Name: %s"
        "\n\t| Gender: %s"
        "\n\t| Date of Birth: %s"
        "\n\t| Mobile Number: %s"
        "\n\t| Email: %s"
        "\n\t| Username: %s"
        "\n\t| Password: %s"
        "\n",
        current_user->name, current_user->gender,
        current_user->birth_date, current_user->HP_num,
        current_user->email, current_user->username,
        current_user->password);
    return;
}

void customer_change_userpass(struct customer *current_user)
{
    system("@cls||clear");

    printf(
        "\n\t===================================="
        "\n\t    Change Username and Password"
        "\n\t===================================="
        "\n"
        "\n\t| Current username: %s"
        "\n\t| Current password: %s"
        "\n"
        "\n\t* Note: no spaces allowed *"
        "\n\t* Enter \"c\" to cancel. *\n",
        current_user->username,
        current_user->password);

    char *new_username;
    do
    {
        new_username = input_scanner("\n\t| (Max. 20 chars) Enter new username: ", 20, 's', 1);
        if (new_username == NULL)
        {
            free(new_username);
            return;
        }
        if (strchr(new_username, ' ') != NULL)
            continue;
        else
            break;
    } while (printf(INVALID_INPUT_MESSAGE));

    char *new_password;
    int flag;
    do
    {
        flag = 0;

        new_password = input_scanner("\n\t| (Max. 20 chars) Enter new password: ", 20, 's', 1);
        if (new_password == NULL)
        {
            free(new_username);
            free(new_password);
            return;
        }

        char *retry_password = input_scanner("\n\t| (Max. 20 chars) Re-enter new password: ", 20, 's', 1);
        if (retry_password == NULL)
        {
            free(new_username);
            free(new_password);
            free(retry_password);
            return;
        }

        if (strchr(new_password, ' ') != NULL)
        {
            printf("\n\t*** Invalid input. Please try again.\n");
            flag = 1;
            continue;
        }
        if (strncmp(new_password, retry_password, 20) != 0)
        {
            printf("\n\t*** Re-entered password does not match first password. Please try again. ***\n");
            flag = 1;
            continue;
        }
    } while (flag == 1);

    strcpy(current_user->username, new_username);
    strcpy(current_user->password, new_password);
    printf("\n\t* Username and password change successful. *\n");

    // 3 temporary mallocs used, 3 freed
    free(new_username);
    free(new_password);
    return;
}
