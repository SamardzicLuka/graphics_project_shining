#!/bin/sh

clang-tidy src/main.cpp --checks="modernize-*" \
                        --fix -fix-errors -- --std=c++17 >log.txt

