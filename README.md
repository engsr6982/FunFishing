# FunFishing - 趣味钓鱼

此插件可以为钓鱼添加自定义物品、生物。

> 比如：玩家钓上来个 苦力怕

## 下载/安装

- Lib

```bash
lip install github.com/engsr6982/FunFishing
```

- 手动

下载 `FunFishing-windows-x64.zip` 解压后把 `FunFishing` 文件夹放入 `plugins` 文件夹下

## 配置文件

```json
{
  "version": 1,
  "lists": [
    {
      "type": "None", // 生成类别 None 为兼容原版物品而保留、Mob 生成实体、Item 生成物品
      "probability": 0.20000000298023224, // 生成概率 0.01 ~ 1.0 (大于1.0还钓什么鱼，直接送吧)
      "data": "", // 数据，填写对应物品、生物命名空间或SNBT（SNBT仅限Item有效）
      "isSNBT": false // 数据是否为SNBT(仅Item有效)
    },
    {
      "type": "Mob", // 生成实体
      "probability": 0.15000000596046448, // 0.15的概率（此处由于精度问题，导致默认配置文件浮点数不准）
      "data": "minecraft:creeper", // 苦力怕
      "isSNBT": false
    },
    {
      "type": "Item",
      "probability": 0.05,
      "data": "minecraft:clock",
      "isSNBT": false
    }
  ]
}
```
