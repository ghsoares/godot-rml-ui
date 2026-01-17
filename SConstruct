#!/usr/bin/env python
import os
import sys

from generate_bindings import generate_input_key_mapping

env = SConscript("dependencies/godot-cpp/SConstruct")

deps_build_folder = os.path.abspath(f"build/{env["platform"]}.{env["target"]}.{env["arch"]}")

opts = Variables([], ARGUMENTS)
opts.Add(BoolVariable("svg_plugin", "Build with SVG plugin (LunaSVG)", True))
opts.Update(env)

def build_dependencies():
    import subprocess

    build_type = "Release" if env["target"] == "template_release" else "Debug"

    commands = []
    commands.append([
        f"cmake -B {deps_build_folder} -S .",
        f"-DBUILD_SHARED_LIBS=ON",
        f"-DTARGET_ARCH={env["arch"]}",
        f"-DTARGET_PLATFORM={env["platform"]}",
        f"-DCMAKE_TOOLCHAIN_FILE=./cmake/{env["platform"]}.cmake",
        f"-DCMAKE_BUILD_TYPE={build_type}",
        f"-DRMLUI_SVG_PLUGIN={env["svg_plugin"]}",
    ])

    commands.append([
        f"cmake --build {deps_build_folder}"
    ])

    for cmd in commands:
        cmd = " ".join(cmd)
        process = subprocess.run(cmd, shell=True)

generate_input_key_mapping()
build_dependencies()

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

folders.append('dependencies/RmlUi/Include')
env.Append(CPPPATH=folders)

# Include third-party libraries
thirdparty_paths = [
    f'{deps_build_folder}/bin'
]

env.Append(LIBPATH=thirdparty_paths)
env.Append(LIBS=[f"librmlui"])

library = env.SharedLibrary(
    "{}/lib{}{}".format(bin_folder, module_name, env["SHLIBSUFFIX"]),
    source=sources,
)

thirdparty_libs = None
for path in thirdparty_paths:
    fg = Glob(os.path.join(path, f"*{env["SHLIBSUFFIX"]}*"))
    thirdparty_libs = fg if thirdparty_libs == None else thirdparty_libs + fg

print(thirdparty_libs)

installed_libs = env.Install(bin_folder, thirdparty_libs)
env.Depends(library, installed_libs)

Default(library)
