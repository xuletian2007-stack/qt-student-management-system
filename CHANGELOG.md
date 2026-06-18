# Changelog

## [1.0.0] - 2026-05-28

### 新增
- 用户登录/注册系统（学生 & 管理员）
- 批量导入课程与成绩（CSV/JSON 解析）
- GPA 统计分析与折线图可视化
- 成就记录（奖项、角色、志愿者经历）
- 标准简历导出（含证明材料路径）
- AI 简历增强（DeepSeek API → HTML 网页）
- 数据备份与恢复（加密/压缩）


### 架构
- MVC 三层分离：Model / View / Controller + Service
- QStackedWidget + QTabBar 导航
- DatabaseManager（单例）本地 JSON 持久化
