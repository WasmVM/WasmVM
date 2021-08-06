#!/bin/sh

astyle --style=kr --exclude=build*/ --exclude=third_party/ --indent=spaces=4 --indent-switches --suffix=none --recursive *.c *.h *.hpp