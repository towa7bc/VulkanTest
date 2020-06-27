add_executable(
        VulkanTest
        main.cpp
        stb_image.cpp
        stb_image.hpp
        #Application.cpp
        #Application.hpp
        Application2.cpp
        Application2.hpp
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
        Vulkan::Vulkan
)
