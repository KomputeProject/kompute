
from komputepy import Tensor, Manager, Sequence

def test_opmult():
    """
    Test basic OpMult operation
    """

    tensor_in_a = Tensor([2, 2, 2])
    tensor_in_b = Tensor([1, 2, 3])
    tensor_out = Tensor([0, 0, 0])

    mgr = Manager()

    mgr.evalOpDefaultTensorCreate([tensor_in_a, tensor_in_b, tensor_out])

    mgr.evalOpDefaultAlgoMult([tensor_in_a, tensor_in_b, tensor_out])

    mgr.evalOpDefaultTensorSyncLocal([tensor_out])

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

    mgr.evalOpDefaultTensorCreate([tensor_in_a, tensor_in_b, tensor_out])

    mgr.evalOpDefaultAlgoBaseData([tensor_in_a, tensor_in_b, tensor_out], list(shaderData))

    mgr.evalOpDefaultTensorSyncLocal([tensor_out])

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

    mgr.evalOpDefaultTensorCreate([tensor_in_a, tensor_in_b, tensor_out])

    mgr.evalOpDefaultAlgoBaseFile([tensor_in_a, tensor_in_b, tensor_out], shaderFilePath)

    mgr.evalOpDefaultTensorSyncLocal([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]

def test_sequence():
    """
    Test basic OpAlgoBase operation
    """

    tensor_in_a = Tensor([2, 2, 2])
    tensor_in_b = Tensor([1, 2, 3])
    tensor_out = Tensor([0, 0, 0])

    mgr = Manager()

    shaderFilePath = "../../shaders/glsl/opmult.comp"

    mgr.evalOpDefaultTensorCreate([tensor_in_a, tensor_in_b, tensor_out])

    seq = mgr.createManagedSequence("op")

    seq.begin()
    seq.recordOpAlgoBaseFile([tensor_in_a, tensor_in_b, tensor_out], shaderFilePath)
    seq.end()

    seq.eval()

    mgr.evalOpDefaultTensorSyncLocal([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]

if __name__ == "__main__":
    test_sequence()
