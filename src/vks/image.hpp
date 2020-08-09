#pragma once

#include "allocation.hpp"

namespace vks {
// Encaspulates an image, the memory for that image, a view of the image,
// as well as a sampler and the image format.
//
// The sampler is not populated by the allocation code, but is provided
// for convenience and easy cleanup if it is populated.
struct Image : public Allocation {
private:
    using Parent = Allocation;

public:
    vk::Image image;
    vk::Extent3D extent;
    vk::ImageView view;
    vk::Sampler sampler;
    vk::Format format{ vk::Format::eUndefined };

    operator bool() const { return image.operator bool(); }

    void destroy() override {
        if (sampler) {
            device.destroySampler(sampler);
            sampler = vk::Sampler();
        }
        if (view) {
            device.destroyImageView(view);
            view = vk::ImageView();
        }
        if (image) {
            device.destroyImage(image);
            image = vk::Image();
        }
        Parent::destroy();
    }
};
}  // namespace vks
