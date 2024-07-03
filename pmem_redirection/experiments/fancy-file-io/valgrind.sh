#!/bin/bash

make myio && valgrind -s --leak-check=full --show-leak-kinds=all ./myio
