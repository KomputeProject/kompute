Tutorial 02 - use validation layers - 1
===========
1. Add validation layers into gradle and pack them into apk
1. Enable all validation layers/extensions found on the system
1. Use vulkan wrappers in common/vulkan_wrapper directory

Verification
============
Planted error: this sample sets `VkDeviceQueueCreateInfo::pQueuePriorities` to nullptr,
which will trigger validation layers to notify us from registered callback function
`vkDebugReportCallbackEX_impl();` putting a breakpoint with Android Studo, observe
the breakpoint being triggered.


