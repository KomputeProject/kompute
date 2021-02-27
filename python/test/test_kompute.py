import os

import kp
import numpy as np
import logging
import pyshader as ps

DIRNAME = os.path.dirname(os.path.abspath(__file__))

def test_opalgobase_file():
    """
    Test basic OpMult operation
    """

    tensor_in_a = kp.Tensor([2, 2, 2])
    tensor_in_b = kp.Tensor([1, 2, 3])
    tensor_out = kp.Tensor([0, 0, 0])

    mgr = kp.Manager()
    mgr.rebuild([tensor_in_a, tensor_in_b, tensor_out])

    shader_path = os.path.join(DIRNAME, "../../shaders/glsl/opmult.comp.spv")

    mgr.eval_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shader_path)

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]



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

    tensor_in_a = kp.Tensor([2, 2, 2])
    tensor_in_b = kp.Tensor([1, 2, 3])
    tensor_out = kp.Tensor([0, 0, 0])

    mgr = kp.Manager()
    mgr.rebuild([tensor_in_a, tensor_in_b, tensor_out])

    spirv = kp.Shader.compile_source(shader)

    mgr.eval_algo_data_def([tensor_in_a, tensor_in_b, tensor_out], spirv)

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]

def test_sequence():
    """
    Test basic OpAlgoBase operation
    """
    mgr = kp.Manager(0, [2])

    tensor_in_a = kp.Tensor([2, 2, 2])
    tensor_in_b = kp.Tensor([1, 2, 3])
    tensor_out = kp.Tensor([0, 0, 0])

    mgr.rebuild([tensor_in_a, tensor_in_b, tensor_out])

    shader_path = os.path.abspath(os.path.join(DIRNAME, "../../shaders/glsl/opmult.comp.spv"))
    mgr.eval_async_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shader_path)

    mgr.eval_await_def()

    seq = mgr.sequence("op")
    seq.begin()
    seq.record_tensor_sync_local([tensor_in_a])
    seq.record_tensor_sync_local([tensor_in_b])
    seq.record_tensor_sync_local([tensor_out])
    seq.end()
    seq.eval()

    mgr.destroy("op")

    assert seq.is_init() == False

    assert tensor_out.data() == [2.0, 4.0, 6.0]
    assert np.all(tensor_out.numpy() == [2.0, 4.0, 6.0])

    mgr.destroy(tensor_in_a)
    mgr.destroy([tensor_in_b, tensor_out])

    assert tensor_in_a.is_init() == False
    assert tensor_in_b.is_init() == False
    assert tensor_out.is_init() == False

def test_workgroup():
    mgr = kp.Manager(0)

    tensor_a = kp.Tensor(np.zeros([16,8]))
    tensor_b = kp.Tensor(np.zeros([16,8]))

    mgr.rebuild([tensor_a, tensor_b])

    @ps.python2shader
    def compute_shader_wg(gl_idx=("input", "GlobalInvocationId", ps.ivec3),
                          gl_wg_id=("input", "WorkgroupId", ps.ivec3),
                          gl_wg_num=("input", "NumWorkgroups", ps.ivec3),
                          data1=("buffer", 0, ps.Array(ps.f32)),
                          data2=("buffer", 1, ps.Array(ps.f32))):
        i = gl_wg_id.x * gl_wg_num.y + gl_wg_id.y
        data1[i] = f32(gl_idx.x)
        data2[i] = f32(gl_idx.y)

    seq = mgr.sequence("new")
    seq.begin()
    seq.record_algo_data([tensor_a, tensor_b], compute_shader_wg.to_spirv(), workgroup=(16,8,1))
    seq.end()
    seq.eval()

    mgr.destroy(seq)

    assert seq.is_init() == False

    mgr.eval_tensor_sync_local_def([tensor_a, tensor_b])

    print(tensor_a.numpy())
    print(tensor_b.numpy())

    assert np.all(tensor_a.numpy() == np.stack([np.arange(16)]*8, axis=1).ravel())
    assert np.all(tensor_b.numpy() == np.stack([np.arange(8)]*16, axis=0).ravel())

    mgr.destroy([tensor_a, tensor_b])

    assert tensor_a.is_init() == False
    assert tensor_b.is_init() == False


def test_tensor_rebuild_backwards_compat():
    """
    Test basic OpMult operation
    """

    tensor_in_a = kp.Tensor([2, 2, 2])
    tensor_in_b = kp.Tensor([1, 2, 3])
    tensor_out = kp.Tensor([0, 0, 0])

    mgr = kp.Manager()

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    shader_path = os.path.abspath(os.path.join(DIRNAME, "../../shaders/glsl/opmult.comp.spv"))
    mgr.eval_async_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shader_path)
    mgr.eval_await_def()

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]
    assert np.all(tensor_out.numpy() == [2.0, 4.0, 6.0])


