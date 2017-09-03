#!/usr/bin/python2

import json
import subprocess
import os
import sys

compdb = json.loads(sys.stdin.read())
outputdb = []

HEADER_EXTENSIONS = ['.h', '.hpp', '.hxx', '.hh']

def db_has_file(source, cdb):
    for source_comp in cdb:
        if source_comp['file'] == source:
            return True
    return False

def is_header(source):
    return os.path.splitext(source)[1] in HEADER_EXTENSIONS

def get_path(source, cdb):
    for source_comp in cdb:
        if source_comp['file'] == os.path.basename(source):
            return source_comp['directory']
    return None

def get_command(source, cdb):
    for source_comp in cdb:
        if source_comp['file'] == os.path.basename(source):
            return source_comp['command']
    return None

def db_entry(source, path, command):
    return {'file':source, 'directory':path, 'command':command}

def get_files(cdb):
    return [x['file'] for x in cdb]

def get_include_dirs(command):
    includes = []
    cparts = command.split()
    add_next = False
    for cpart in cparts:
        if add_next:
            includes.append(cpart)
            add_next = False
            continue

        if cpart.startswith('-I') and len(cpart) > 2:
            includes.append(cpart[2:])
        elif cpart == '-I':
            add_next = True

    return includes

def get_includes_below(source, basedir, cdb):
    basedir = os.path.abspath(basedir)
    command = get_command(source, cdb)

    if not command:
        return []

    includes_string = ""
    try:
        includes_string = subprocess.check_output(command.split() + ['-H'], stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        return []
    
    test_paths = {}

    for include_string in includes_string.split('\n'):
        split_idx = include_string.find(' ')
        dots, path = include_string[:split_idx],include_string[split_idx+1:]
        numdots = len(dots)
        if test_paths.has_key(numdots):
            test_paths[numdots].append(path)
        else:
            test_paths[numdots] = [path]

    includes = []

    for key in sorted(test_paths.keys()):
        has_path = False
        for path in test_paths[key]:
            if os.path.abspath(path).startswith(basedir):
                includes.append(path)
                has_path = True

        if not has_path:
            break

    return includes

for source in get_files(compdb):
    for include in get_includes_below(source, '.', compdb):
        if is_header(include):
            if not db_has_file(include, outputdb):
                command = get_command(source, compdb)
                source_base = os.path.splitext(source)[0]
                subbed_command = command.replace(source, include).replace(source_base, include)
                outputdb.append(db_entry(include, get_path(source, compdb), subbed_command))

print json.dumps(compdb + outputdb, sort_keys=True, indent=4)
