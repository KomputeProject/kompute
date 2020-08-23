"""
    Script to handle conversion of compute shaders to spirv and to headers
"""
import os
import logging
import click
import sh

logger = logging.getLogger(__name__)
logger.addHandler(logging.StreamHandler())

@click.command()
@click.option(
    "--shader-path",
    "-p",
    envvar="KOMPUTE_SHADER_PATH",
    required=True,
    help="The path for the directory to build and convert shaders",
)
@click.option(
    "--shader-binary",
    "-s",
    envvar="KOMPUTE_SHADER_BINARY",
    required=True,
    help="The path for the directory to build and convert shaders",
)
@click.option(
    "--header-path",
    "-c",
    envvar="KOMPUTE_HEADER_PATH",
    default="",
    required=False,
    help="The (optional) output file for the cpp header files",
)
@click.option(
    "--verbose",
    "-v",
    envvar="KOMPUTE_HEADER_PATH",
    default=False,
    is_flag=True,
    help="Enable versbosity if flag is provided",
)
def run_cli(
    shader_path: str = None,
    shader_binary: str = None,
    header_path: bool = None,
    verbose: bool = None,
):
    """
    CLI function for shader generation
    """

    if verbose:
        logger.setLevel(logging.DEBUG)
    else:
        logger.setLevel(logging.WARNING)

    logger.debug(f"Starting script with variables: {locals()}")

    shader_files = []
    for root, directory, files in os.walk(shader_path):
        for file in files:
            if file.endswith(".comp"):
                shader_files.append(os.path.join(root, file))

    shader_cmd = sh.Command(shader_binary)

    logger.debug(f"Output spirv path: {shader_path}")
    logger.debug(f"Converting files to spirv: {shader_files}")

    spirv_files = []
    for file in shader_files:
        logger.debug(f"Converting to spirv: {file}")
        spirv_file = f"{file}.spv"
        shader_cmd("-V", file, "-o", spirv_file)
        spirv_files.append(spirv_file)

    # Create cpp files if header_path provided
    if header_path:
        logger.debug(f"Header path provided. Converting bin files to hpp.")
        logger.debug(f"Output header path: {shader_path}")

        for file in spirv_files:
            header_data = str(sh.xxd("-i", file))
            # Ensuring the variable is a static unsigned const
            header_data = header_data.replace("unsigned", "static unsigned const")
            file_name = file.split("/")[-1]
            header_file = file_name.replace(".comp.spv", ".hpp")
            header_file_define = "SHADEROP_" + header_file.replace(".", "_").upper()
            logger.debug(f"Converting to hpp: {file_name}")
            with open(os.path.join(header_path, header_file), "w+") as fstream:
                fstream.write(f"#ifndef {header_file_define}\n")
                fstream.write(f"#define {header_file_define}\n\n")
                fstream.write("namespace kp {\n")
                fstream.write("namespace shader_data {\n")
                fstream.write(f"{header_data}")
                fstream.write("}\n")
                fstream.write("}\n")
                fstream.write(f"#endif // define {header_file_define}\n")


if __name__ == "__main__":
    run_cli()
