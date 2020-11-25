#!/bin/bash

uuidgen | sed -r 's/(..)(..)(..)(.*)/\1:\2:\3/g'
