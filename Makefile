# Author: Marko Kubrachenko
# Date: 02.22
# Project: http server IPK

SHELL = /usr/bin/env bash
NAME = hinfosvc
LOGIN = xkubra00
all:
	gcc $(NAME).c -o $(NAME) -lm

zip:
	zip $(LOGIN).zip $(NAME).c Makefile README.md

clean:
	rm -f $(NAME) $(LOGIN).zip
