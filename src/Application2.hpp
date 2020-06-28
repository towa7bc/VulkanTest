//
// Created by Michael Wittmann on 25/06/2020.
//

#ifndef VULKANTEST_APPLICATION2_HPP
#define VULKANTEST_APPLICATION2_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure
#include <imgui.h>

#include <algorithm>
#include <array>
#include <assimp/Importer.hpp>  // C++ importer interface
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <future>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "stb_image.hpp"

constexpr uint32_t WIDTH = 1280;
constexpr uint32_t HEIGHT = 720;

static int XPOSITION{0};
static int YPOSITION{0};
static bool TOGGLEFULLSCREEN{true};
static float ZOOMDEGREES{45.0f};
static float ROTATEDEGREES{45.0f};
static float TRANSLATEFACTOR{0.0f};

const std::string MODEL_PATH = "../../src/models/viking_room.obj";
const std::string TEXTURE_PATH = "../../src/textures/viking_room.png";

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator);

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoord;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::array<VkVertexInputAttributeDescription, 3>
  getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
  }

  bool operator==(const Vertex& other) const {
    return pos == other.pos && color == other.color &&
           texCoord == other.texCoord;
  }
};

namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const {
    return ((hash<glm::vec3>()(vertex.pos) ^
             (hash<glm::vec3>()(vertex.color) << 1)) >>
            1) ^
           (hash<glm::vec2>()(vertex.texCoord) << 1);
  }
};
}  // namespace std

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

class Application2 {
 public:
  Application2() = default;
  void run();

 private:
  GLFWwindow* window_{};

  VkInstance instance_{};
  VkDebugUtilsMessengerEXT debugMessenger_{};
  VkSurfaceKHR surface_{};

  VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
  VkSampleCountFlagBits msaaSamples_ = VK_SAMPLE_COUNT_1_BIT;
  VkDevice device_{};

  VkQueue graphicsQueue_{};
  VkQueue presentQueue_{};

  VkSwapchainKHR swapChain_{};
  std::vector<VkImage> swapChainImages_;
  VkFormat swapChainImageFormat_{};
  VkExtent2D swapChainExtent_{};
  std::vector<VkImageView> swapChainImageViews_;
  std::vector<VkFramebuffer> swapChainFramebuffers_;

  VkRenderPass renderPass_{};
  VkDescriptorSetLayout descriptorSetLayout_{};
  VkPipelineLayout pipelineLayout_{};
  VkPipeline graphicsPipeline_{};

  VkCommandPool commandPool_{};

  VkImage colorImage_{};
  VkDeviceMemory colorImageMemory_{};
  VkImageView colorImageView_{};

  VkImage depthImage_{};
  VkDeviceMemory depthImageMemory_{};
  VkImageView depthImageView_{};

  uint32_t mipLevels_{};
  VkImage textureImage_{};
  VkDeviceMemory textureImageMemory_{};
  VkImageView textureImageView_{};
  VkSampler textureSampler_{};

  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;
  VkBuffer vertexBuffer_{};
  VkDeviceMemory vertexBufferMemory_{};
  VkBuffer indexBuffer_{};
  VkDeviceMemory indexBufferMemory_{};

  std::vector<VkBuffer> uniformBuffers_;
  std::vector<VkDeviceMemory> uniformBuffersMemory_;

  VkDescriptorPool descriptorPool_{};
  std::vector<VkDescriptorSet> descriptorSets_;

  std::vector<VkCommandBuffer> commandBuffers_;

  std::vector<VkSemaphore> imageAvailableSemaphores_;
  std::vector<VkSemaphore> renderFinishedSemaphores_;
  std::vector<VkFence> inFlightFences_;
  std::vector<VkFence> imagesInFlight_;
  size_t currentFrame_ = 0;

  bool framebufferResized_ = false;

  void initWindow();
  static void framebufferResizeCallback(GLFWwindow* window, int width,
                                        int height);
  static void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                          int mods);
  void initVulkan();
  void mainLoop();
  void cleanupSwapChain();
  void cleanup();
  void recreateSwapChain();
  void createInstance();
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();
  void createDescriptorSetLayout();
  void createGraphicsPipeline();
  void createFramebuffers();
  void createCommandPool();
  void createColorResources();
  void createDepthResources();
  VkFormat findDepthFormat();
  VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);
  bool hasStencilComponent(VkFormat format);
  void createTextureImage();
  void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth,
                       int32_t texHeight, uint32_t mipLevels);
  VkSampleCountFlagBits getMaxUsableSampleCount();
  void createTextureImageView();
  void createTextureSampler();
  VkImageView createImageView(VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags,
                              uint32_t mipLevels);
  void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                   VkSampleCountFlagBits numSamples, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkImage& image,
                   VkDeviceMemory& imageMemory);
  void transitionImageLayout(VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout,
                             uint32_t mipLevels);
  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height);
  void loadModel();
  void createVertexBuffer();
  void createIndexBuffer();
  void createUniformBuffers();
  void createDescriptorPool();
  void createDescriptorSets();
  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer& buffer,
                    VkDeviceMemory& bufferMemory);
  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer commandBuffer);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  void createCommandBuffers();
  void createSyncObjects();
  void updateUniformBuffer(uint32_t currentImage);
  void drawFrame();
  VkShaderModule createShaderModule(const std::vector<char>& code);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
  bool isDeviceSuitable(VkPhysicalDevice device);
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  std::vector<const char*> getRequiredExtensions();
  bool checkValidationLayerSupport();
  static std::vector<char> readFile(const std::string& filename);
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData);
};

#endif  // VULKANTEST_APPLICATION2_HPP
