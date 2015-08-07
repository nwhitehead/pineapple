from ipykernel.kernelapp import IPKernelApp
from .kernel import EchoKernel

IPKernelApp.launch_instance(kernel_class=EchoKernel)
