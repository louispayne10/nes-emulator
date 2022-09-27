#!/bin/python3

# Used to compare the logs of our emulator against the correct logs of a test roms log
# file that is known to be correct

# This is all very hacky - I could do with brushing up my python
# Could probably make this 10x easier to read and understand if rewritten properly

from os.path import exists
import sys
import subprocess
import re
import collections
from itertools import zip_longest
from pprint import pprint

nestest_log_formatted_path = "./roms/nestest_formatted.log"

def find_memonic_index(split_line):
    for index, data in enumerate(split_line):
        if len(data) == 3 or (data[0] == '*' and len(data) == 4):
            return index
    sys.exit("couldn't find memomnic")

def get_reg(split_line, name):
    for data in split_line:
        if len(data) >= 2 and data[0] == name and data[1] == ':':
            return int(data[2:], base=16)
    sys.exit('couldnt find {}'.format(name))

def get_sp(split_line):
    for data in split_line:
        if len(data) >= 3 and data[0] == 'S' and data[1] == 'P' and data[2] == ':':
            return int(data[3:], base=16)
    sys.exit('couldnt find sp')

def get_cyc(split_line):
    full = split_line[len(split_line)-1]
    return int(full[4:])

def create_formatted_file():
    original_file = open('./roms/nes-test-roms/other/nestest.log', 'r')
    formatted_file = open(nestest_log_formatted_path, 'w', encoding="utf-8")
    for line in original_file:
        split_line = line.split()
        pc = int(split_line[0], base=16)
        memonic_index = find_memonic_index(split_line)
        memonic = split_line[memonic_index]
        opcode = int(split_line[1], base=16)
        a = get_reg(split_line, 'A')
        x = get_reg(split_line, 'X')
        y = get_reg(split_line, 'Y')
        p = get_reg(split_line, 'P')
        sp = get_sp(split_line)
        cyc = get_cyc(split_line)
        formatted_file.write(f'{pc:04X} {memonic:4} {opcode:02X}  a: {a:02X}  x: {x:02X}  y: {y:02X}  sp: {sp:02X}  p: {p:02X}  cyc:{cyc:6}\n')
    formatted_file.close()
    original_file.close()

if not exists(nestest_log_formatted_path):
    print('creating formatted nestest.log file')
    create_formatted_file()
else:
    print('nestest log file already exists')

def format_out_file(file):
    final_out_file = open("./roms/nes-emulator-final.log", "w")
    regex = re.compile(r"flags:.*\((.*)\)")
    i = 0
    for line in file:
        line = line.replace('cycles', 'cyc')
        line = regex.sub('p: \\1', line)
        final_out_file.write(line)

raw_out_file = open("./roms/nes-emulator-raw.log", "r+")
subprocess.run(["./build/src/nes-emulator", "./roms/nes-test-roms/other/nestest.nes"], stderr=raw_out_file)
raw_out_file.close()

raw_out_file = open("./roms/nes-emulator-raw.log", "r+")
format_out_file(raw_out_file)
raw_out_file.close()

def compare_files(our_file, our_raw_file, correct_file):
    last_instructions = collections.deque(maxlen=10)
    for our_line, our_raw_line, correct_line in zip_longest(our_file, our_raw_file, correct_file):
        if not our_line == correct_line:
            print('Lines differ')
            print('Ours:')
            print(our_line)
            print('Correct:')
            print(correct_line)
            print('Our original line:')
            print(our_raw_line)
            # Would be nice to add a readable diff here

            print('\nLast lines were:')
            for ins in reversed(last_instructions):
                print(ins, end='')
            sys.exit(1)
        else:
            last_instructions.append(our_raw_line)

our_file = open("./roms/nes-emulator-final.log", 'r')
correct_file = open("./roms/nestest_formatted.log")
raw_out_file = open("./roms/nes-emulator-raw.log", 'r')
compare_files(our_file, raw_out_file, correct_file)

