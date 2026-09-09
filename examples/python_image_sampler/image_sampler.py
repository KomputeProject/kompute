import os

import kp
import numpy as np


def compile_source(source):
    open("tmp_kp_shader.comp", "w").write(source)
    os.system("glslangValidator -V tmp_kp_shader.comp -o tmp_kp_shader.comp.spv")
    return open("tmp_kp_shader.comp.spv", "rb").read()


def main():
    mgr = kp.Manager()

    # A tiny 4x4 single-channel "image" (a coarse 2x2 checkerboard).
    in_width, in_height = 4, 4
    input_data = np.array(
        [
            [0.0, 0.0, 1.0, 1.0],
            [0.0, 0.0, 1.0, 1.0],
            [1.0, 1.0, 0.0, 0.0],
            [1.0, 1.0, 0.0, 0.0],
        ],
        dtype=np.float32,
    )

    input_image = mgr.image(input_data, in_width, in_height, 1)

    # Attaching a sampler switches this image's descriptor type from a
    # storage image (`image2D`, raw imageLoad/imageStore) to a combined
    # image sampler (`sampler2D`, hardware-filtered `texture()` reads).
    # create_sampler() defaults to linear filtering with clamp-to-edge
    # addressing.
    input_image.create_sampler()

    # A larger 16x16 output image that we fill by *sampling* (not just
    # copying) the small input image, letting the GPU's bilinear filter
    # smoothly interpolate between input texels.
    out_width, out_height = 16, 16
    output_data = np.zeros((out_height, out_width), dtype=np.float32)
    output_image = mgr.image(output_data, out_width, out_height, 1)

    params = [input_image, output_image]

    shader = compile_source(
        """
        #version 450

        layout (local_size_x = 1, local_size_y = 1) in;

        layout(binding = 0) uniform sampler2D inputImg;
        layout(binding = 1, r32f) writeonly uniform image2D outputImg;

        void main() {
            ivec2 outCoord = ivec2(gl_GlobalInvocationID.xy);
            ivec2 outSize = imageSize(outputImg);
            vec2 uv = (vec2(outCoord) + 0.5) / vec2(outSize);
            float value = texture(inputImg, uv).r;
            imageStore(outputImg, outCoord, vec4(value, 0.0, 0.0, 0.0));
        }
        """
    )

    algo = mgr.algorithm(params, shader, (out_width, out_height, 1))

    (
        mgr.sequence()
        .record(kp.OpSyncDevice(params))
        .record(kp.OpAlgoDispatch(algo))
        .record(kp.OpSyncLocal(params))
        .eval()
    )

    print(
        f"Upsampled output ({out_width}x{out_height}) sampled from a "
        f"{in_width}x{in_height} source image:\n"
    )
    print(np.round(output_image.data().reshape(out_height, out_width), 2))


if __name__ == "__main__":
    main()
