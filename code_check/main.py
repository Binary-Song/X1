import os
import sys
import subprocess
import logging
from pathlib import Path
import argparse
from register_rules import register_rules
from code_check import Rule, Check, Line, RawDiff

def print_err(msg):
    print(msg, file=sys.stderr)

def get_file_attrs(path):
    cmd = ['git', 'check-attr', '-a', '-z', '--', path]
    result = subprocess.run(cmd,
                        cwd=repo_path,
                        stdout=subprocess.PIPE,
                        text=False,
                        check=True)
    # example output: "path/to/my/file\0key\0value\0"
    # where \0 is the NUL char.
    output = result.stdout
    attr = { }
    for index, word in enumerate(output.split(b'\x00')):
        if index % 3 == 0:
            continue
        elif index % 3 == 1:
            key = word.decode('utf-8')
        elif index % 3 == 2:
            value = word.decode('utf-8')
            attr[key] = value
    return attr

def get_file_type(path, diff: RawDiff):
    attr = get_file_attrs(path)
    
    # if it contains any lfs, then it is a lfs-file
    for lfs_attr in ['diff', 'filter', 'merge']:
        if lfs_attr in attr and attr[lfs_attr] == 'lfs':
            return 'lfs-file'
        
    # if the diff contains '@@ -1,69 +0,0 @@' line, then it is a text file
    for line in diff.lines:
        if line.text.startswith(b'@@'):
            return 'text'
    
    # otherwise, it is a binary file
    return 'binary'
    
def check_diff(repo_path : Path, rules: list[Rule], incremental=False):
    empty_tree_hash = '4b825dc642cb6eb9a060e54bf8d69288fbee4904'
    cmd = ['git', 'diff', '--staged', 'HEAD'] if incremental else ['git', 'diff', empty_tree_hash, 'HEAD']
    result = subprocess.run(cmd,
                            cwd=repo_path,
                            stdout=subprocess.PIPE,
                            text=False,
                            check=True)
    raw_git_diff = result.stdout
    diff_path = repo_path / "log" / "staged.diff"
    with open(diff_path, 'wb') as f:
        f.write(raw_git_diff)

    ok = True
    current_file = "<unknown>"
    file_to_diff: dict[str, RawDiff] = {}

    for i, line in enumerate(raw_git_diff.split(b'\n'), start=1):
        # line example: diff --git a/1.png b/1.png
        if line.startswith(b'diff --git'):
            # current_file example: a/src/test/extension.test.ts
            current_file = line.split()[-1].decode('utf-8')
            if current_file.startswith('a/') or current_file.startswith('b/'):
                # remove the 'a/' prefix
                current_file = current_file[2:]
        else:
            if current_file not in file_to_diff:
                file_to_diff[current_file] = RawDiff([])
            
            file_to_diff[current_file].lines.append(Line(i, line))
    
    # apply the rules
    ok = True
    for file, diff in file_to_diff.items():
        file_type = get_file_type(file, diff)
        file_attrs = get_file_attrs(file)
        for rule in rules:
            if not rule.applies_to(file_type):
                continue
            chk_attr_name = f'chk_{rule.name()}'
            chk_attr_on =  chk_attr_name in file_attrs and file_attrs[chk_attr_name] == 'set'
            if not chk_attr_on:
                continue

            chk = Check(file, diff)
            if not rule.check(chk):
                ok = False
                for error in chk.errors:
                    if error.line_no != -1:
                        print_err(f"Error: Rule '{rule.name()}' failed for {file}. See diff at {diff_path}:{error.line_no}. Message: {error.message}")
                    else:
                        print_err(f"Error: Rule '{rule.name()}' failed for {file}. Message: {error.message}")
        # if line.startswith(b'+') and b'\r' in line:
        #     if current_file not in bad_file_set:
        #         bad_file_set.add(current_file)
        #         logging.error(f"CR in file {current_file}, see {diff_path}:{i}.")
        #     ok = False
        # if line.startswith(b'+') and b'\xef\xbb\xbf' in line:
        #     if current_file not in bad_file_set:
        #         bad_file_set.add(current_file)
        #         logging.error(f"BOM in file {current_file}, see {diff_path}:{i}.")
        #     ok = False
        # if line.startswith(b'+') and b'X1_LOG' in line and 'Assert' not in current_file and current_file.endswith("cpp"):
        #     if current_file not in bad_file_set:
        #         bad_file_set.add(current_file)
        #         logging.error(f"X1_LOG in {current_file}, see {diff_path}:{i}.")
        #     ok = False
    return ok

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Check for encoding issues in git diffs.")
    parser.add_argument('--full', action='store_true', help='Run a full check instead of an incremental one.')
    args = parser.parse_args()

    repo_path = (Path(__file__) / ".." / "..").resolve()
    log_path = (repo_path / "log").resolve()
    os.makedirs("log", exist_ok=True)
    passed = True

    checks = register_rules()
    if not check_diff(repo_path, checks, incremental=not args.full):
        passed = False
    
    if passed:
        print("Encoding checks passed.")
        sys.exit(0)
    else:
        sys.exit(1)
