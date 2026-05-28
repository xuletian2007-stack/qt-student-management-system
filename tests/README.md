# 测试目录

## 单元测试计划

由于项目使用 Qt 5.9.1，推荐使用 QTest 框架编写单元测试。

### 待测模块

| 模块 | 测试内容 |
|------|---------|
| GpaCalculator | 标准成绩 → GPA 计算、边界值（0分/100分）、空课程列表 |
| DatabaseManager | CRUD 操作、文件读写、并发安全 |
| Achievement | 构造函数、getter/setter、toLine 格式化 |
| StudentProfile | 聚合根完整性、添加/删除子对象 |
| ImportController | CSV/JSON 解析、数据校验（成绩范围0-100） |

### 运行测试

```bash
cd tests
qmake tests.pro
make
./tests
```
