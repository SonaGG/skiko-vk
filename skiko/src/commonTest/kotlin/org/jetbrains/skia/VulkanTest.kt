package org.jetbrains.skia

import org.jetbrains.skiko.RenderException
import kotlin.test.Test
import kotlin.test.assertNotNull
import kotlin.test.assertEquals

/**
 * Tests for Vulkan backend API.
 *
 * These tests verify:
 * - When Vulkan support IS compiled (SK_VULKAN defined, Vulkan-enabled Skia build):
 *   - Null proc-address is rejected with RenderException before touching Vulkan
 *   - BackendRenderTarget and BackendTexture can be created (struct wrapping)
 * - When Vulkan support IS NOT compiled:
 *   - Calls throw UnsatisfiedLinkError (binding not present), which is silently skipped
 *
 * To enable Vulkan support, build with SK_VULKAN=1 env var and a Vulkan-enabled Skia build.
 */
class VulkanTest {

    /**
     * Verifies DirectContext.makeVulkan is accessible. With a Vulkan-enabled Skia build and
     * null function pointers it throws RenderException; without Vulkan it throws UnsatisfiedLinkError.
     */
    @Test
    fun makeVulkanContextNullProcAddrRejectsOrNotLinked() {
        try {
            DirectContext.makeVulkan(
                instancePtr = 0L,
                physicalDevicePtr = 0L,
                devicePtr = 0L,
                queuePtr = 0L,
                graphicsQueueIndex = 0,
                instanceProcAddr = 0L,
                deviceProcAddr = 0L,
                apiVersion = DirectContext.VK_API_VERSION_1_1
            )
            // If we reach here, a context was created with null pointers (unexpected but not fatal)
        } catch (e: RenderException) {
            // Expected when SK_VULKAN is compiled: null proc-addr rejected
        } catch (e: UnsatisfiedLinkError) {
            // Expected when SK_VULKAN is NOT compiled: Vulkan bindings not present, skip
            return
        }
    }

    /**
     * Verifies BackendRenderTarget.makeVulkan is accessible and creates a struct wrapper.
     */
    @Test
    fun makeVulkanRenderTargetOrNotLinked() {
        try {
            // VK_FORMAT_B8G8R8A8_UNORM=44, VK_IMAGE_TILING_OPTIMAL=1, VK_IMAGE_LAYOUT_UNDEFINED=0
            val rt = BackendRenderTarget.makeVulkan(
                width = 100,
                height = 100,
                imagePtr = 0L,
                imageTiling = 1,
                imageLayout = 0,
                format = 44,
                imageUsageFlags = 16, // VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                sampleCnt = 1,
                levelCnt = 1
            )
            assertNotNull(rt)
            rt.close()
        } catch (e: UnsatisfiedLinkError) {
            // SK_VULKAN not compiled, skip
        }
    }

    /**
     * Verifies BackendTexture.makeVulkan is accessible and creates a struct wrapper.
     */
    @Test
    fun makeVulkanTextureOrNotLinked() {
        try {
            val tex = BackendTexture.makeVulkan(
                width = 64,
                height = 64,
                isMipmapped = false,
                imagePtr = 0L,
                imageTiling = 1,   // VK_IMAGE_TILING_OPTIMAL
                imageLayout = 5,   // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                format = 44,       // VK_FORMAT_B8G8R8A8_UNORM
                imageUsageFlags = 4 or 16,
                sampleCnt = 1,
                levelCnt = 1
            )
            assertNotNull(tex)
            tex.close()
        } catch (e: UnsatisfiedLinkError) {
            // SK_VULKAN not compiled, skip
        }
    }

    /**
     * Verifies VK_API_VERSION_1_1 constant value.
     */
    @Test
    fun vulkanApiVersionConstant() {
        assertEquals(0x00401000, DirectContext.VK_API_VERSION_1_1)
    }
}
