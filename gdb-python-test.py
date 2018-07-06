import gdb.printing


class StudentPrinter(object):
    """
    This is a class that is supposed to change
    the way student class is printed in GDB.
    """

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return ("\n{\nname: _%s_\n}\n" % self.val['name'])

    def display_hint(self):
        return "student"


pp = gdb.printing.RegexpCollectionPrettyPrinter("student")
pp.add_printer("student", "^student$", StudentPrinter)
gdb.printing.register_pretty_printer(gdb.current_objfile(), pp)
