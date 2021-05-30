# Waifu2x VGG7 implementation

This demonstrates performing image upscaling using Python and vulkan-kompute.

To import the no-noise-compensation VGG7 model (into `model-kipper`):

```
curl -o model.json https://raw.githubusercontent.com/nagadomi/waifu2x/master/models/vgg_7/art/scale2.0x_model.json
python3 import_vgg7.py model.json
```

Other models from the vgg\_7 set (such as `https://raw.githubusercontent.com/nagadomi/waifu2x/master/models/vgg_7/photo/noise0_model.json`) can be subsituted as desired.

To execute that model (no tiling is performed, so be careful about image sizes):

`python3 run_vgg7.py w2wbinit.png out.png`

