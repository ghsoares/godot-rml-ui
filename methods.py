
def print_warning(*values: object) -> None:
	print(*values)

def add_source_files(self, sources, files, allow_gen=False):
	if isinstance(files, str):
		skip_gen_cpp = "*" in files
		files = self.Glob(files)
		if skip_gen_cpp and not allow_gen:
			files = [f for f in files if not str(f).endswith(".gen.cpp")]
	
	for file in files:
		obj = self.Object(file)
		if obj in sources:
			print_warning('Object "{}" already included in environment sources.'.format(obj))
			continue
		sources.append(obj)

def add_shared_library(env, name, sources, **args):
    library = env.SharedLibrary(name, sources, **args)
    env.NoCache(library)
    return library

def add_library(env, name, sources, **args):
    library = env.Library(name, sources, **args)
    env.NoCache(library)
    return library

def add_program(env, name, sources, **args):
    program = env.Program(name, sources, **args)
    env.NoCache(program)
    return program

def module_add_dependencies(self, module, dependencies, optional=False):
    """
    Adds dependencies for a given module.
    Meant to be used in module `can_build` methods.
    """
    if module not in self.module_dependencies:
        self.module_dependencies[module] = [[], []]
    if optional:
        self.module_dependencies[module][1].extend(dependencies)
    else:
        self.module_dependencies[module][0].extend(dependencies)


def module_check_dependencies(self, module):
    """
    Checks if module dependencies are enabled for a given module,
    and prints a warning if they aren't.
    Meant to be used in module `can_build` methods.
    Returns a boolean (True if dependencies are satisfied).
    """
    missing_deps = set()
    required_deps = self.module_dependencies[module][0] if module in self.module_dependencies else []
    for dep in required_deps:
        opt = "module_{}_enabled".format(dep)
        if opt not in self or not self[opt] or not module_check_dependencies(self, dep):
            missing_deps.add(dep)

    if missing_deps:
        if module not in self.disabled_modules:
            print_warning(
                "Disabling '{}' module as the following dependencies are not satisfied: {}".format(
                    module, ", ".join(missing_deps)
                )
            )
            self.disabled_modules.add(module)
        return False
    else:
        return True

