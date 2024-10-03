#!/bin/bash
set -e
cc -Wall -Wextra -g -std=c99 -fPIC -fpie blackjack.c -o blackjack -lncurses
