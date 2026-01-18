#!/usr/bin/env python
import os
import sys
import methods

from generate_bindings import generate_input_key_mapping

env = SConscript("godot-cpp/SConstruct")

opts = Variables([], ARGUMENTS)
opts.Add(BoolVariable("brotli", "Enable Brotli for decompression and WOFF2 fonts support", True))
opts.Add(BoolVariable("modules_enabled_by_default", "If no, disable all modules except ones explicitly enabled", True))
opts.Add(BoolVariable("svg_plugin", "Build with SVG plugin (LunaSVG)", True))

# Thirdparty libraries
opts.Add(BoolVariable("builtin_brotli", "Use the built-in Brotli library", True))
opts.Add(BoolVariable("builtin_libpng", "Use the built-in libpng library", True))
opts.Add(BoolVariable("builtin_zlib", "Use the built-in zlib library", True))

opts.Update(env)

# The module name
module_name = "gdex"

# The folder of the extension binaries
bin_folder = f"project/addons/rmlui/bin/{env["platform"]}.{env["target"]}.{env["arch"]}"

# Root directory of the extension code
root_folder = "src/"

folders = [x[0] for x in os.walk(root_folder)]
sources = None
for folder in folders:
    fg = Glob(os.path.join(folder, '*.cpp'))
    if sources == None:
        sources = fg
    else:
        sources = sources + fg

env.Append(CPPPATH=folders)

env.__class__.add_source_files = methods.add_source_files
env.__class__.add_library = methods.add_library
env.__class__.add_shared_library = methods.add_shared_library
env.__class__.add_program = methods.add_program
env.__class__.module_add_dependencies = methods.module_add_dependencies
env.__class__.module_check_dependencies = methods.module_check_dependencies

env.module_dependencies = {}
env.disabled_modules = set()

Export("env")

SConscript("modules/SCsub")

library = env.add_shared_library("{}/lib{}".format(bin_folder, module_name), sources)
Default(library)





