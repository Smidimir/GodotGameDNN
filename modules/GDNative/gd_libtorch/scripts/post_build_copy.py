import sys, getopt, shutil, functools 

def as_short_optdef(opt):
    return "%s:" % format(opt)

def as_long_optdef(opt):
    return "%s=" % format(opt)

def as_short_opt(opt):
    return "-%s" % format(opt)

def as_long_opt(opt):
    return "--%s" % format(opt)

def print_help():
    print("test.py -i <inputfile> -o <outputfile>")

def parse_opts(argv):
    class OptName:
        def __init__(self, opt_name, optdef_formater, opt_formater):
            self.__opt_name = opt_name
            self.__optdef_formater = optdef_formater
            self.__opt_formater = opt_formater

        def name(self):
            return self.__opt_name

        def optdef(self):
            return self.__optdef_formater(self.name())

        def opt(self):
            return self.__opt_formater(self.name())

    class Opt:
        def __init__(self, opt_short, opt_long):
            self.short = opt_short
            self.long = opt_long

            def action(*_):
                pass

            self.action = action

        def opt_pair(self):
            return (self.short, self.long)

        def optdefs(self):
            return tuple(map(lambda opt: opt.optdef(), self.opt_pair()))

        def opts(self):
            return tuple(map(lambda opt: opt.opt(), self.opt_pair()))

    class OptBuilder:
        def __init__(self):
            self.short = None
            self.long = None
            self.args = False

        def set_short(self, opt_name):
            self.short = opt_name
            return self

        def set_long(self, opt_name):
            self.long = opt_name
            return self

        def enable_args(self, enabled):
            self.args = enabled
            return self

        def build(self):
            def optdef_formater_no_args(opt_name):
                return opt_name

            def optdef_formater_args_short(opt_name):
                return "%s:" % (opt_name)

            def optdef_formater_args_long(opt_name):
                return "%s=" % (opt_name)

            def opt_formater_short(opt_name):
                return "-%s" % (opt_name)

            def opt_formater_long(opt_name):
                return "--%s:" % (opt_name)

            opt_short = None
            if self.short is not None:
                opt_short = OptName(
                    self.short,
                    optdef_formater_args_short if self.args else optdef_formater_no_args,
                    opt_formater_short
                )

            opt_long = None
            if self.long is not None:
                opt_long = OptName(
                    self.long,
                    optdef_formater_args_long if self.args else optdef_formater_no_args,
                    opt_formater_long
                )

            return Opt(
                opt_short,
                opt_long
            )

    class Context:
        def __init__(self):
            self.path = ""
            self.src_relative = ""
            self.dest_relative = ""

    def action_on_help(*_):
        print_help()
    opt_help = OptBuilder() \
        .set_short("h") \
        .set_long("help") \
        .build()
    opt_help.action = action_on_help

    def action_on_path(context, arg):
        context.path = arg
    opt_path = OptBuilder() \
        .set_short("p") \
        .set_long("path") \
        .enable_args(True) \
        .build()
    opt_path.action = action_on_path

    def action_on_src(context, arg):
        context.src_relative = arg
    opt_src = OptBuilder() \
        .set_short("s") \
        .set_long("src") \
        .enable_args(True) \
        .build()
    opt_src.action = action_on_src

    def action_on_dest(context, arg):
        context.dest_relative = arg
    opt_dest = OptBuilder() \
        .set_short("d") \
        .set_long("dest") \
        .enable_args(True) \
        .build()
    opt_dest.action = action_on_dest

    opts = (opt_help, opt_path, opt_src, opt_dest)

    shortopts = str(
        functools.reduce(
            lambda a, b: a + b,
            map(
                lambda opt: opt.short.optdef(),
                opts
            )
        )
    )

    longopts = tuple(
        map(
            lambda opt: opt.long.optdef(),
            opts
        )
    )

    context = Context()

    try:
        opt_names, _ = getopt.getopt(argv, shortopts, longopts)
    except getopt.GetoptError:
        print_help()
        sys.exit(2)
    for opt_name, arg in opt_names:
        for opt in opts:
            if opt_name in opt.opts():
                opt.action(context, arg)

    src_full = context.path + context.src_relative
    dest_full = context.path + context.dest_relative

    return src_full, dest_full

def main(argv):
    src, dest = parse_opts(argv)
    print("src:", src)
    print("dest:", dest)
    shutil.copyfile(src, dest)

if __name__ == "__main__":
    main(sys.argv[1:])
