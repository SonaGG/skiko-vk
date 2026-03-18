#include <iostream>
#include <jni.h>
#include "ganesh/GrBackendSurface.h"
#include "SkData.h"
#include "SkImage.h"
#include <ganesh/gl/GrGLBackendSurface.h>
#include "ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/SkImageGanesh.h"
#include "ganesh/GrDirectContext.h"
#include "ganesh/gl/GrGLDirectContext.h"


static void deleteBackendTexture(GrBackendTexture* rt) {
    // std::cout << "Deleting [GrBackendTexture " << rt << "]" << std::endl;
    delete rt;
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_skia_BackendTextureKt_BackendTexture_1nGetFinalizer
  (JNIEnv* env, jclass jclass) {
    return static_cast<jlong>(reinterpret_cast<uintptr_t>(&deleteBackendTexture));
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_skia_BackendTextureKt__1nMakeGL
  (JNIEnv* env, jclass jclass, jint width, jint height, jboolean isMipmapped, jint textureId, jint target, jint format) {
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
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_skia_BackendTextureKt__1nGLTextureParametersModified
  (JNIEnv* env, jclass jclass, jlong backendTexturePtr) {
    GrBackendTexture* backendTexture = reinterpret_cast<GrBackendTexture*>(static_cast<uintptr_t>(backendTexturePtr));
    GrBackendTextures::GLTextureParametersModified(backendTexture);
}

#ifdef SK_VULKAN
#include "include/gpu/ganesh/vk/GrVkBackendSurface.h"
#include "include/gpu/ganesh/vk/GrVkTypes.h"

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_skia_BackendTextureKt__1nMakeVulkan
  (JNIEnv* env, jclass jclass, jint width, jint height, jboolean isMipmapped, jlong imagePtr, jint imageTiling, jint imageLayout, jint format, jint imageUsageFlags, jint sampleCnt, jint levelCnt) {
    GrVkImageInfo vkInfo = {};
    vkInfo.fImage = reinterpret_cast<VkImage>(static_cast<uintptr_t>(imagePtr));
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
    return reinterpret_cast<jlong>(instance);
}
#endif // SK_VULKAN
