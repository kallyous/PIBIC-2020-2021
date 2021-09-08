#!/usr/bin/python3

import os

INSTANCES_DIR = 'instances'
SOLVERS_DIR = 'bin'

if __name__ == '__main__':

    solvers = os.listdir(SOLVERS_DIR)
    instances = [
        os.path.join(root, name)
        for root, dirs, files
        in os.walk(INSTANCES_DIR)
        for name in files
    ]  # https://stackoverflow.com/questions/5817209/browse-files-and-subfolders-in-python

    for solver in solvers:

        for instance in instances:

            result = os.popen(f'{SOLVERS_DIR}/{solver} < "{instance}"')

            path_words = instance.split('/')
            out_path = os.path.join('results', solver, *path_words[1:-1])
            os.makedirs(out_path, exist_ok=True)
            out_file = os.path.join(out_path, path_words[-1] + '.txt')

            with open(out_file, 'w') as file:
                file.write(result.read())
