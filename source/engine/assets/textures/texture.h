#include "descriptor.h"
struct GraphicsSetup;

struct Textures {
    VkDevice *device;
    struct descriptor descriptor;

    size_t quantity;
    struct Data {
        VkImage image;
        VkDeviceMemory imageMemory;
        uint32_t mipLevels;
        VkImageView imageView;
        VkSampler sampler;
    } *data;
};

struct Textures *loadCubeMaps(struct GraphicsSetup *graphics, const char *texturePath[6]);
struct Textures *loadTextures(struct GraphicsSetup *graphics, uint32_t texturesQuantity, const char *texturePath[static texturesQuantity]);
void unloadTextures(void *texturePtr);
