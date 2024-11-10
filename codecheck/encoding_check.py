import os
import sys
import subprocess
import logging
from pathlib import Path

repo_path = (Path(__file__) / ".." / "..").resolve()
log_path = (repo_path / "log").resolve()
os.makedirs(log_path, exist_ok=True)
bad_files = set()

log_format = "%(levelname)s - %(message)s"
logging.basicConfig(level=logging.ERROR, format=log_format, handlers=[
    logging.FileHandler(log_path / "encoding_check.log"),
    logging.StreamHandler(sys.stdout)
])

bom_bytes = b'\xef\xbb\xbf'

def report_bad_file(file_path, message):
    if file_path not in bad_files:
        bad_files.add(file_path)
        logging.error(message)

def check_encoding(file_path):
    try:
        with open(file_path, 'rb') as f:
            raw_data = f.read()
            if raw_data.contains(bom_bytes):
                report_bad_file(file_path, f"{file_path} contains BOM.")
                return False
            raw_data.decode('utf-8')

    except UnicodeDecodeError:
        report_bad_file(file_path, f"{file_path} is not UTF-8 encoded.")
        return False
    return True

def check_repo_encoding(repo_path):
    try:
        result = subprocess.run(['git', 'ls-files', '--eol'], cwd=repo_path, stdout=subprocess.PIPE, text=True, check=True)
        files = result.stdout.splitlines()
        all_files_passed = True
        for file_info in files:
            parts = file_info.split()
            if len(parts) < 4:
                report_bad_file(file_info, f"failed to parse git ls-files output: {file_info}")
                all_files_passed = False
                continue
            index_eol, workdir_eol, attr, file_rel_path = parts[0], parts[1], parts[2], parts[3]
            is_text_file = '-text' not in attr
            if is_text_file: # only check text files
                file_path = os.path.join(repo_path, file_rel_path)
                if not check_encoding(file_path):
                    all_files_passed = False
        return all_files_passed
    except subprocess.CalledProcessError as e:
        logging.error(f"Error: {e}")
        return False

def check_diff(repo_path : Path, incremental=False):
    empty_tree_hash = '4b825dc642cb6eb9a060e54bf8d69288fbee4904'
    diff_target = 'HEAD' if incremental == True else empty_tree_hash
    result = subprocess.run(['git', 'diff', '--staged', diff_target],
                            cwd=repo_path,
                            stdout=subprocess.PIPE,
                            text=False,
                            #encoding='utf-8',
                            check=True)
    output = result.stdout
    diff_path = repo_path / "log" / "staged.diff"
    with open(diff_path, 'wb') as f:
        f.write(output)

    ok = True
    current_file = "<unknown>"
    for i, line in enumerate(output.split(b'\n'), start=1):
        
        # Set current file
        if line.startswith(b'diff --git'):
            # current_file example: b/src/test/extension.test.ts
            current_file = line.split()[-1].decode('utf-8')
            if current_file.startswith('a/') or current_file.startswith('b/'):
                # current_file example: src/test/extension.test.ts
                current_file = current_file[2:]
        
        if b'\r' in line:
            report_bad_file(current_file, f"CR in file {current_file}, see {diff_path}:{i}")
            ok = False
        if bom_bytes in line:
            report_bad_file(current_file, f"BOM in file {current_file}, see {diff_path}:{i}")
            ok = False
    return ok

if __name__ == "__main__":
    passed = True

    def failed():
        sys.exit(1)

    if not check_diff(repo_path, incremental=True):
        failed()
    
    logging.info("encoding check passed")
    sys.exit(0)
