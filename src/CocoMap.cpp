#include "CocoMap.h"

// 静态成员变量，存储 COCO 类别到垃圾分类的映射
const std::unordered_map<std::string, std::string> CocoMap::map_ = {
    // continue类
    { "person", "continue" }, // 人
    { "bicycle", "continue" }, // 交通工具
    { "car", "continue" },
    { "motorbike", "continue" },
    { "aeroplane", "continue" },
    { "bus", "continue" },
    { "train", "continue" },
    { "truck", "continue" },
    { "boat", "continue" },
    { "traffic light", "continue" },
    { "fire hydrant", "continue" },
    { "stop sign", "continue" },
    { "parking meter", "continue" },
    { "bench", "continue" },
    { "bird", "continue" },
    { "cat", "continue" },
    { "dog", "continue" },
    { "horse", "continue" },
    { "sheep", "continue" },
    { "cow", "continue" },
    { "elephant", "continue" },
    { "bear", "continue" },
    { "zebra", "continue" },
    { "giraffe", "continue" },
    { "backpack", "Residual waste" }, // 生活用品
    { "umbrella", "Residual waste" },
    { "handbag", "Residual waste" },
    { "tie", "Residual waste" },
    { "suitcase", "Residual waste" },
    { "frisbee", "continue" },
    { "skis", "continue" },
    { "snowboard", "continue" },
    { "sports ball", "continue" },
    { "kite", "continue" },
    { "baseball bat", "continue" },
    { "baseball glove", "continue" },
    { "skateboard", "continue" },
    { "surfboard", "continue" },
    { "tennis racket", "continue" },
    { "bottle", "Recyclable waste" }, // 可回收
    { "wine glass", "Recyclable waste" },
    { "cup", "Recyclable waste" },
    { "fork", "Recyclable waste" },
    { "knife", "Recyclable waste" },
    { "spoon", "Recyclable waste" },
    { "bowl", "Recyclable waste" },
    { "banana", "Food waste" }, // 食物
    { "apple", "Food waste" },
    { "sandwich", "Food waste" },
    { "orange", "Food waste" },
    { "broccoli", "Food waste" },
    { "carrot", "Food waste" },
    { "hot dog", "Food waste" },
    { "pizza", "Food waste" },
    { "donut", "Food waste" },
    { "cake", "Food waste" },
    { "chair", "continue" },
    { "sofa", "continue" },
    { "pottedplant", "continue" },
    { "bed", "continue" },
    { "diningtable", "continue" },
    { "toilet", "continue" },
    { "tvmonitor", "continue" },
    { "laptop", "Hazardous waste" }, // 电子垃圾
    { "mouse", "Hazardous waste" },
    { "remote", "Hazardous waste" },
    { "keyboard", "Hazardous waste" },
    { "cell phone", "Hazardous waste" },
    { "microwave", "Hazardous waste" },
    { "oven", "Hazardous waste" },
    { "toaster", "Hazardous waste" },
    { "sink", "continue" },
    { "refrigerator", "Hazardous waste" },
    { "book", "Recyclable waste" }, // 纸制品
    { "clock", "Hazardous waste" },
    { "vase", "Recyclable waste" },
    { "scissors", "Recyclable waste" },
    { "teddy bear", "Residual waste" }, // 玩具
    { "hair drier", "Hazardous waste" },
    { "toothbrush", "Residual waste" }, // 生活用品
    // 新类别继续向下补充（根据你的coco.names完整列表自动填充）
};

// 根据 COCO 类别名称获取垃圾分类类型
std::string CocoMap::getGarbageType(const std::string& name)
{
    auto it = map_.find(name);
    if (it != map_.end())
        return it->second; // 返回对应的垃圾分类
    return "continue"; // 默认返回“continue”
}