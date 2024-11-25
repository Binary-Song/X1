from rule_no_bom import NoBomRule
from rule_no_cr import NoCRRule
from rule_no_log import NoLogRule
from rule_no_binary_outside_lfs import NoBinaryOutsideLFS

def register_rules():
    rules = [ NoBomRule(), NoCRRule(), NoLogRule(), NoBinaryOutsideLFS() ]
    names = set()
    for rule in rules:
        if rule.name() in names:
            raise Exception("Duplicate rule name: " + rule.name())
        else:
            names.add(rule.name())
    return rules
