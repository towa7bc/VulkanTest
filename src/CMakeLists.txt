add_executable(
        VulkanTest
        main.cpp
        stb_image.cpp
        stb_image.hpp
        Application.cpp
        Application.hpp
        imgui_impl_glfw.cpp
        imgui_impl_glfw.h
        imgui_impl_vulkan.cpp
        imgui_impl_vulkan.h
)

find_package(Vulkan REQUIRED)

target_link_libraries(
        VulkanTest
        PRIVATE project_options
        project_warnings
        CONAN_PKG::catch2
        CONAN_PKG::spdlog
        CONAN_PKG::glm
        CONAN_PKG::glfw
        CONAN_PKG::assimp
        CONAN_PKG::imgui
        Vulkan::Vulkan
)
