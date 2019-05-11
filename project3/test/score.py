#!/usr/bin/env python3

import os
import sys
from collections import namedtuple


DELIMITER = '|'
FlagPoints = namedtuple('FlagPoints', ['flag', 'points'])


def get(f):
    return map(
        lambda x: list(map(lambda x: x.strip(), x.strip().split(DELIMITER))),
        filter(lambda x: x.strip(), f.readlines()),
    )


def check_input_filename(filename):
    filename = os.path.basename(filename)
    if not filename:
        raise ValueError('You did not provide filename')
    if len(filename.split('.')) != 1:
        raise ValueError('Your file include extension')


def check_student_ids(student_ids):
    for student in student_ids:
        if not student:
            raise ValueError('Empty student id')
        if len(student) < 3:
            raise ValueError('Student id too short (< 3)')


def main():
    if len(sys.argv) != 3:
        print('usage: score.py [input] [answer]')
        exit(1)

    input_fn = sys.argv[1]
    answer_fn = sys.argv[2]

    check_input_filename(input_fn)

    with open(input_fn) as f:
        student_ids = next(f).strip().split(',')
        input_lines = get(f)

    check_student_ids(student_ids)

    with open(answer_fn) as f:
        lines = get(f)
        answer = {k: FlagPoints(v, int(s)) for k, v, s in lines}

    score = 0
    for k, v in input_lines:
        if answer[k].flag == v:
            score += answer[k].points

    for student in student_ids:
        print(f'{student}: {score}')


if __name__ == '__main__':
    main()
