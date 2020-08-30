
Memory Management Principles
=====================

The principle in Vulkan Kompute on memory management is summarised as follows:
* Explicit is better than implicit for specifying memory management 
* Interfaces for memory management are constant until freed
* Memory management responsibilities are acyclic from static object references

Vulkan Kompute is responsible for managing both the CPU and GPU memory allocations and resources, and is important that they are able to explicitly define when these objects are released or destroyed. Similarly, it's important that the memory resources created by the application are released safely.

Vulkan Kompute is built with the BYOV principle in mind (Bring your own Vulkan). This means that even though the top level resources are managing the memory to its owned resources, they themselves may not have full ownership of the GPU / Vulkan components themselves.


