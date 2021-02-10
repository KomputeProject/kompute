/*
  This file contains docstrings for use in the Python bindings.
  Do not edit! They were automatically extracted by pybind11_mkdoc.
 */

#define __EXPAND(x)                                      x
#define __COUNT(_1, _2, _3, _4, _5, _6, _7, COUNT, ...)  COUNT
#define __VA_SIZE(...)                                   __EXPAND(__COUNT(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1))
#define __CAT1(a, b)                                     a ## b
#define __CAT2(a, b)                                     __CAT1(a, b)
#define __DOC1(n1)                                       __doc_##n1
#define __DOC2(n1, n2)                                   __doc_##n1##_##n2
#define __DOC3(n1, n2, n3)                               __doc_##n1##_##n2##_##n3
#define __DOC4(n1, n2, n3, n4)                           __doc_##n1##_##n2##_##n3##_##n4
#define __DOC5(n1, n2, n3, n4, n5)                       __doc_##n1##_##n2##_##n3##_##n4##_##n5
#define __DOC6(n1, n2, n3, n4, n5, n6)                   __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6
#define __DOC7(n1, n2, n3, n4, n5, n6, n7)               __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6##_##n7
#define DOC(...)                                         __EXPAND(__EXPAND(__CAT2(__DOC, __VA_SIZE(__VA_ARGS__)))(__VA_ARGS__))

#if defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


static const char *__doc_kp_Algorithm =
R"doc(Abstraction for compute shaders that are run on top of tensors grouped
via ParameterGroups (which group descriptorsets))doc";

static const char *__doc_kp_Algorithm_Algorithm =
R"doc(Base constructor for Algorithm. Should not be used unless explicit
intended.)doc";

static const char *__doc_kp_Algorithm_Algorithm_2 =
R"doc(Default constructor for Algorithm

@param device The Vulkan device to use for creating resources @param
commandBuffer The vulkan command buffer to bind the pipeline and
shaders)doc";

static const char *__doc_kp_Algorithm_createDescriptorPool = R"doc()doc";

static const char *__doc_kp_Algorithm_createParameters = R"doc()doc";

static const char *__doc_kp_Algorithm_createPipeline = R"doc()doc";

static const char *__doc_kp_Algorithm_createShaderModule = R"doc()doc";

static const char *__doc_kp_Algorithm_init =
R"doc(Initialiser for the shader data provided to the algorithm as well as
tensor parameters that will be used in shader.

@param shaderFileData The bytes in spir-v format of the shader
@tensorParams The Tensors to be used in the Algorithm / shader for
processing)doc";

static const char *__doc_kp_Algorithm_mCommandBuffer = R"doc()doc";

static const char *__doc_kp_Algorithm_mDescriptorPool = R"doc()doc";

static const char *__doc_kp_Algorithm_mDescriptorSet = R"doc()doc";

static const char *__doc_kp_Algorithm_mDescriptorSetLayout = R"doc()doc";

static const char *__doc_kp_Algorithm_mDevice = R"doc()doc";

static const char *__doc_kp_Algorithm_mFreeDescriptorPool = R"doc()doc";

static const char *__doc_kp_Algorithm_mFreeDescriptorSet = R"doc()doc";

static const char *__doc_kp_Algorithm_mFreeDescriptorSetLayout = R"doc()doc";

static const char *__doc_kp_Algorithm_mFreePipeline = R"doc()doc";

static const char *__doc_kp_Algorithm_mFreePipelineCache = R"doc()doc";

static const char *__doc_kp_Algorithm_mFreePipelineLayout = R"doc()doc";

static const char *__doc_kp_Algorithm_mFreeShaderModule = R"doc()doc";

static const char *__doc_kp_Algorithm_mPipeline = R"doc()doc";

static const char *__doc_kp_Algorithm_mPipelineCache = R"doc()doc";

static const char *__doc_kp_Algorithm_mPipelineLayout = R"doc()doc";

static const char *__doc_kp_Algorithm_mShaderModule = R"doc()doc";

static const char *__doc_kp_Algorithm_recordDispatch =
R"doc(Records the dispatch function with the provided template parameters or
alternatively using the size of the tensor by default.

@param x Layout X dispatch value @param y Layout Y dispatch value
@param z Layout Z dispatch value)doc";

static const char *__doc_kp_Manager =
R"doc(Base orchestrator which creates and manages device and child
components)doc";

static const char *__doc_kp_Manager_Manager =
R"doc(Base constructor and default used which creates the base resources
including choosing the device 0 by default.)doc";

static const char *__doc_kp_Manager_Manager_2 =
R"doc(Similar to base constructor but allows the user to provide the device
they would like to create the resources on.

@param physicalDeviceIndex The index of the physical device to use
@param familyQueueIndices (Optional) List of queue indices to add for
explicit allocation @param totalQueues The total number of compute
queues to create.)doc";

static const char *__doc_kp_Manager_Manager_3 =
R"doc(Manager constructor which allows your own vulkan application to
integrate with the vulkan kompute use.

@param instance Vulkan compute instance to base this application
@param physicalDevice Vulkan physical device to use for application
@param device Vulkan logical device to use for all base resources
@param physicalDeviceIndex Index for vulkan physical device used)doc";

static const char *__doc_kp_Manager_createDevice = R"doc()doc";

static const char *__doc_kp_Manager_createInstance = R"doc()doc";

static const char *__doc_kp_Manager_destroy =
R"doc(Destroy owned Vulkan GPU resources and free GPU memory for single
tensor.

@param tensors Single tensor to rebuild)doc";

static const char *__doc_kp_Manager_destroy_2 =
R"doc(Destroy owned Vulkan GPU resources and free GPU memory for vector of
tensors.

@param tensors Single tensor to rebuild)doc";

static const char *__doc_kp_Manager_destroy_3 =
R"doc(Destroy owned Vulkan GPU resources and free GPU memory for vector of
sequences. Destroying by sequence name is more efficent and hence
recommended instead of by object.

@param sequences Vector for shared ptrs with sequences to destroy)doc";

static const char *__doc_kp_Manager_destroy_4 =
R"doc(Destroy owned Vulkan GPU resources and free GPU memory for single
sequence. Destroying by sequence name is more efficent and hence
recommended instead of by object.

@param sequences Single sequence to rebuild)doc";

static const char *__doc_kp_Manager_destroy_5 =
R"doc(Destroy owned Vulkan GPU resources and free GPU memory for sequence by
name.

@param sequenceName Single name of named sequence to destroy)doc";

static const char *__doc_kp_Manager_destroy_6 =
R"doc(Destroy owned Vulkan GPU resources and free GPU memory for sequences
using vector of named sequence names.

@param sequenceName Vector of sequence names to destroy)doc";

static const char *__doc_kp_Manager_evalOp =
R"doc(Function that evaluates operation against named sequence.

@param tensors The tensors to be used in the operation recorded @param
sequenceName The name of the sequence to be retrieved or created
@param TArgs Template parameters that will be used to initialise
Operation to allow for extensible configurations on initialisation)doc";

static const char *__doc_kp_Manager_evalOpAsync =
R"doc(Function that evaluates operation against named sequence
asynchronously.

@param tensors The tensors to be used in the operation recorded @param
sequenceName The name of the sequence to be retrieved or created
@param params Template parameters that will be used to initialise
Operation to allow for extensible configurations on initialisation)doc";

static const char *__doc_kp_Manager_evalOpAsyncDefault =
R"doc(Operation that evaluates operation against default sequence
asynchronously.

@param tensors The tensors to be used in the operation recorded @param
params Template parameters that will be used to initialise Operation
to allow for extensible configurations on initialisation)doc";

static const char *__doc_kp_Manager_evalOpAwait =
R"doc(Operation that awaits for named sequence to finish.

@param sequenceName The name of the sequence to wait for termination
@param waitFor The amount of time to wait before timing out)doc";

static const char *__doc_kp_Manager_evalOpAwaitDefault =
R"doc(Operation that awaits for default sequence to finish.

@param tensors The tensors to be used in the operation recorded @param
params Template parameters that will be used to initialise Operation
to allow for extensible configurations on initialisation)doc";

static const char *__doc_kp_Manager_evalOpDefault =
R"doc(Function that evaluates operation against a newly created sequence.

@param tensors The tensors to be used in the operation recorded @param
TArgs Template parameters that will be used to initialise Operation to
allow for extensible configurations on initialisation)doc";

static const char *__doc_kp_Manager_mComputeQueueFamilyIndices = R"doc()doc";

static const char *__doc_kp_Manager_mComputeQueues = R"doc()doc";

static const char *__doc_kp_Manager_mCurrentSequenceIndex = R"doc()doc";

static const char *__doc_kp_Manager_mDevice = R"doc()doc";

static const char *__doc_kp_Manager_mFreeDevice = R"doc()doc";

static const char *__doc_kp_Manager_mFreeInstance = R"doc()doc";

static const char *__doc_kp_Manager_mInstance = R"doc()doc";

static const char *__doc_kp_Manager_mManagedSequences = R"doc()doc";

static const char *__doc_kp_Manager_mManagedTensors = R"doc()doc";

static const char *__doc_kp_Manager_mPhysicalDevice = R"doc()doc";

static const char *__doc_kp_Manager_mPhysicalDeviceIndex = R"doc()doc";

static const char *__doc_kp_Manager_rebuild =
R"doc(Function that simplifies the common workflow of tensor initialisation.
It will take the constructor parameters for a Tensor and will will us
it to create a new Tensor. The tensor memory will then be managed and
owned by the manager.

@param tensors Array of tensors to rebuild @param syncDataToGPU
Whether to sync the data to GPU memory)doc";

static const char *__doc_kp_Manager_rebuild_2 =
R"doc(Function that simplifies the common workflow of tensor initialisation.
It will take the constructor parameters for a Tensor and will will us
it to create a new Tensor. The tensor memory will then be managed and
owned by the manager.

@param tensors Single tensor to rebuild @param syncDataToGPU Whether
to sync the data to GPU memory)doc";

static const char *__doc_kp_Manager_sequence =
R"doc(Get or create a managed Sequence that will be contained by this
manager. If the named sequence does not currently exist, it would be
created and initialised.

@param sequenceName The name for the named sequence to be retrieved or
created @param queueIndex The queue to use from the available queues
@return Shared pointer to the manager owned sequence resource)doc";

static const char *__doc_kp_Manager_tensor =
R"doc(Function that simplifies the common workflow of tensor creation and
initialization. It will take the constructor parameters for a Tensor
and will will us it to create a new Tensor and then create it. The
tensor memory will then be managed and owned by the manager.

@param data The data to initialize the tensor with @param tensorType
The type of tensor to initialize @param syncDataToGPU Whether to sync
the data to GPU memory @returns Initialized Tensor with memory Syncd
to GPU device)doc";

static const char *__doc_kp_OpAlgoBase =
R"doc(Operation that provides a general abstraction that simplifies the use
of algorithm and parameter components which can be used with shaders.
By default it enables the user to provide a dynamic number of tensors
which are then passed as inputs.)doc";

static const char *__doc_kp_OpAlgoBase_KomputeWorkgroup = R"doc()doc";

static const char *__doc_kp_OpAlgoBase_KomputeWorkgroup_x = R"doc()doc";

static const char *__doc_kp_OpAlgoBase_KomputeWorkgroup_y = R"doc()doc";

static const char *__doc_kp_OpAlgoBase_KomputeWorkgroup_z = R"doc()doc";

static const char *__doc_kp_OpAlgoBase_OpAlgoBase = R"doc(Base constructor, should not be used unless explicitly intended.)doc";

static const char *__doc_kp_OpAlgoBase_OpAlgoBase_2 =
R"doc(Default constructor with parameters that provides the bare minimum
requirements for the operations to be able to create and manage their
sub-components.

@param physicalDevice Vulkan physical device used to find device
queues @param device Vulkan logical device for passing to Algorithm
@param commandBuffer Vulkan Command Buffer to record commands into
@param tensors Tensors that are to be used in this operation @param
shaderFilePath Optional parameter to specify the shader to load
(either in spirv or raw format) @param komputeWorkgroup Optional
parameter to specify the layout for processing)doc";

static const char *__doc_kp_OpAlgoBase_OpAlgoBase_3 =
R"doc(Constructor that enables a file to be passed to the operation with the
contents of the shader. This can be either in raw format or in
compiled SPIR-V binary format.

@param physicalDevice Vulkan physical device used to find device
queues @param device Vulkan logical device for passing to Algorithm
@param commandBuffer Vulkan Command Buffer to record commands into
@param tensors Tensors that are to be used in this operation @param
shaderFilePath Parameter to specify the shader to load (either in
spirv or raw format) @param komputeWorkgroup Optional parameter to
specify the layout for processing)doc";

static const char *__doc_kp_OpAlgoBase_OpAlgoBase_4 =
R"doc(Constructor that enables raw shader data to be passed to the main
operation which can be either in raw shader glsl code or in compiled
SPIR-V binary.

@param physicalDevice Vulkan physical device used to find device
queues @param device Vulkan logical device for passing to Algorithm
@param commandBuffer Vulkan Command Buffer to record commands into
@param tensors Tensors that are to be used in this operation @param
shaderDataRaw Optional parameter to specify the shader data either in
binary or raw form @param komputeWorkgroup Optional parameter to
specify the layout for processing)doc";

static const char *__doc_kp_OpAlgoBase_fetchSpirvBinaryData = R"doc()doc";

static const char *__doc_kp_OpAlgoBase_init =
R"doc(The init function is responsible for the initialisation of the
algorithm component based on the parameters specified, and allows for
extensibility on the options provided. Further dependent classes can
perform more specific checks such as ensuring tensors provided are
initialised, etc.)doc";

static const char *__doc_kp_OpAlgoBase_mAlgorithm = R"doc()doc";

static const char *__doc_kp_OpAlgoBase_mFreeAlgorithm = R"doc()doc";

static const char *__doc_kp_OpAlgoBase_mKomputeWorkgroup = R"doc()doc";

static const char *__doc_kp_OpAlgoBase_mShaderDataRaw =
R"doc(< Optional member variable which can be provided to contain either the
raw shader content or the spirv binary content)doc";

static const char *__doc_kp_OpAlgoBase_mShaderFilePath =
R"doc(< Optional member variable which can be provided for the OpAlgoBase to
find the data automatically and load for processing)doc";

static const char *__doc_kp_OpAlgoBase_postEval =
R"doc(Executes after the recorded commands are submitted, and performs a
copy of the GPU Device memory into the staging buffer so the output
data can be retrieved.)doc";

static const char *__doc_kp_OpAlgoBase_preEval = R"doc(Does not perform any preEval commands.)doc";

static const char *__doc_kp_OpAlgoBase_record =
R"doc(This records the commands that are to be sent to the GPU. This
includes the barriers that ensure the memory has been copied before
going in and out of the shader, as well as the dispatch operation that
sends the shader processing to the gpu. This function also records the
GPU memory copy of the output data for the staging buffer so it can be
read by the host.)doc";

static const char *__doc_kp_OpAlgoLhsRhsOut =
R"doc(Operation base class to simplify the creation of operations that
require right hand and left hand side datapoints together with a
single output. The expected data passed is two input tensors and one
output tensor.)doc";

static const char *__doc_kp_OpAlgoLhsRhsOut_OpAlgoLhsRhsOut = R"doc(Base constructor, should not be used unless explicitly intended.)doc";

static const char *__doc_kp_OpAlgoLhsRhsOut_OpAlgoLhsRhsOut_2 =
R"doc(Default constructor with parameters that provides the bare minimum
requirements for the operations to be able to create and manage their
sub-components.

@param physicalDevice Vulkan physical device used to find device
queues @param device Vulkan logical device for passing to Algorithm
@param commandBuffer Vulkan Command Buffer to record commands into
@param tensors Tensors that are to be used in this operation @param
freeTensors Whether operation manages the memory of the Tensors @param
komputeWorkgroup Optional parameter to specify the layout for
processing)doc";

static const char *__doc_kp_OpAlgoLhsRhsOut_init =
R"doc(The init function is responsible for ensuring that all of the tensors
provided are aligned with requirements such as LHS, RHS and Output
tensors, and creates the algorithm component which processes the
computation.)doc";

static const char *__doc_kp_OpAlgoLhsRhsOut_mTensorLHS =
R"doc(< Reference to the parameter used in the left hand side equation of
the shader)doc";

static const char *__doc_kp_OpAlgoLhsRhsOut_mTensorOutput =
R"doc(< Reference to the parameter used in the output of the shader and will
be copied with a staging vector)doc";

static const char *__doc_kp_OpAlgoLhsRhsOut_mTensorRHS =
R"doc(< Reference to the parameter used in the right hand side equation of
the shader)doc";

static const char *__doc_kp_OpAlgoLhsRhsOut_postEval =
R"doc(Executes after the recorded commands are submitted, and performs a
copy of the GPU Device memory into the staging buffer so the output
data can be retrieved.)doc";

static const char *__doc_kp_OpAlgoLhsRhsOut_record =
R"doc(This records the commands that are to be sent to the GPU. This
includes the barriers that ensure the memory has been copied before
going in and out of the shader, as well as the dispatch operation that
sends the shader processing to the gpu. This function also records the
GPU memory copy of the output data for the staging buffer so it can be
read by the host.)doc";

static const char *__doc_kp_OpBase =
R"doc(Base Operation which provides the high level interface that Kompute
operations implement in order to perform a set of actions in the GPU.

Operations can perform actions on tensors, and optionally can also own
an Algorithm with respective parameters. kp::Operations with
kp::Algorithms would inherit from kp::OpBaseAlgo.)doc";

static const char *__doc_kp_OpBase_OpBase = R"doc(Base constructor, should not be used unless explicitly intended.)doc";

static const char *__doc_kp_OpBase_OpBase_2 =
R"doc(Default constructor with parameters that provides the bare minimum
requirements for the operations to be able to create and manage their
sub-components.

@param physicalDevice Vulkan physical device used to find device
queues @param device Vulkan logical device for passing to Algorithm
@param commandBuffer Vulkan Command Buffer to record commands into
@param tensors Tensors that are to be used in this operation)doc";

static const char *__doc_kp_OpBase_init =
R"doc(The init function is responsible for setting up all the resources and
should be called after the Operation has been created.)doc";

static const char *__doc_kp_OpBase_mCommandBuffer = R"doc(< Vulkan Command Buffer)doc";

static const char *__doc_kp_OpBase_mDevice = R"doc(< Vulkan Logical Device)doc";

static const char *__doc_kp_OpBase_mFreeTensors =
R"doc(< Explicit boolean that specifies whether the < tensors are freed (if
they are managed))doc";

static const char *__doc_kp_OpBase_mPhysicalDevice = R"doc(< Vulkan Physical Device)doc";

static const char *__doc_kp_OpBase_mTensors =
R"doc(< Tensors referenced by operation that can be managed < optionally by
operation)doc";

static const char *__doc_kp_OpBase_postEval =
R"doc(Post eval is called after the Sequence has called eval and submitted
the commands to the GPU for processing, and can be used to perform any
tear-down steps required as the computation iteration finishes. It's
worth noting that there are situations where eval can be called
multiple times, so the resources that are destroyed should not require
a re-init unless explicitly provided by the user.)doc";

static const char *__doc_kp_OpBase_preEval =
R"doc(Pre eval is called before the Sequence has called eval and submitted
the commands to the GPU for processing, and can be used to perform any
per-eval setup steps required as the computation iteration begins.
It's worth noting that there are situations where eval can be called
multiple times, so the resources that are created should be idempotent
in case it's called multiple times in a row.)doc";

static const char *__doc_kp_OpBase_record =
R"doc(The record function is intended to only send a record command or run
commands that are expected to record operations that are to be
submitted as a batch into the GPU.)doc";

static const char *__doc_kp_OpMult =
R"doc(Operation that performs multiplication on two tensors and outpus on
third tensor.)doc";

static const char *__doc_kp_OpMult_OpMult = R"doc(Base constructor, should not be used unless explicitly intended.)doc";

static const char *__doc_kp_OpMult_OpMult_2 =
R"doc(Default constructor with parameters that provides the bare minimum
requirements for the operations to be able to create and manage their
sub-components.

@param physicalDevice Vulkan physical device used to find device
queues @param device Vulkan logical device for passing to Algorithm
@param commandBuffer Vulkan Command Buffer to record commands into
@param tensors Tensors that are to be used in this operation @param
komputeWorkgroup Optional parameter to specify the layout for
processing)doc";

static const char *__doc_kp_OpTensorCopy =
R"doc(Operation that copies the data from the first tensor to the rest of
the tensors provided, using a record command for all the vectors. This
operation does not own/manage the memory of the tensors passed to it.
The operation must only receive tensors of type)doc";

static const char *__doc_kp_OpTensorCopy_OpTensorCopy = R"doc()doc";

static const char *__doc_kp_OpTensorCopy_OpTensorCopy_2 =
R"doc(Default constructor with parameters that provides the core vulkan
resources and the tensors that will be used in the operation.

@param physicalDevice Vulkan physical device used to find device
queues @param device Vulkan logical device for passing to Algorithm
@param commandBuffer Vulkan Command Buffer to record commands into
@param tensors Tensors that will be used to create in operation.)doc";

static const char *__doc_kp_OpTensorCopy_init =
R"doc(Performs basic checks such as ensuring there are at least two tensors
provided, that they are initialised and that they are not of type
TensorTypes::eStorage.)doc";

static const char *__doc_kp_OpTensorCopy_postEval =
R"doc(Copies the local vectors for all the tensors to sync the data with the
gpu.)doc";

static const char *__doc_kp_OpTensorCopy_preEval = R"doc(Does not perform any preEval commands.)doc";

static const char *__doc_kp_OpTensorCopy_record =
R"doc(Records the copy commands from the first tensor into all the other
tensors provided. Also optionally records a barrier.)doc";

static const char *__doc_kp_OpTensorSyncDevice =
R"doc(Operation that syncs tensor's device by mapping local data into the
device memory. For TensorTypes::eDevice it will use a record operation
for the memory to be syncd into GPU memory which means that the
operation will be done in sync with GPU commands. For
TensorTypes::eStaging it will only map the data into host memory which
will happen during preEval before the recorded commands are
dispatched. This operation won't have any effect on
TensorTypes::eStaging.)doc";

static const char *__doc_kp_OpTensorSyncDevice_OpTensorSyncDevice = R"doc()doc";

static const char *__doc_kp_OpTensorSyncDevice_OpTensorSyncDevice_2 =
R"doc(Default constructor with parameters that provides the core vulkan
resources and the tensors that will be used in the operation. The
tensos provided cannot be of type TensorTypes::eStorage.

@param physicalDevice Vulkan physical device used to find device
queues @param device Vulkan logical device for passing to Algorithm
@param commandBuffer Vulkan Command Buffer to record commands into
@param tensors Tensors that will be used to create in operation.)doc";

static const char *__doc_kp_OpTensorSyncDevice_init =
R"doc(Performs basic checks such as ensuring that there is at least one
tensor provided with min memory of 1 element.)doc";

static const char *__doc_kp_OpTensorSyncDevice_postEval = R"doc(Does not perform any postEval commands.)doc";

static const char *__doc_kp_OpTensorSyncDevice_preEval = R"doc(Does not perform any preEval commands.)doc";

static const char *__doc_kp_OpTensorSyncDevice_record =
R"doc(For device tensors, it records the copy command for the tensor to copy
the data from its staging to device memory.)doc";

static const char *__doc_kp_OpTensorSyncLocal =
R"doc(Operation that syncs tensor's local memory by mapping device data into
the local CPU memory. For TensorTypes::eDevice it will use a record
operation for the memory to be syncd into GPU memory which means that
the operation will be done in sync with GPU commands. For
TensorTypes::eStaging it will only map the data into host memory which
will happen during preEval before the recorded commands are
dispatched. This operation won't have any effect on
TensorTypes::eStaging.)doc";

static const char *__doc_kp_OpTensorSyncLocal_OpTensorSyncLocal = R"doc()doc";

static const char *__doc_kp_OpTensorSyncLocal_OpTensorSyncLocal_2 =
R"doc(Default constructor with parameters that provides the core vulkan
resources and the tensors that will be used in the operation. The
tensors provided cannot be of type TensorTypes::eStorage.

@param physicalDevice Vulkan physical device used to find device
queues @param device Vulkan logical device for passing to Algorithm
@param commandBuffer Vulkan Command Buffer to record commands into
@param tensors Tensors that will be used to create in operation.)doc";

static const char *__doc_kp_OpTensorSyncLocal_init =
R"doc(Performs basic checks such as ensuring that there is at least one
tensor provided with min memory of 1 element.)doc";

static const char *__doc_kp_OpTensorSyncLocal_postEval =
R"doc(For host tensors it performs the map command from the host memory into
local memory.)doc";

static const char *__doc_kp_OpTensorSyncLocal_preEval = R"doc(Does not perform any preEval commands.)doc";

static const char *__doc_kp_OpTensorSyncLocal_record =
R"doc(For device tensors, it records the copy command for the tensor to copy
the data from its device to staging memory.)doc";

static const char *__doc_kp_Sequence = R"doc(Container of operations that can be sent to GPU as batch)doc";

static const char *__doc_kp_Sequence_Sequence =
R"doc(Base constructor for Sequence. Should not be used unless explicit
intended.)doc";

static const char *__doc_kp_Sequence_Sequence_2 =
R"doc(Main constructor for sequence which requires core vulkan components to
generate all dependent resources.

@param physicalDevice Vulkan physical device @param device Vulkan
logical device @param computeQueue Vulkan compute queue @param
queueIndex Vulkan compute queue index in device)doc";

static const char *__doc_kp_Sequence_begin =
R"doc(Begins recording commands for commands to be submitted into the
command buffer.

@return Boolean stating whether execution was successful.)doc";

static const char *__doc_kp_Sequence_createCommandBuffer = R"doc()doc";

static const char *__doc_kp_Sequence_createCommandPool = R"doc()doc";

static const char *__doc_kp_Sequence_end =
R"doc(Ends the recording and stops recording commands when the record
command is sent.

@return Boolean stating whether execution was successful.)doc";

static const char *__doc_kp_Sequence_eval =
R"doc(Eval sends all the recorded and stored operations in the vector of
operations into the gpu as a submit job with a barrier.

@return Boolean stating whether execution was successful.)doc";

static const char *__doc_kp_Sequence_evalAsync =
R"doc(Eval Async sends all the recorded and stored operations in the vector
of operations into the gpu as a submit job with a barrier. EvalAwait()
must be called after to ensure the sequence is terminated correctly.

@return Boolean stating whether execution was successful.)doc";

static const char *__doc_kp_Sequence_evalAwait =
R"doc(Eval Await waits for the fence to finish processing and then once it
finishes, it runs the postEval of all operations.

@param waitFor Number of milliseconds to wait before timing out.
@return Boolean stating whether execution was successful.)doc";

static const char *__doc_kp_Sequence_freeMemoryDestroyGPUResources =
R"doc(Destroys and frees the GPU resources which include the buffer and
memory and sets the sequence as init=False.)doc";

static const char *__doc_kp_Sequence_init =
R"doc(Initialises sequence including the creation of the command pool and
the command buffer.)doc";

static const char *__doc_kp_Sequence_isInit =
R"doc(Returns true if the sequence has been successfully initialised.

@return Boolean stating if sequence has been initialised.)doc";

static const char *__doc_kp_Sequence_isRecording =
R"doc(Returns true if the sequence is currently in recording activated.

@return Boolean stating if recording ongoing.)doc";

static const char *__doc_kp_Sequence_isRunning =
R"doc(Returns true if the sequence is currently running - mostly used for
async workloads.

@return Boolean stating if currently running.)doc";

static const char *__doc_kp_Sequence_mCommandBuffer = R"doc()doc";

static const char *__doc_kp_Sequence_mCommandPool = R"doc()doc";

static const char *__doc_kp_Sequence_mComputeQueue = R"doc()doc";

static const char *__doc_kp_Sequence_mDevice = R"doc()doc";

static const char *__doc_kp_Sequence_mFence = R"doc()doc";

static const char *__doc_kp_Sequence_mFreeCommandBuffer = R"doc()doc";

static const char *__doc_kp_Sequence_mFreeCommandPool = R"doc()doc";

static const char *__doc_kp_Sequence_mIsInit = R"doc()doc";

static const char *__doc_kp_Sequence_mIsRunning = R"doc()doc";

static const char *__doc_kp_Sequence_mOperations = R"doc()doc";

static const char *__doc_kp_Sequence_mPhysicalDevice = R"doc()doc";

static const char *__doc_kp_Sequence_mQueueIndex = R"doc()doc";

static const char *__doc_kp_Sequence_mRecording = R"doc()doc";

static const char *__doc_kp_Sequence_record =
R"doc(Record function for operation to be added to the GPU queue in batch.
This template requires classes to be derived from the OpBase class.
This function also requires the Sequence to be recording, otherwise it
will not be able to add the operation.

@param tensors Vector of tensors to use for the operation @param TArgs
Template parameters that are used to initialise operation which allows
for extensible configurations on initialisation.)doc";

static const char *__doc_kp_Tensor =
R"doc(Structured data used in GPU operations.

Tensors are the base building block in Kompute to perform operations
across GPUs. Each tensor would have a respective Vulkan memory and
buffer, which would be used to store their respective data. The
tensors can be used for GPU data storage or transfer.)doc";

static const char *__doc_kp_Tensor_Tensor = R"doc(Base constructor, should not be used unless explicitly intended.)doc";

static const char *__doc_kp_Tensor_Tensor_2 =
R"doc(Default constructor with data provided which would be used to create
the respective vulkan buffer and memory.

@param data Non-zero-sized vector of data that will be used by the
tensor @param tensorType Type for the tensor which is of type
TensorTypes)doc";

static const char *__doc_kp_Tensor_TensorTypes =
R"doc(Type for tensors created: Device allows memory to be transferred from
staging buffers. Staging are host memory visible. Storage are device
visible but are not set up to transfer or receive data (only for
shader storage).)doc";

static const char *__doc_kp_Tensor_TensorTypes_eDevice = R"doc(< Type is device memory, source and destination)doc";

static const char *__doc_kp_Tensor_TensorTypes_eHost = R"doc(< Type is host memory, source and destination)doc";

static const char *__doc_kp_Tensor_TensorTypes_eStorage = R"doc(< Type is Device memory (only))doc";

static const char *__doc_kp_Tensor_allocateBindMemory = R"doc()doc";

static const char *__doc_kp_Tensor_allocateMemoryCreateGPUResources = R"doc()doc";

static const char *__doc_kp_Tensor_constructDescriptorBufferInfo =
R"doc(Constructs a vulkan descriptor buffer info which can be used to
specify and reference the underlying buffer component of the tensor
without exposing it.

@return Descriptor buffer info with own buffer)doc";

static const char *__doc_kp_Tensor_copyBuffer = R"doc()doc";

static const char *__doc_kp_Tensor_createBuffer = R"doc()doc";

static const char *__doc_kp_Tensor_data =
R"doc(Returns the vector of data currently contained by the Tensor. It is
important to ensure that there is no out-of-sync data with the GPU
memory.

@return Reference to vector of elements representing the data in the
tensor.)doc";

static const char *__doc_kp_Tensor_freeMemoryDestroyGPUResources =
R"doc(Destroys and frees the GPU resources which include the buffer and
memory.)doc";

static const char *__doc_kp_Tensor_getPrimaryBufferUsageFlags = R"doc()doc";

static const char *__doc_kp_Tensor_getPrimaryMemoryPropertyFlags = R"doc()doc";

static const char *__doc_kp_Tensor_getStagingBufferUsageFlags = R"doc()doc";

static const char *__doc_kp_Tensor_getStagingMemoryPropertyFlags = R"doc()doc";

static const char *__doc_kp_Tensor_init =
R"doc(Initialiser which calls the initialisation for all the respective
tensors as well as creates the respective staging tensors. The staging
tensors would only be created for the tensors of type
TensorType::eDevice as otherwise there is no need to copy from host
memory.)doc";

static const char *__doc_kp_Tensor_isInit =
R"doc(Returns true if the tensor initialisation function has been carried
out successful, which would mean that the buffer and memory will have
been provisioned.)doc";

static const char *__doc_kp_Tensor_mData = R"doc()doc";

static const char *__doc_kp_Tensor_mDevice = R"doc()doc";

static const char *__doc_kp_Tensor_mFreePrimaryBuffer = R"doc()doc";

static const char *__doc_kp_Tensor_mFreePrimaryMemory = R"doc()doc";

static const char *__doc_kp_Tensor_mFreeStagingBuffer = R"doc()doc";

static const char *__doc_kp_Tensor_mFreeStagingMemory = R"doc()doc";

static const char *__doc_kp_Tensor_mIsInit = R"doc()doc";

static const char *__doc_kp_Tensor_mPhysicalDevice = R"doc()doc";

static const char *__doc_kp_Tensor_mPrimaryBuffer = R"doc()doc";

static const char *__doc_kp_Tensor_mPrimaryMemory = R"doc()doc";

static const char *__doc_kp_Tensor_mShape = R"doc()doc";

static const char *__doc_kp_Tensor_mStagingBuffer = R"doc()doc";

static const char *__doc_kp_Tensor_mStagingMemory = R"doc()doc";

static const char *__doc_kp_Tensor_mTensorType = R"doc()doc";

static const char *__doc_kp_Tensor_mapDataFromHostMemory =
R"doc(Maps data from the Host Visible GPU memory into the data vector. It
requires the Tensor to be of staging type for it to work.)doc";

static const char *__doc_kp_Tensor_mapDataIntoHostMemory =
R"doc(Maps data from the data vector into the Host Visible GPU memory. It
requires the tensor to be of staging type for it to work.)doc";

static const char *__doc_kp_Tensor_memorySize = R"doc()doc";

static const char *__doc_kp_Tensor_operator_array =
R"doc(Overrides the subscript operator to expose the underlying data's
subscript operator which in this case would be its underlying
vector's.

@param i The index where the element will be returned from. @return
Returns the element in the position requested.)doc";

static const char *__doc_kp_Tensor_recordBufferMemoryBarrier =
R"doc(Records the buffer memory barrier into the command buffer which
ensures that relevant data transfers are carried out correctly.

@param commandBuffer Vulkan Command Buffer to record the commands into
@param srcAccessMask Access flags for source access mask @param
dstAccessMask Access flags for destination access mask @param
scrStageMask Pipeline stage flags for source stage mask @param
dstStageMask Pipeline stage flags for destination stage mask)doc";

static const char *__doc_kp_Tensor_recordCopyFrom =
R"doc(Records a copy from the memory of the tensor provided to the current
thensor. This is intended to pass memory into a processing, to perform
a staging buffer transfer, or to gather output (between others).

@param commandBuffer Vulkan Command Buffer to record the commands into
@param copyFromTensor Tensor to copy the data from @param
createBarrier Whether to create a barrier that ensures the data is
copied before further operations. Default is true.)doc";

static const char *__doc_kp_Tensor_recordCopyFromDeviceToStaging =
R"doc(Records a copy from the internal device memory to the staging memory
using an optional barrier to wait for the operation. This function
would only be relevant for kp::Tensors of type eDevice.

@param commandBuffer Vulkan Command Buffer to record the commands into
@param createBarrier Whether to create a barrier that ensures the data
is copied before further operations. Default is true.)doc";

static const char *__doc_kp_Tensor_recordCopyFromStagingToDevice =
R"doc(Records a copy from the internal staging memory to the device memory
using an optional barrier to wait for the operation. This function
would only be relevant for kp::Tensors of type eDevice.

@param commandBuffer Vulkan Command Buffer to record the commands into
@param createBarrier Whether to create a barrier that ensures the data
is copied before further operations. Default is true.)doc";

static const char *__doc_kp_Tensor_setData =
R"doc(Sets / resets the vector data of the tensor. This function does not
perform any copies into GPU memory and is only performed on the host.)doc";

static const char *__doc_kp_Tensor_shape =
R"doc(Returns the shape of the tensor, which includes the number of
dimensions and the size per dimension.

@return Array containing the sizes for each dimension. Zero means
respective dimension is not active.)doc";

static const char *__doc_kp_Tensor_size =
R"doc(Returns the size/magnitude of the Tensor, which will be the total
number of elements across all dimensions

@return Unsigned integer representing the total number of elements)doc";

static const char *__doc_kp_Tensor_tensorType =
R"doc(Retrieve the tensor type of the Tensor

@return Tensor type of tensor)doc";

#if defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

