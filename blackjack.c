#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <ncurses.h>

#define DECK_SIZE 56

char *deck[DECK_SIZE] = {
    "1", "1", "1", "1",
    "2", "2", "2", "2",
    "3", "3", "3", "3",
    "4", "4", "4", "4",
    "5", "5", "5", "5",
    "6", "6", "6", "6",
    "7", "7", "7", "7",
    "8", "8", "8", "8",
    "9", "9", "9", "9",
    "10", "10", "10", "10",
    "J", "J", "J", "J",
    "Q", "Q", "Q", "Q",
    "K", "K", "K", "K",
    "A", "A", "A", "A"
};

int cards_drawn[DECK_SIZE];
int cards_drawn_cnt = 0;

char *draw_card() {
    int card = rand() % (DECK_SIZE + 1);
    
    for (int i = 0; i < cards_drawn_cnt; i++) {
        if (cards_drawn[i] == card)
            return draw_card();
    }

    cards_drawn[cards_drawn_cnt++] = card;
    return deck[card];
}

int card_value(char *card) {
    if (strcmp(card, "1") == 0)
        return 1;
    else if (strcmp(card, "2") == 0)
        return 2;
    else if (strcmp(card, "3") == 0)
        return 3;
    else if (strcmp(card, "4") == 0)
        return 4;
    else if (strcmp(card, "5") == 0)
        return 5;
    else if (strcmp(card, "6") == 0)
        return 6;
    else if (strcmp(card, "7") == 0)
        return 7;
    else if (strcmp(card, "8") == 0)
        return 8;
    else if (strcmp(card, "9") == 0)
        return 9;
    else if (strcmp(card, "A") == 0)
        return 11;
    return 10;
}

void show_hands(int bet, char *user_hand[9], int user_hand_cnt, int user_total, char *dealer_hand[9], int dealer_hand_cnt, int dealer_total) {
    printw("Bet: $%d\nDealer's hand:", bet);

    for (int i = 0; i < dealer_hand_cnt; i++)
        printw(" %s", dealer_hand[i]);

    printw("\nTotal: %d\n\nYour hand:", dealer_total);

    for (int i = 0; i < user_hand_cnt; i++)
        printw(" %s", user_hand[i]);
    printw("\nTotal: %d\n\n", user_total);
}

int main(void) {
    srand(time(NULL));
    initscr();
    cbreak();

    int cash = 1000;
    int bet;
    char *user_hand[9];
    int user_hand_cnt = 0;
    char *dealer_hand[9];
    int dealer_hand_cnt = 0;
    int user_total = 0;
    int dealer_total = 0;
    int key;
    char buf[32];
    char *endptr;
    bool won;
    bool can_double;
    bool can_hit;
    bool push;

    while (true) {
        memset(buf, 0, 32);

        cards_drawn_cnt = user_hand_cnt = dealer_hand_cnt = user_total = dealer_total = 0;
        won = push = false;
        can_hit = true;
        clear();
        curs_set(1);
        echo();

        while (true) {
            printw("Cash: $%d\nBet amount: $", cash);
            getstr(buf);

            bet = strtol(buf, &endptr, 10);
            if (buf == endptr || errno == ERANGE)
                printw("Error: Invalid bet amount: %s\n", strerror(errno));
            else if (bet < 1)
                printw("Error: You must bet at least 1\n");
            else if (bet > cash)
                printw("Error: You don't have enough cash to bet that much\n");
            else
                break;
        }

        clear();
        curs_set(0);
        noecho();

        if (bet * 2 <= cash)
            can_double = true;
        else
            can_double = false;

        user_hand[user_hand_cnt++] = draw_card();
        user_total += card_value(user_hand[user_hand_cnt - 1]);
        dealer_hand[dealer_hand_cnt++] = draw_card();
        dealer_total += card_value(dealer_hand[dealer_hand_cnt - 1]);
        show_hands(bet, user_hand, user_hand_cnt, user_total, dealer_hand, dealer_hand_cnt, dealer_total);

        if (can_hit) {
            while (user_total < 21 && can_hit) {

                if (dealer_total == 21) {
                    printw("Blackjack! Dealer wins $%d!\n", bet);
                    won = true;
                    break;
                } else if (user_total == 21) {
                    printw("Blackjack! You win $%d!\n", bet);
                    won = true;
                    break;
                }

                printw("Hit: 0\nStand: 1\n");
                if (can_double)
                    printw("Double: 2\n");

                key = getch();
                clear();

                if (key == '0') {
                    user_hand[user_hand_cnt++] = draw_card();
                    user_total += card_value(user_hand[user_hand_cnt - 1]);
                    can_double = false;
                } else if (key == '1')
                    can_hit = false;
                else if (key == '2' && can_double) {
                    bet *= 2;
                    user_hand[user_hand_cnt++] = draw_card();
                    user_total += card_value(user_hand[user_hand_cnt - 1]);
                    can_hit = false;
                } else {
                    printw("Error: Bad option '%s'\n", buf);
                    continue;
                }

                printf("\n");
                show_hands(bet, user_hand, user_hand_cnt, user_total, dealer_hand, dealer_hand_cnt, dealer_total);
            }

            while (dealer_total < 17) {
                dealer_hand[dealer_hand_cnt++] = draw_card();
                dealer_total += card_value(dealer_hand[dealer_hand_cnt - 1]);
            }

            clear();
            show_hands(bet, user_hand, user_hand_cnt, user_total, dealer_hand, dealer_hand_cnt, dealer_total);

            if (user_total > 21)
                printw("Bust! Dealer wins $%d\n", bet);
            else if (dealer_total > 21) {
                printw("Bust! You win $%d\n", bet);
                won = true;
            } else if (user_total > dealer_total) {
                printw("You win $%d!\n", bet);
                won = true;
            } else if (user_total < dealer_total)
                printw("Dealer wins $%d!\n", bet);
            else {
                printw("Push! Nobody wins!\n");
                push = true;
            }
        }

        if (won)
            cash += bet;
        else if (!push)
            cash -= bet;

        while (true) {
            printw("Play again?\nYes: 0\nNo: 1\n");
            key = getch();

            if (key == '0')
                break;
            else if (key == '1') {
                endwin();
                return 0;
            } else
                printw("Error: Bad option '%s'\n", buf);
        }
    }
}
