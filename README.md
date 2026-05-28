# 个人发展规划网 (PersonalDevPlan)

> 基于 Qt/C++ 的学生个人发展管理系统 — 记录学习历程、管理成就、生成简历

## 📋 项目简介

帮助大学生记录和追踪四年学习历程，围绕个人成长与发展需求，对学习、能力、经历与未来目标进行记录、分析、规划与展示。

**体系结构风格：MVC（模型-视图-控制器）**

## ✅ 功能完成

- [x] 批量导入课程与成绩（CSV/JSON）
- [x] GPA 统计分析与学期趋势图
- [x] 成就记录（奖项、班级角色、志愿者）
- [x] 标准简历导出
- [x] AI 简历增强（DeepSeek API → HTML 网页）
- [x] 数据备份与恢复
- [x] 用户登录/注册（学生 & 管理员）

## 🏗️ 项目结构

```
├── README.md
├── LICENSE
├── CHANGELOG.md
├── src/
│   ├── PersonalDevPlan.pro    # Qt 项目文件
│   ├── main.cpp               # 入口
│   ├── model/                 # 数据模型层
│   ├── view/                  # 视图层 (Qt UI)
│   ├── controller/            # 控制器层
│   ├── service/               # AI 服务层
│   └── icons/                 # 图标资源
├── docs/
│   ├── 设计文档.md            # 完整设计文档
│   ├── 用例图.jpeg
│   ├── 类图.jpeg
│   └── 时序图.png
└── tests/
    └── README.md              # 测试计划
```

## 🛠️ 技术栈

- **语言：** C++ (C++11)
- **框架：** Qt 5.9.1
- **编译器：** Clang (macOS) / MinGW 32-bit (Windows)
- **AI 服务：** DeepSeek API
- **版本管理：** Git + GitHub

## 🚀 编译运行

### macOS
```bash
cd src
qmake PersonalDevPlan.pro
make
open PersonalDevPlan.app
```

### Windows (MinGW)
```bash
cd src
qmake PersonalDevPlan.pro
mingw32-make
release\PersonalDevPlan.exe
```

## 👥 分工

| 成员 | 角色 | 职责 |
|------|------|------|
| 徐乐天 | System Architect | MVC 交互协议设计、接口规范 |
| — | Model (Logic) | GPA 计算模型与算法实现 |
| — | Model (Data) | 成就/角色数据模型、CRUD、持久化 |
| — | Service/Controller | AI 业务流控制、API 调用、结果反馈 |
| — | View & Controller | Qt 界面开发、信号槽逻辑 |

## 📄 文档

详见 [docs/设计文档.md](docs/设计文档.md)
