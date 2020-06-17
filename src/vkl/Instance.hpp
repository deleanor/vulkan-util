#ifndef vkl_Instance_hpp
#define vkl_Instance_hpp

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace vkl {

class Instance {
public:
    Instance();
    ~Instance();

    std::vector< std::string > physicalDevices() const;

private:
    VkInstance mInstance;
};

} // namespace vkl

#endif // include guard