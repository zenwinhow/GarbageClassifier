#ifndef COCOMAP_H
#define COCOMAP_H

#include <string>
#include <unordered_map>

// CocoMap 类，用于将 COCO 类别名称映射到垃圾分类类型
class CocoMap
{
public:
    // 获取垃圾分类结果，name 为 COCO 类别名称
    static std::string getGarbageType(const std::string &name);

private:
    // 静态映射表，存储 COCO 类别到垃圾分类的映射
    static const std::unordered_map<std::string, std::string> map_;
};

#endif // COCOMAP_H
