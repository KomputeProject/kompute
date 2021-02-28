import os

import kp
import numpy as np
import logging
import pyshader as ps

DIRNAME = os.path.dirname(os.path.abspath(__file__))

kp_log = logging.getLogger("kp")

# TODO: Add example with file
#def test_opalgobase_file():
#    """
#    Test basic OpMult operation
#    """
#
#    tensor_in_a = kp.Tensor([2, 2, 2])
#    tensor_in_b = kp.Tensor([1, 2, 3])
#    tensor_out = kp.Tensor([0, 0, 0])
#
#    mgr = kp.Manager()
#    mgr.rebuild([tensor_in_a, tensor_in_b, tensor_out])
#
#    shader_path = os.path.join(DIRNAME, "../../shaders/glsl/opmult.comp.spv")
#
#    mgr.eval_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shader_path)
#
#    mgr.eval_tensor_sync_local_def([tensor_out])
#
#    assert tensor_out.data() == [2.0, 4.0, 6.0]



def test_shader_str():
    """
    Test basic OpAlgoBase operation
    """

    shader = """
#version 450
layout(set = 0, binding = 0) buffer tensorLhs {float valuesLhs[];};
layout(set = 0, binding = 1) buffer tensorRhs {float valuesRhs[];};
layout(set = 0, binding = 2) buffer tensorOutput { float valuesOutput[];};
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main()
{
    uint index = gl_GlobalInvocationID.x;
    valuesOutput[index] = valuesLhs[index] * valuesRhs[index];
}
    """

    spirv = kp.Shader.compile_source(shader)

    mgr = kp.Manager()

    tensor_in_a = mgr.tensor([2, 2, 2])
    tensor_in_b = mgr.tensor([1, 2, 3])
    tensor_out = mgr.tensor([0, 0, 0])

    params = [tensor_in_a, tensor_in_b, tensor_out]

    algo = mgr.algorithm(params, spirv)

    (mgr.sequence()
        .record(kp.OpTensorSyncDevice(params))
        .record(kp.OpAlgoDispatch(algo))
        .record(kp.OpTensorSyncLocal(params))
        .eval())

    assert tensor_out.data().tolist() == [2.0, 4.0, 6.0]

def test_sequence():
    """
    Test basic OpAlgoBase operation
    """

    shader = """
        #version 450
        layout(set = 0, binding = 0) buffer tensorLhs {float valuesLhs[];};
        layout(set = 0, binding = 1) buffer tensorRhs {float valuesRhs[];};
        layout(set = 0, binding = 2) buffer tensorOutput { float valuesOutput[];};
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

        void main()
        {
            uint index = gl_GlobalInvocationID.x;
            valuesOutput[index] = valuesLhs[index] * valuesRhs[index];
        }
    """

    spirv = kp.Shader.compile_source(shader)

    mgr = kp.Manager(0)

    tensor_in_a = mgr.tensor([2, 2, 2])
    tensor_in_b = mgr.tensor([1, 2, 3])
    tensor_out = mgr.tensor([0, 0, 0])

    params = [tensor_in_a, tensor_in_b, tensor_out]

    algo = mgr.algorithm(params, spirv)

    sq = mgr.sequence()

    sq.record(kp.OpTensorSyncDevice(params))
    sq.record(kp.OpAlgoDispatch(algo))
    sq.record(kp.OpTensorSyncLocal(params))

    sq.eval()

    assert sq.is_init() == True

    sq.destroy()

    assert sq.is_init() == False

    assert tensor_out.data().tolist() == [2.0, 4.0, 6.0]
    assert np.all(tensor_out.data() == [2.0, 4.0, 6.0])

    tensor_in_a.destroy()
    tensor_in_b.destroy()
    tensor_out.destroy()

    assert tensor_in_a.is_init() == False
    assert tensor_in_b.is_init() == False
    assert tensor_out.is_init() == False

def test_pushconsts():

    spirv = kp.Shader.compile_source("""
          #version 450
          layout(push_constant) uniform PushConstants {
            float x;
            float y;
            float z;
          } pcs;
          layout (local_size_x = 1) in;
          layout(set = 0, binding = 0) buffer a { float pa[]; };
          void main() {
              pa[0] += pcs.x;
              pa[1] += pcs.y;
              pa[2] += pcs.z;
          }
    """)

    mgr = kp.Manager()

    tensor = mgr.tensor([0, 0, 0])

    algo = mgr.algorithm([tensor], spirv, (1, 1, 1))

    (mgr.sequence()
        .record(kp.OpTensorSyncDevice([tensor]))
        .record(kp.OpAlgoDispatch(algo, [0.1, 0.2, 0.3]))
        .record(kp.OpAlgoDispatch(algo, [0.3, 0.2, 0.1]))
        .record(kp.OpTensorSyncLocal([tensor]))
        .eval())

    assert np.all(tensor.data() == np.array([0.4, 0.4, 0.4], dtype=np.float32))

def test_workgroup():
    mgr = kp.Manager(0)

    tensor_a = mgr.tensor(np.zeros([16,8]))
    tensor_b = mgr.tensor(np.zeros([16,8]))

    @ps.python2shader
    def compute_shader_wg(gl_idx=("input", "GlobalInvocationId", ps.ivec3),
                          gl_wg_id=("input", "WorkgroupId", ps.ivec3),
                          gl_wg_num=("input", "NumWorkgroups", ps.ivec3),
                          data1=("buffer", 0, ps.Array(ps.f32)),
                          data2=("buffer", 1, ps.Array(ps.f32))):
        i = gl_wg_id.x * gl_wg_num.y + gl_wg_id.y
        data1[i] = f32(gl_idx.x)
        data2[i] = f32(gl_idx.y)

    algo = mgr.algorithm([tensor_a, tensor_b], compute_shader_wg.to_spirv(), (16,8,1))

    (mgr.sequence()
        .record(kp.OpTensorSyncDevice([tensor_a, tensor_b]))
        .record(kp.OpAlgoDispatch(algo))
        .record(kp.OpTensorSyncLocal([tensor_a, tensor_b]))
        .eval())

    print(tensor_a.data())
    print(tensor_b.data())

    assert np.all(tensor_a.data() == np.stack([np.arange(16)]*8, axis=1).ravel())
    assert np.all(tensor_b.data() == np.stack([np.arange(8)]*16, axis=0).ravel())

