import os


def compile_source(source):
    os.system("glslangValidator --stdin -S comp -V -o tmp_kp_shader.comp.spv << END\n" + source + "\nEND")
    return open("tmp_kp_shader.comp.spv", "rb").read()

