
class Line:
    def __init__(self, line_no: int, line: bytes):
        self.line_no = line_no
        self.text = line

class RawDiff:
    def __init__(self, lines: list[Line]):
        self.lines = lines

class Error:
    def __init__(self, message: str, line_no: int = -1):
        self.line_no = line_no
        self.message = message

class Check:
    def __init__(self, path: str, diff: RawDiff):
        self.path = path
        self.diff = diff
        self.errors : list[Error] = []
    
    def add_error(self, msg: str, line_no: int = -1):
        self.errors.append(Error(msg, line_no))

class Rule:
    
    def __init__(self):
        pass
    
    def name(self) -> str:
        '''
        Name of the rule.
        '''
        raise NotImplementedError()

    def check(self, chk: Check) -> bool:
        '''
        Returns whether the check has passed.
        '''
        raise NotImplementedError()

    def applies_to(self, file_type: str) -> bool:
        """
        Determines whether this rule applies to the specified file type.
        
        `file_type`: The type of file to check against the rule. Possible values:
        
        - `"lfs-file"`: A file that is managed by git-lfs

        - `"text"`: A file that is not a `"lfs-file"`, and can be decoded by utf-8.

        - `"binary"`: A file that is not a `"lfs-file"`, and can not be decoded by utf-8.

        """
        if file_type == 'text':
            return True
        return False
    

    def no_adding_word(self, chk: Check, word: bytes, message: str):
        ok = True
        for line in chk.diff.lines:
            line_text = line.text
            if line_text.startswith(b'+') and word in line_text:
                chk.add_error(message, line.line_no)
                ok = False
        return ok
    