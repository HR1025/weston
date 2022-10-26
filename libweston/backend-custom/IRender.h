/**
 * @file     IRender.h
 * @author   haorui
 * @brief    渲染器类型抽象
 * @version  0.1
 * @date     2022-10-21     
 * 
 * @note     IRender 对渲染器进行了高度抽象,以模板的方式约束具体渲染器的逻辑行为;
 *           为了确保衍生渲染器可以定制自己需要的特定内容,使用 key-value 的方式
 *           供用户设置 opaque 数据
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#pragma once

#include <cstdint>
#include <string>
#include <set>
#include <memory>

#include "CustomDefine.h"

namespace weston
{

class IRender
{
public:
    static std::shared_ptr<IRender> Create(RenderType renderType);
public:
    virtual ~IRender() = default;
public:
    /**
     * @brief   初始化
     * @return  true 代表成功
     */
    virtual bool Init()     =  0;
    /**
     * @brief    重置
     * @return   true 代表成功
     */
    virtual bool Uinit()    =  0;
    /**
     * @brief    开始
     * @return   true 代表成功
     */
    virtual bool Enable()   =  0;
    /**
     * @brief    关闭
     * @return   true 代表成功
     */
    virtual bool Disable()  =  0;
    /**
     * @brief    重绘
     * @return   damage : 损坏数据,即需要重绘的数据
     */
    virtual bool Repaint(pixman_region32_t *damage) = 0;
public:
    /**
     * @brief      设置 opaque 数据
     * @param[in]  key : 索引值, 用于衍生类判断数据类型
     * @param[in]  opaque
     */
    virtual void  SetOpaqueData(std::string key, void* opaque) = 0;
    /**
     * @brief      获取 opaque 数据
     * @param[in]  key : 索引值, 用于衍生类判断数据类型
     * @return     若成功则非空
     */
    virtual void const* GetOpaqueData(std::string key) = 0;
    /**
     * @brief       获取支持的 opaque 类型
     */
    virtual std::set<std::string /* key */> GetOpaqueOptions() = 0;
};

} // namespace weston