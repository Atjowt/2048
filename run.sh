#!/usr/bin/env bash
mkdir -p build && cmake -B build && cmake --build build --config Release && build/2048
