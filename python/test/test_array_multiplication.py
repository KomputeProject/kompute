import pyshader as ps
import kp


def test_array_multiplication():

    @ps.python2shader
    def compute_shader_multiply(index=("input", "GlobalInvocationId", ps.ivec3),
                                data1=("buffer", 0, ps.Array(ps.f32)),
                                data2=("buffer", 1, ps.Array(ps.f32)),
                                data3=("buffer", 2, ps.Array(ps.f32))):
        i = index.x
        data3[i] = data1[i] * data2[i]

    tensor_in_a = kp.Tensor([2, 2, 2])
    tensor_in_b = kp.Tensor([1, 2, 3])
    tensor_out = kp.Tensor([0, 0, 0])

    mgr = kp.Manager()

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])
    mgr.eval_algo_data_def([tensor_in_a, tensor_in_b, tensor_out], compute_shader_multiply.to_spirv())
    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]
