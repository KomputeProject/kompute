# Waifu2x VGG7 implementation

This demonstrates performing image upscaling using Python and vulkan-kompute.

To import an existing VGG7 model (assuming you have https://github.com/nagadomi/waifu2x/ cloned somewhere):

`python3 import_vgg7.py waifu2x/models/vgg_7/art/scale2.0x_model.json`

To execute that model (no tiling is performed, so be careful about image sizes):

`python3 run_vgg7.py w2wbinit.png out.png`

