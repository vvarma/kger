load("@local_config_cuda//cuda:build_defs.bzl", "if_cuda")
load("@local_config_cuda//cuda:build_defs.bzl", "cuda_default_copts")
load("@pip_deps//:requirements.bzl", "requirement")

def pytorch_cpp_extension(
        name,
        srcs = [],
        gpu_srcs = [],
        deps = [],
        copts = [],
        defines = [],
        binary = True,
        linkopts = []):
    """Create a pytorch cpp extension as a cpp and importable python library.

    All options defined below should stay close to the official torch cpp extension options as
    defined in https://github.com/pytorch/pytorch/blob/master/torch/utils/cpp_extension.py.
    """
    name_so = name + ".so"
    torch_deps = [
        requirement("torch", target = "cpp"),
    ]

    cuda_deps = [
        "@local_config_cuda//cuda:cudart_static",
        "@local_config_cuda//cuda:cuda_headers",
    ]

    copts = copts + [
        "-fPIC",
        "-D_GLIBCXX_USE_CXX11_ABI=0",
        "-DTORCH_API_INCLUDE_EXTENSION_H",
        "-fno-strict-aliasing",
        "-fopenmp",
        "-fstack-protector-strong",
        "-fwrapv",
        "-O2",
        "-std=c++14",
        "-DTORCH_EXTENSION_NAME=" + name,
    ]

    if gpu_srcs:
        native.cc_library(
            name = name_so + "_gpu",
            srcs = gpu_srcs,
            deps = deps + torch_deps + if_cuda(cuda_deps),
            copts = copts + cuda_default_copts(),
            defines = defines,
            linkopts = linkopts,
        )
        cuda_deps.extend([":" + name_so + "_gpu"])

    if binary:
        native.cc_binary(
            name = name_so,
            srcs = srcs,
            deps = deps + torch_deps + if_cuda(cuda_deps),
            linkshared = 1,
            copts = copts,
            defines = defines,
            linkopts = linkopts,
        )
    else:
        native.cc_library(
            name = name_so,
            srcs = srcs,
            deps = deps + torch_deps + if_cuda(cuda_deps),
            copts = copts,
            defines = defines,
            linkopts = linkopts,
        )

    native.py_library(
        name = name,
        data = [":" + name_so],
    )
