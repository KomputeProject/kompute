
from kp import Tensor, Manager, Sequence

def test_opmult():
    """
    Test basic OpMult operation
    """

    tensor_in_a = Tensor([2, 2, 2])
    tensor_in_b = Tensor([1, 2, 3])
    tensor_out = Tensor([0, 0, 0])

    mgr = Manager()

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    mgr.eval_algo_mult_def([tensor_in_a, tensor_in_b, tensor_out])

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]

def test_opalgobase_data():
    """
    Test basic OpAlgoBase operation
    """

    tensor_in_a = Tensor([2, 2, 2])
    tensor_in_b = Tensor([1, 2, 3])
    tensor_out = Tensor([0, 0, 0])

    mgr = Manager()

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

    mgr.eval_algo_data_def([tensor_in_a, tensor_in_b, tensor_out], list(shaderData))

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]


def test_opalgobase_file():
    """
    Test basic OpAlgoBase operation
    """

    tensor_in_a = Tensor([2, 2, 2])
    tensor_in_b = Tensor([1, 2, 3])
    tensor_out = Tensor([0, 0, 0])

    mgr = Manager()

    shaderFilePath = "../../shaders/glsl/opmult.comp"

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    mgr.eval_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shaderFilePath)

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]

def test_sequence():
    """
    Test basic OpAlgoBase operation
    """

    mgr = Manager(0, [2])

    tensor_in_a = Tensor([2, 2, 2])
    tensor_in_b = Tensor([1, 2, 3])
    tensor_out = Tensor([0, 0, 0])

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    seq = mgr.create_sequence("op")

    shaderFilePath = "../../shaders/glsl/opmult.comp"
    mgr.eval_async_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shaderFilePath)
    mgr.eval_await_def()

    seq.begin()
    seq.record_tensor_sync_local([tensor_in_a])
    seq.record_tensor_sync_local([tensor_in_b])
    seq.record_tensor_sync_local([tensor_out])
    seq.end()

    seq.eval()

    assert tensor_out.data() == [2.0, 4.0, 6.0]

if __name__ == "__main__":
    test_sequence()
