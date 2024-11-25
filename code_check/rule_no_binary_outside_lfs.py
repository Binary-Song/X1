from code_check import Rule, Check, Line, RawDiff

class NoBinaryOutsideLFS(Rule):
    def name(self) -> str:
        return 'no_binary_outside_lfs'
    
    def check(self, chk: Check) -> bool:
        chk.add_error("add this file to lfs if you want to commit it")
        return False
    
    def applies_to(self, file_type: str) -> bool:
        if file_type == 'binary':
            return True
        return False