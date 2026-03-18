#include <iostream>
#include <stdint.h>
#include "common.h"
#include "gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/SkImageGanesh.h"
#include "include/gpu/ganesh/gl/GrGLTypes.h"
#include "gpu/ganesh/GrBackendSurface.h"

static void deleteBackendTexture(GrBackendTexture* rt) {
    delete rt;
}
SKIKO_EXPORT KNativePointer org_jetbrains_skia_BackendTexture__1nGetFinalizer() {
    return reinterpret_cast<KNativePointer>(&deleteBackendTexture);
}

SKIKO_EXPORT KNativePointer org_jetbrains_skia_BackendTexture__1nMakeGL
  (KInt width, KInt height, KBoolean isMipmapped, KInt textureId, KInt target, KInt format) {
    GrGLTextureInfo textureInfo;
    textureInfo.fID = static_cast<GrGLuint>(textureId);
    textureInfo.fTarget = static_cast<GrGLenum>(target);
    textureInfo.fFormat = static_cast<GrGLenum>(format);

    GrBackendTexture obj = GrBackendTextures::MakeGL(
        width,
        height,
        isMipmapped ? skgpu::Mipmapped::kYes : skgpu::Mipmapped::kNo,
        textureInfo
    );

    GrBackendTexture* instance = new GrBackendTexture(obj);
    return instance;
}

SKIKO_EXPORT void org_jetbrains_skia_BackendTexture__1nGLTextureParametersModified
  (KNativePointer backendTexturePtr) {
    GrBackendTexture* backendTexture = reinterpret_cast<GrBackendTexture*>(backendTexturePtr);
    GrBackendTextures::GLTextureParametersModified(backendTexture);
}

#ifdef SK_VULKAN
#include "include/gpu/ganesh/vk/GrVkBackendSurface.h"
#include "include/gpu/ganesh/vk/GrVkTypes.h"
#endif

SKIKO_EXPORT KNativePointer org_jetbrains_skia_BackendTexture__1nMakeVulkan
  (KInt width, KInt height, KBoolean isMipmapped, KNativePointer imagePtr, KInt imageTiling, KInt imageLayout, KInt format, KInt imageUsageFlags, KInt sampleCnt, KInt levelCnt) {
#ifdef SK_VULKAN
    GrVkImageInfo vkInfo = {};
    vkInfo.fImage = reinterpret_cast<VkImage>(imagePtr);
    vkInfo.fAlloc = {};
    vkInfo.fImageTiling = static_cast<VkImageTiling>(imageTiling);
    vkInfo.fImageLayout = static_cast<VkImageLayout>(imageLayout);
    vkInfo.fFormat = static_cast<VkFormat>(format);
    vkInfo.fImageUsageFlags = static_cast<VkImageUsageFlags>(imageUsageFlags);
    vkInfo.fSampleCount = static_cast<uint32_t>(sampleCnt);
    vkInfo.fLevelCount = static_cast<uint32_t>(levelCnt);

    // For Vulkan, mipmapping is encoded in fLevelCount; MakeVk has no separate Mipmapped param
    GrBackendTexture obj = GrBackendTextures::MakeVk(width, height, vkInfo);

    GrBackendTexture* instance = new GrBackendTexture(obj);
    return instance;
#else
    return 0;
#endif // SK_VULKAN
}