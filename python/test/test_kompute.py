import os

import kp
import numpy as np

DIRNAME = os.path.dirname(os.path.abspath(__file__))

def test_opmult():
    """
    Test basic OpMult operation
    """

    tensor_in_a = kp.Tensor([2, 2, 2])
    tensor_in_b = kp.Tensor([1, 2, 3])
    tensor_out = kp.Tensor([0, 0, 0])

    mgr = kp.Manager()

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    mgr.eval_algo_mult_def([tensor_in_a, tensor_in_b, tensor_out])

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]
    assert np.all(tensor_out.numpy() == [2.0, 4.0, 6.0])

def test_opalgobase_data():
    """
    Test basic OpAlgoBase operation
    """

    tensor_in_a = kp.Tensor([2, 2, 2])
    tensor_in_b = kp.Tensor([1, 2, 3])
    tensor_out = kp.Tensor([0, 0, 0])

    mgr = kp.Manager()

    shaderData = """
        #version 450

        layout (local_size_x = 1) in;

        // The input tensors bind index is relative to index in parameter passed
        layout(set = 0, binding = 0) buffer bina { float tina[]; };
        layout(set = 0, binding = 1) buffer binb { float tinb[]; };
        layout(set = 0, binding = 2) buffer bout { float tout[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            tout[index] = tina[index] * tinb[index];
        }
    """

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    mgr.eval_algo_str_def([tensor_in_a, tensor_in_b, tensor_out], shaderData)

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]
    assert np.all(tensor_out.numpy() == [2.0, 4.0, 6.0])


def test_opalgobase_file():
    """
    Test basic OpAlgoBase operation
    """

    tensor_in_a = kp.Tensor([2, 2, 2])
    tensor_in_b = kp.Tensor([1, 2, 3])
    tensor_out = kp.Tensor([0, 0, 0])

    mgr = kp.Manager()

    shaderFilePath = os.path.join(DIRNAME, "../../shaders/glsl/opmult.comp")

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    mgr.eval_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shaderFilePath)

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

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    shaderFilePath = os.path.join(DIRNAME, "../../shaders/glsl/opmult.comp")
    mgr.eval_async_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shaderFilePath)

    mgr.eval_await_def()

    seq = mgr.create_sequence("op")
    seq.begin()
    seq.record_tensor_sync_local([tensor_in_a])
    seq.record_tensor_sync_local([tensor_in_b])
    seq.record_tensor_sync_local([tensor_out])
    seq.end()
    seq.eval()

    assert tensor_out.data() == [2.0, 4.0, 6.0]
    assert np.all(tensor_out.numpy() == [2.0, 4.0, 6.0])



def test_workgroup():
    mgr = kp.Manager(0)

    tensor_a = kp.Tensor(np.zeros([16,8]))
    tensor_b = kp.Tensor(np.zeros([16,8]))
    mgr.eval_tensor_create_def([tensor_a, tensor_b])

    shader_src = """
        #version 450

        layout (local_size_x = 1) in;

        // The input tensors bind index is relative to index in parameter passed
        layout(set = 0, binding = 0) writeonly buffer bout  { float toutx[]; };
        layout(set = 0, binding = 1) writeonly buffer bout2 { float touty[]; };

        void main() {
            uint index   = gl_WorkGroupID.x*gl_NumWorkGroups.y + gl_WorkGroupID.y;
            
            toutx[index] = gl_GlobalInvocationID.x;
            touty[index] = gl_GlobalInvocationID.y;
        }
    """
    shader_src = bytes(shader_src, encoding='utf8')

    seq = mgr.create_sequence()
    seq.begin()
    seq.record_algo_data([tensor_a, tensor_b], shader_src, (16,8,1))
    seq.end()
    seq.eval()
    
    mgr.eval_tensor_sync_local_def([tensor_a, tensor_b])
    assert np.all(tensor_a.numpy() == np.stack([np.arange(16)]*8, axis=1).ravel())
    assert np.all(tensor_b.numpy() == np.stack([np.arange(8)]*16, axis=0).ravel())
