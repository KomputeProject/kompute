# Kompute Pi4 Mesa Build Example

The Raspberry Pi 4 is an impressive little computer when you consider that the Broadcom GPU is able to run 2x 4K displays. This example intends to show how to get Vulkan Kompute up and running on a Raspberry Pi 4. This has huge potential for edge processing using the power of the Pi 4 GPU.

Special thanks to [Alejandro Piñeiro](https://blogs.igalia.com/apinheiro/) and others for their work on Broadcom drivers for [Mesa](https://gitlab.freedesktop.org/mesa/mesa) which make this example possible.

## Raspberry Pi Operating System

For this experiment we used [RaspiOS Lite 2021-01-12](https://downloads.raspberrypi.org/raspios_lite_armhf/images/raspios_lite_armhf-2021-01-12/2021-01-11-raspios-buster-armhf-lite.zip), though it is likely best to start with the latest available operation system from [Raspberry Pi operating system images](https://www.raspberrypi.org/software/operating-systems/). In other experiments the full Raspberry Pi operating system (with desktop environment) was found to work. However, when attempting to use Ubuntu on the Raspberry Pi we were not able to run the Python Vulkan Kompute examples.

## Running the Pi headless

By far the easiest way to get up and running with a Raspberry Pi is to configure it for headless operation. This removes the requirement to have a monitor, keyboard or mouse. To run headless the Pi needs access to the internet and for SSH enabled. The following guides from the Raspberry Pi foundation should help.

- [Setting up a Raspberry Pi headless](https://www.raspberrypi.org/documentation/configuration/wireless/headless.md)
- [SSH (Secure Shell)](https://www.raspberrypi.org/documentation/remote-access/ssh/)

## Ensure all packages are using the latest version

```
sudo apt-get update
sudo apt-get upgrade
```

## Install dependencies for building mesa and running Vulkan Kompute

```
sudo apt-get install \
    git build-essential cmake \
    python3-dev python3-mako python3-venv \
    flex bison meson ninja-build \
    libxcb-shm0-dev libxcb1-dev libxcb-*-dev \
    libx11-dev libx11-xcb-dev x11proto-dri2-dev x11proto-dri3-dev \
    libdrm-dev libxshmfence-dev libxrandr-dev libxfixes-dev \
    vulkan-tools libvulkan-dev
```

## Clone mesa repository

```
git clone --depth 1 https://gitlab.freedesktop.org/mesa/mesa.git
```

## Build mesa

Use meson and ninja to build mesa using the Broadcom Vulkan drivers. For information on the Gallium drivers please see [V3D — The Mesa 3D Graphics Library latest documentation](https://docs.mesa3d.org/drivers/v3d.html).

TODO: Experiment adding kmsro and vc4 gallium drivers

```
meson --libdir lib \
    --prefix /mesa-install \
    -D platforms=x11 \
    -D vulkan-drivers=broadcom \
    -D gallium-drivers=v3d \
    -D dri-drivers=[] \
    -D buildtype=debug \
    build

ninja -C build
sudo ninja -C build install
```

## Configure preferred Vulkan driver

Export the path for the Broadcom drivers. This command will need to be run for every new terminal session. Alternatively adding this line to `~/.bash_profile` (os similar) will export the environment variable for every terminal session.

```
export VK_ICD_FILENAMES=/mesa-install/share/vulkan/icd.d/broadcom_icd.armv7l.json
```

To confirm that mesa was configured and built correctly use.

```
vulkaninfo
```

## Clone Vulkan Kompute

Clone Vulkan Kompute for access to the latest Python tests.

```
git clone https://github.com/EthicalML/vulkan-kompute.git
```

## Install dependencies to run the tests 

Navigate to the available tests and install required dependencies.

```
cd vulkan-kompute/python/test
python3 -m venv .venv
source .venv/bin/activate
pip install --upgrade pip wheel
pip install -r requirements-dev.txt
pip install git+git://github.com/EthicalML/vulkan-kompute.git@master
```

## Run the available tests

```
pytest
```