// Introduction to C Programming Assignment
// They Kai Zhe
// TP062802
// APU1F2203CS

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "ICP_Assignment_Functions.h"

int main()
{
    int staff_count, customer_count, item_count, order_count;
    if (initialize_structs(&staff_count, &customer_count, &item_count, &order_count) != 1)
    {
        pause();
        exit(EXIT_FAILURE);
    }

    struct staff *current_staff;
    struct customer *current_cus;
    int user_type;
    while (user_type != 5)
    {
        user_type = main_menu();
        switch (user_type)
        {
        case 1:
            current_staff = staff_login(staff_count);
            if (current_staff != NULL)
            {
                while (staff_menu(current_staff->name, &item_count, &customer_count, &order_count) != 1)
                {
                }
            }
            break;

        case 2:
            current_cus = customer_login(customer_count);
            if (current_cus != NULL)
            {
                while (customer_menu(current_cus, &order_count, item_count) != 1)
                {
                }
            }
            break;

        case 3:
            view_menu();
            pause();
            break;

        case 4:
            register_new_customer(&customer_count);
            pause();
            break;

        case 5:
            printf("\n\t* Thank you for using Dosta Coffee's Ordering System. *\n");
            break;

        default:
            break;
        }
    }
    pause();
    return 0;
}