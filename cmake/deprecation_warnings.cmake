if(KOMPUTE_OPT_REPO_SUBMODULE_BUILD)
    message(FATAL_ERROR "'KOMPUTE_OPT_REPO_SUBMODULE_BUILD' got replaced by 'KOMPUTE_OPT_USE_BUILD_IN_SPDLOG', 'KOMPUTE_OPT_USE_BUILD_IN_FMT', 'KOMPUTE_OPT_USE_BUILD_IN_GOOGLE_TEST', 'KOMPUTE_OPT_USE_BUILD_IN_PYBIND11' and 'KOMPUTE_OPT_USE_BUILD_IN_VULKAN_HEADER'. Please use them instead.")
endif()